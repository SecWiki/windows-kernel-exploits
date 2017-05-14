#!/usr/bin/python

# Author
# ------
# Sylvain Monne
# Contact : sylvain dot monne at solucom dot fr
# http://twitter.com/bidord

from struct import unpack, pack
from time import time, gmtime, strftime, strptime, localtime
from calendar import timegm


def gt2epoch(gt):
    return timegm(strptime(gt, '%Y%m%d%H%M%SZ'))

def epoch2gt(epoch=None, microseconds=False):
    if epoch is None:
        epoch = time()
    gt = strftime('%Y%m%d%H%M%SZ', gmtime(epoch))
    if microseconds:
        ms = int(epoch * 1000000) % 1000000
        return (gt, ms)
    return gt

def epoch2filetime(epoch=None):
    if epoch is None:
        epoch = time()
    return pack('Q', int((epoch + 11644473600) * 10000000))

def filetime2local(s):
    t = unpack('Q', s)[0]
    if t == 0x7fffffffffffffff:
        return 'NEVER'
    if t == 0:
        return 'NULL'
    secs = t / 10000000 - 11644473600
    digits = t % 10000000
    return "%s.%07d" % (strftime('%Y/%m/%d %H:%M:%S', localtime(secs)), digits)

def bitstring2int(bs):
    return sum(b << i for i, b in enumerate(reversed(bs)))


