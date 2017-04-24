#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
# Windows Exploit Suggester
# revision 3.3, 2017-02-13
#
# author: Sam Bertram, Gotham Digital Science
# contact: labs@gdssecurity.com,sbertram@gdssecurity.com,sammbertram@gmail.com
# blog post: "Introducing Windows Exploit Suggester", http://blog.gdssecurity.com/
# 
# DESCRIPTION
# 
# This tool compares a targets patch levels against the Microsoft vulnerability
# database in order to detect potential missing patches on the target. It also
# notifies the user if there are public exploits and Metasploit modules
# available for the missing bulletins.
#
# It requires the 'systeminfo' command output from a Windows host in order to
# compare that the Microsoft security bulletin database and determine the 
# patch level of the host.
#
# It has the ability to automatically download the security bulletin database
# from Microsoft with the --update flag, and saves it as an Excel spreadsheet.
#
# When looking at the command output, it is important to note that it assumes
# all vulnerabilities and then selectively removes them based upon the hotfix
# data. This can result in many false-positives, and it is key to know what
# software is actually running on the target host. For example, if there are
# known IIS exploits it will flag them even if IIS is not running on the
# target host.
#
# The output shows either public exploits (E), or Metasploit modules (M) as
# indicated by the character value. 
#
# It was heavily inspired by Linux_Exploit_Suggester by Pentura.
#
# Blog Post: "Introducing Windows Exploit Suggester", https://blog.gdssecurity.com/labs/2014/7/11/introducing-windows-exploit-suggester.html
#
# USAGE
# 
# update the database
#
# $ ./windows-exploit-suggester.py --update
# [*] initiating...
# [*] successfully requested base url
# [*] scraped ms download url
# [+] writing to file 2014-06-06-mssb.xlsx
# [*] done
#
# install dependencies
#
# (install python-xlrd, $ pip install xlrd --upgrade)
#
# feed it "systeminfo" input, and point it to the microsoft database
#
# $ ./windows-exploit-suggester.py --database 2014-06-06-mssb.xlsx --systeminfo win7sp1-systeminfo.txt 
# [*] initiating...
# [*] database file detected as xls or xlsx based on extension
# [*] reading from the systeminfo input file
# [*] querying database file for potential vulnerabilities
# [*] comparing the 15 hotfix(es) against the 173 potential bulletins(s)
# [*] there are now 168 remaining vulns
# [+] windows version identified as 'Windows 7 SP1 32-bit'
# [*] 
# [M] MS14-012: Cumulative Security Update for Internet Explorer (2925418) - Critical
# [E] MS13-101: Vulnerabilities in Windows Kernel-Mode Drivers Could Allow Elevation of Privilege (2880430) - Important
# [M] MS13-090: Cumulative Security Update of ActiveX Kill Bits (2900986) - Critical
# [M] MS13-080: Cumulative Security Update for Internet Explorer (2879017) - Critical
# [M] MS13-069: Cumulative Security Update for Internet Explorer (2870699) - Critical
# [M] MS13-059: Cumulative Security Update for Internet Explorer (2862772) - Critical
# [M] MS13-055: Cumulative Security Update for Internet Explorer (2846071) - Critical
# [M] MS13-053: Vulnerabilities in Windows Kernel-Mode Drivers Could Allow Remote Code Execution (2850851) - Critical
# [M] MS13-009: Cumulative Security Update for Internet Explorer (2792100) - Critical
# [M] MS13-005: Vulnerability in Windows Kernel-Mode Driver Could Allow Elevation of Privilege (2778930) - Important
# [*] done
#
# possible exploits for an operating system can be used without hotfix data
# $ ./windows-exploit-suggester.py --database 2014-06-06-mssb.xlsx --ostext 'windows server 2008 r2' 
# [*] initiating...
# [*] database file detected as xls or xlsx based on extension
# [*] getting OS information from command line text
# [*] querying database file for potential vulnerabilities
# [*] comparing the 0 hotfix(es) against the 196 potential bulletins(s)
# [*] there are now 196 remaining vulns
# [+] windows version identified as 'Windows 2008 R2 64-bit'
# [*] 
# [M] MS13-009: Cumulative Security Update for Internet Explorer (2792100) - Critical
# [M] MS13-005: Vulnerability in Windows Kernel-Mode Driver Could Allow Elevation of Privilege (2778930) - Important
# [E] MS11-011: Vulnerabilities in Windows Kernel Could Allow Elevation of Privilege (2393802) - Important
# [M] MS10-073: Vulnerabilities in Windows Kernel-Mode Drivers Could Allow Elevation of Privilege (981957) - Important
# [M] MS10-061: Vulnerability in Print Spooler Service Could Allow Remote Code Execution (2347290) - Critical
# [E] MS10-059: Vulnerabilities in the Tracing Feature for Services Could Allow Elevation of Privilege (982799) - Important
# [E] MS10-047: Vulnerabilities in Windows Kernel Could Allow Elevation of Privilege (981852) - Important
# [M] MS10-002: Cumulative Security Update for Internet Explorer (978207) - Critical
# [M] MS09-072: Cumulative Security Update for Internet Explorer (976325) - Critical
#
# TROUBLESHOOTING
#
# If you're receiving the following error message, update the xlrd library
# $ pip install xlrd --update
#
# [*] initiating winsploit version 24...
# [*] database file detected as xls or xlsx based on extension
# Traceback (most recent call last):
# 	  File "windows-exploit-suggester/windows-exploit-suggester.py", line 1414, in <module>
# 	      main()
# 	        File "windows-exploit-suggester/windows-exploit-suggester.py", line 354, in main
# 		    wb = xlrd.open_workbook(ARGS.database)
# 		      File "/usr/lib/pymodules/python2.7/xlrd/__init__.py", line 370, in open_workbook
# 		          biff_version = bk.getbof(XL_WORKBOOK_GLOBALS)
# 			    File "/usr/lib/pymodules/python2.7/xlrd/__init__.py", line 1323, in getbof
# 			        raise XLRDError('Expected BOF record; found 0x%04x' % opcode)
# 			xlrd.biffh.XLRDError: Expected BOF record; found 0x4b50
#
# LIMITATIONS
#
# Currently, if the 'systeminfo' command reveals 'File 1' as the output for
# the hotfixes, it will not be able to determine which are installed on
# the target. If this occurs, the list of hotfixes will need to be 
# retrieved from the target host and passed in using the --hotfixes flag
#
# It currently does not seperate 'editions' of the Windows OS such as
# 'Tablet' or 'Media Center' for example, or different architectures, such as
# Itanium-based only
#
# False positives also occur where it assumes EVERYTHING is installed
# on the target Windows operating system. If you receive the 'File 1'
# output, try executing 'wmic qfe list full' and feed that as input
# with the --hotfixes flag, along with the 'systeminfo'
#
# LICENSE
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# TODOLIST
#
# TODO better if/then/case when detecting OS. more flexibility with parsing
#     different systeminfo output
# TODO seperate by editions? may result in false positives
# TODO count the number of exploits in the summary prior to outputting it?
# TODO finish -s --search function so that all info on an MS number can be
#      returned
# TODO add titles to exploit list so that it is more portable
# TODO test for Windows RT systeminfo output
# TODO improved msf/poc output? perhaps adding details on each MS number?
# TODO if it's running on windows, then try and execute the systeminfo command?
# TODO SPEED. this is now way too slow...  somewhat improved!
# TODO automatically install python module? xlrd.
# TODO manually override MS11-011 for Non-Affected Products. The bulletin
# database is wrong.
#  Windows 7 for 32-bit Systems Service Pack 1
#  Windows 7 for x64-based Systems Service Pack 1
#  Windows Server 2008 R2 for x64-based Systems Service Pack 1
#  Windows Server 2008 R2 for Itanium-based Systems Service Pack 1
#
# CHANGE LOG
# v33 2017-02-13
# - added links to exploits and resources for each bulletins. can be ignored with the -q/--quiet flag
# - hard coded ms11-011 to ignore false positives
# - added additional resources
#
# v31 2016-02-10
# - changed bulletin url, microsoft 404'd it
#	
# v30 2016-01-04
# - added exploits and bulletins from the past six months
#
# v29 2015-09-16
# - adding support for windows 10
#
# v28 2015-07-30
# - added bulletin scraping for xlsx and xls files using regex. thanks to
#   edebernis for reporting the bug
# - added ms15-022, ms15-015 update to msf
#
# v27 2015-06-18
# - added new bulletin url that is only xls and not xlsx. thanks to bstork for
#   reporting the bug
# - added ms15-010, ms15-051, and ms15-052
#
# v26 2015-06-02
# - small bug fix with linked output
# - added duplicates flag that can allow for bulletins to be displayed
#   multiple times. this will allow for greater analysis on linked bulletins
#
# v25 2015-05-18
# - added ms15-051 local priv
#
# v24 2015-01-30
# - added --sub/-s command in order to display output of msids as linked
#   this aides in demonstrating what patches need to be applied precisely. 
#   this change was implemented in v23, but only followed the depth to level
#   1 instead of the entire way.
# - fixed a bug that know allows for multiple supercedes msids in the db
# - allowed for getarchitecture to be recursive, and reduced redunancy when
#   it is called throughout the program
# - added ms14-070
#
# v23 2015-01-26
# - typo in --local flag case (pontential vs potential). issue #5 closed. 
#
# v22 2015-01-23
# - speed optimisations! it was too slow beforehand. realised i could easily
#   make it a bit more efficient
# 
# v21 2015-01-22
# - changed display formatting to include nested/linked MS numbers. makes it
#   easier to determine the dependencies
# - made args global
# - changed some code formatting, including double-space instead of \t
# - added some additional comments
# - disable ANSI output if on windows platform 
# - added recent exploits
#
# v20 2014-12-16
# - added ms14-068,ms14-064,ms14-060, and ms14-058 to the internal vuln list
#
# v19 2014-10-08
# - added support for windows server 2012, this includes ignoring the
#   architecture for 2012, and forcing from 32-bit to 64-bit
#
# v18 2014-09-02
# - added ms14-029 poc
#
# v17 2014-08-05
# - fixed a bug where it would not detect OS version when a unicode char comes
#   before search string
#
# v16 2014-07-28
# - improved reading of various file encodings for systeminfo. now attempts to 
#   detect the file first, otherwise loops through common encodings
# - improved OS, service pack, architecture, and release detection. this is now
#   not English-dependent as it was previously
# - better architecture detection of systeminfo input (look for -based string)
# - added /usr/bin/env python
# - added ms14-035 poc
#
# v15 2014-07-15
# - changed file open to io, and attempt to decode as utf-8; otherwise attempt
#   utf-16
#
# v14 2014-07-13
# - allowed for --ostext flag to properly supersede OS detection of systeminfo
#   input
#
# v13a 2014-07-01
# - added new msf flags for ms13-097, and ms14-009
#
# v12a 2014-06-06
# - quick cleanup for release
#
# v11a 2014-05-02
# - fixed the bulletin scrape regex for the update command. ms changed it
#
# v10a 2014-03-24
# - added a hotfixes argument, that can be used to supplement the list
#  of hotfixes detected in the systeminfo input
# - added severity at the end of the output when reporting bulletins
# - added a 'patches' argument, that can be used to determine any
#  of the hotfixes for a specific bulletin. this is good for debugging.
#
# v09a 2014-03-18
# - again, another massive bug on the linked kb searching function
#   getlinkedms(). should be fixed now
# - also checks columns 11 and 12 for superseded, i think it has to
#   do with dos and *nix output
#
# v08a 2014-02-14
# - bug where the superseded column wasn't being checked
#   this may be because it's only xlsx and it parsed differently in csv
# - added some new exploits from edb
#
# v07a 2014-02-12
# - added indicator for os version, and in green
# - better parsing of architecture for itanium based support
#
# v06a 2014-01-19
# - added 'ostext' or 'o' option, when don't have any patch information
#   but just know the OS
#
# v05a
# - added a check for "Kernel version" column, as well as "OS version"
#
# v04a
# - added support for XLSX files directly with the updated XLRD library, this
#   requires the python-xlrd library to be installed and upgraded with:
#   $ pip install xlrd --upgrade
# - changed MS13-101 to E, as there isn't a metasploit module (yet!)
#
# v03a
# - fixed an issue where component KB wasn't being checked
#
# FUNCTIONS
#
# def main():
# def run(database):
# def detect_encoding(filename):
# def trace(database):
# def patches(database):
# def getversion(name, release, servicepack, architecture):
# def getname(ostext):
# def getrelease(ostext):    
# def getservicepack(ostext):
# def getarchitecture(ostext):
# def getitanium(ostext):
# def getpatch(ostext):
# def getbulletinids(haystack):
# def isaffected(name, release, servicepack, architecture, haystack):
# def getlinkedms(msids, database):
# def getexploit(msid = 0):
# def update():
# def merge_list(li):
#
import re
import platform
import argparse
import subprocess
import csv
import StringIO
import os
import datetime
import urllib2
import io
from random import randint
from time import sleep
from tempfile import NamedTemporaryFile
from sys import exit

# constants/globals
MSSB_URL = 'http://www.microsoft.com/en-gb/download/confirmation.aspx?id=36982'
BULLETIN_URL = 'http://download.microsoft.com/download/6/7/3/673E4349-1CA5-40B9-8879-095C72D5B49D/BulletinSearch.xlsx'
VERSION = "3.3"

# global parser
parser = argparse.ArgumentParser(description="search microsoft security bulletins for exploits based upon the patch level of the machine by feeding in systeminfo command")
parser.add_argument("-v", "--verbose", help="verbose output", action="store_true")
parser.add_argument("-i", "--systeminfo", help="feed in an input file that contains the 'systeminfo' command")
parser.add_argument("-d", "--database", help="the file that contains the microsoft security bulletin database")
parser.add_argument("-u", "--update", help="required flag to even run the script", action="store_true")
parser.add_argument("-a", "--audit", help="show all entries, not only exploits", action="store_true")
parser.add_argument("-t", "--trace", help="used to determine linked ms bulletins")
parser.add_argument("-p", "--patches", help="used to determine specific patches for a ms bulletin")
parser.add_argument("-o", "--ostext", help="a loose text representation of the windows OS (ex: \"windows xp home edition sp2\")")
parser.add_argument("-s", "--sub", help="generate output using linked/sub bulletins. WARNING: SLOW!", action="store_true")
parser.add_argument("-2", "--duplicates", help="allow duplicate ms bulletin output within the results. this will produce a lot of output, but is useful when determining linked ms bulletins", action="store_true")
parser.add_argument("-q", "--quiet", help="don't show exploit information. shorter output", action="store_true")
# hotfixes
# used to parse "wmic qfe list full" input, and to solve the 'File 1' errors
parser.add_argument("-H", "--hotfixes", help="a loose list of hotfixes to be added, for use with the following command: 'wmic qfe list full'")

# search by exploit type only
exptypegroup = parser.add_mutually_exclusive_group()
exptypegroup.add_argument("-r", "--remote", help="search remote exploits only", action="store_true")
exptypegroup.add_argument("-l", "--local", help="search local exploits only", action="store_true")

# global args parsed
ARGS = parser.parse_args()

def main():
  ALERT("initiating winsploit version %s..." % VERSION)

  database = ''

  # if there is a database switch
  if ARGS.database:

    # split name and extension
    name, extension = os.path.splitext(ARGS.database)

    # csv
    if 'csv' in extension:

      ALERT("database file detected as csv based on extension", ALERT.NORMAL)

      # attempt to open the file
      try:
        dbfile = open(ARGS.database, 'r')

      except IOError, e:
        ALERT("could not open the file %s" % filename, ALERT.BAD)
        exit(1)

      data = ''
      for line in dbfile:
        data += line
      database = data

      dbfile.close()

    # xls or xslx
    elif 'xls' in extension:

      ALERT("database file detected as xls or xlsx based on extension", ALERT.NORMAL)

      try:
        import xlrd
      except ImportError as e:
        ALERT("please install and upgrade the python-xlrd library", ALERT.BAD)
        exit(1)

      # open the xls file
      try:
        wb = xlrd.open_workbook(ARGS.database)
      except IOError as e:
        ALERT("no such file or directory '%s'. ensure you have the correct database file passed in --database/-d" % ARGS.database, ALERT.BAD)
        exit(1)
      #sh = wb.sheet_by_name('Export Bulletin Search Spreadsh')
      sh = wb.sheet_by_index(0)

      # read the spreadsheet into a temp file
      f = NamedTemporaryFile(mode='wb')
      wr = csv.writer(f, quoting=csv.QUOTE_NONE, delimiter=',')

      data = ''

      # loop through xls
      for rownum in xrange(sh.nrows):

        values = sh.row_values(rownum)

        # loop through row values, and process input
        for i in range(len(values)):
          values[i] = unicode(values[i]).encode('utf8')
          values[i] = values[i].replace('\n',' ')
          values[i] = values[i].replace(',','')
          values[i] = values[i].replace('.0','')

        data += ",".join(values)
        data += '\n'
  
      # set the database to the csv data
      database = data

    # unknown filetype, error
    else:
      ALERT("unknown filetype. change file extension to indicate csv or xls/xlsx", ALERT.BAD)
      exit(1)

  if ARGS.trace: trace(database)
  elif ARGS.systeminfo or ARGS.ostext: run(database)
  elif ARGS.update: update()
  elif ARGS.patches: patches(database)

  # error
  else:
    ALERT("an error occured while running, not enough arguments", ALERT.BAD)
    exit(1)

  ALERT("done")
  # end main()

def run(database):

  # variables used
  ostext=None
  name=None
  release=None
  servicepack=None
    
  # will default to 32-bit, but can be 64 bit or itanium
  architecture=None

  hotfixes=set([])
  bulletinids=set([])

  potential=[]
  
  vulns={}
  ids=set([])

  cmdoutput = []

  # test for database
  if not ARGS.database:
    ALERT("please supply a MSSB database file with the --database or -d flag, this can be downloaded using the --update command", ALERT.BAD)
    exit(1)

  # read from ostext first
  if ARGS.ostext:
    ALERT("getting OS information from command line text")
        
    name=getname(ARGS.ostext)
    release=getrelease(ARGS.ostext)
    servicepack=getservicepack(ARGS.ostext)
    architecture=getarchitecture(ARGS.ostext)
    
    # the os name at least has to be identified
    if not name:
      ALERT("unable to determine the windows version command line text from '%s'" % ARGS.ostext, ALERT.BAD)
      exit(1)

  # get the systeminfo information from the input file
  if ARGS.systeminfo:

    ALERT("attempting to read from the systeminfo input file")

    # when reading the systeminfo file, we want to attempt to detect it using chardet
    # if this doesn't work, we will loop through a list of common encodings and try them all
    encodings = ['utf-8', 'utf-16', 'utf-16-le', 'utf-16-be', 'iso-8859-2']

    detected_encoding =  detect_encoding(ARGS.systeminfo)

    # insert detected encoding to the front of the list
    if detected_encoding: 
      if ARGS.verbose: ALERT("detected encoding of file as '%s'" % detected_encoding)
      encodings.insert(0, detected_encoding)

    cmdfile = None
    cmdoutput = None
    
    # now loop through all encodings, with the detected one first (if it was possible)
    for encoding in encodings:

      if ARGS.verbose: ALERT("  attempting to read with '%s' encoding" % encoding)          

      # if we can read the file, and read the command output, we are done with the loop
      try: 
        cmdfile = io.open(ARGS.systeminfo, "r", encoding=encoding) # throws UnicodeDecodeError      
        cmdoutput = cmdfile.readlines() # throws UnicodeError
        break

      except (UnicodeError, UnicodeDecodeError) as e:
        ALERT("could not read file using '%s' encoding: %s" % (encoding, e), ALERT.BAD)
  
      # file might not exist
      except:
        ALERT("could not read from input file specified: %s" % ARGS.systeminfo, ALERT.BAD)
        exit(1)  

    # general catchall if somehow it was able to keep processing
    if not cmdfile or not cmdoutput:
      ALERT("could not read from input file, or could not detect encoding", ALERT.BAD)
      exit(1)
    
    # file read successfully
    ALERT("systeminfo input file read successfully (%s)" % encoding, ALERT.GOOD)

  # error
  if not ARGS.systeminfo and not ARGS.ostext and platform.system() != 'Windows':
    ALERT("please run from a Windows machine, or provide an input file using --systeminfo, or use the --ostext option to get data with no patch information", ALERT.BAD)
    exit(1)

  # parse the systeminfo information
  hotfix=False

  # loop through the systeminfo input
  for haystack in cmdoutput:

    # only attempt to set the version, arch, service pack if there is no
    # ostext flag
    if not ARGS.ostext:

      # when detecting the operating system version, every line (independent of language)
      # appears to have Microsoft Windows in it, sometimes with (R)
      if "Microsoft" in haystack and "Windows" in haystack and not name:
        name = getname(haystack)

      # the windows release is similar to the above and has the text 'Microsoft Windows' in the text
      if "Microsoft" in haystack and "Windows" in haystack and not release:
        release = getrelease(haystack)

      # similar to OS, there is the words 'Service Pack' 
      if "Service Pack" in haystack and not servicepack:
        servicepack = getservicepack(haystack)
      
      # get architecture only if -based is in the line, and --ostext hasn't been used
      if "-based" in haystack and not architecture: 
        architecture=getarchitecture(haystack)

    # look for kbs
    if ("KB" in haystack or "]: " in haystack):
      patch=getpatch(haystack)
      
      # if a patch was parsed
      if patch:
        if ARGS.verbose: ALERT("found hotfix %s" % patch)
        hotfixes.add(patch)

  # now process the hotfixes argument input
  if ARGS.hotfixes:

    encodings = ['utf-8', 'utf-16', 'utf-16-le', 'utf-16-be', 'iso-8859-2']

    detected_encoding =  detect_encoding(ARGS.systeminfo)

    # insert detected encoding to the front of the list
    if detected_encoding: 
      if ARGS.verbose: ALERT("detected encoding of file as '%s'" % detected_encoding)
      encodings.insert(0, detected_encoding)

    cmdfile = None
    hotfixesfile = None
    
    # now loop through all encodings, with the detected one first (if it was possible)
    for encoding in encodings:

      if ARGS.verbose: ALERT("  attempting to read with '%s' encoding" % encoding)          

      # if we can read the file, and read the command output, we are done with the loop
      try: 
        cmdfile = io.open(ARGS.hotfixes, "r", encoding=encoding) # throws UnicodeDecodeError      
        hotfixesfile = cmdfile.readlines() # throws UnicodeError
        break

      except (UnicodeError, UnicodeDecodeError) as e:
        if ARGS.verbose: ALERT("could not read file using '%s' encoding: %s" % (encoding, e), ALERT.BAD)
  
      # file might not exist
      except:
        ALERT("could not read from input file specified: %s" % ARGS.hotfixes, ALERT.BAD)
        exit(1)  

    # general catchall if somehow it was able to keep processing
    if not cmdfile or not hotfixesfile:
      ALERT("could not read from input file, or could not detect encoding", ALERT.BAD)
      exit(1)

    # file read successfully
    ALERT("hotfixes input file read successfully (%s)" % encoding, ALERT.GOOD)
    
    # loop through hotfixes file input
    for haystack in hotfixesfile:
      # look for kbs
      if ("KB" in haystack or "]: " in haystack):
        patch=getpatch(haystack)
        
        # if a patch was parsed
        if patch:
          if ARGS.verbose: ALERT("found hotfix %s" % patch)
          hotfixes.add(patch)
        
  if ARGS.verbose:
    ALERT("name: %s; release: %s; servicepack: %s; architecture: %s" % (name, release, servicepack, architecture))

  # verify that a windows os was at least able to be parsed
  if not name:
    if ARGS.systeminfo:
      ALERT("unable to determine the windows versions from the input file specified. consider using --ostext option to force detection (example: --ostext 'windows 7 sp1 64-bit')", ALERT.BAD)
      exit(1)

  if ARGS.verbose:
    ALERT("name: %s" % name)
    ALERT("release: %s" % release)
    ALERT("service pack: %s" % servicepack)
    ALERT("architecture: %s" % architecture)

  ALERT("querying database file for potential vulnerabilities")

  # potential, all matches within the CSV database for the name,release,sp,arch
  # bulletinds, set of the above with MSIDs (good to keep count)

  # get the potential bulletins
  try:
    for row in csv.reader(StringIO.StringIO(database)):
      bulletinid=row[1]
      affected=row[6]

      if isaffected(name, release, servicepack, architecture, affected):
        
        # only add the bulletin if it's not already in the list
        if bulletinid not in bulletinids:
          potential.append(row)
          bulletinids.add(bulletinid)

          if ARGS.verbose:
            ALERT("%s has been added to potential list '%s'" % (bulletinid, affected))
            
  except csv.Error, e:
    ALERT('could not parse database file, make sure it is in the proper format', ALERT.BAD)
    exit(1)
         
  # there should always be some potential vulns, because of the amount of windows software and false positives  
  if len(bulletinid) == 0:
    ALERT("there are no potential vulnerabilities for, ensure you're searching a valid windows OS", ALERT.BAD)
    exit(1)

  ALERT("comparing the %s hotfix(es) against the %s potential bulletins(s) with a database of %s known exploits" % (len(hotfixes), len(bulletinids), getexploit()))
  
  # start removing the vulns because of hotfixes
  for row in list(potential):

    # ms bulletin
    bulletinid=row[1]
    kb=row[2]
    componentkb=row[7]

    for hotfix in hotfixes:
    
      # if either the hotfixes match the kb or componentkb columns, and the bulletin is in the list
      # of potential bulletins
      if (hotfix == kb or hotfix == componentkb) and bulletinid in bulletinids:

        if ARGS.verbose:
          ALERT("  %s hotfix triggered a removal of %skb and the %s bulletin; componentkb is %s" % (hotfix,kb,bulletinid,componentkb))

        # get the linked ms, this will automatically calculate the superseded by as well
        linkedms = getlinkedms([bulletinid], csv.reader(StringIO.StringIO(database)))
        linkedmsstr = ''
        
        # calculate the pretty string, only care when verbose
        if len(linkedms) > 0:
          for m in linkedms:
            linkedmsstr += ' ' + m

        if ARGS.verbose:
        
          if hotfix == kb:
            ALERT("    due to presence of KB%s (Bulletin KB) removing%s bulletin(s)" % (kb, linkedmsstr))
            
          elif componentkb == kb:
            ALERT("    due to presence of KB%s (Component KB) removing%s bulletin(s)" % (componentkb, linkedmsstr))

        bulletinids = bulletinids.difference(linkedms)
        potential.remove(row)

  ALERT("there are now %s remaining vulns" % len(bulletinids))

  # search local exploits only
  if ARGS.local:
    ALERT("searching for local exploits only")
    for row in list(potential):
      bulletinid = row[1]
      impact = row[4]

      if bulletinid in bulletinids and not "elevation of privilege" in impact.lower():

        remove = getlinkedms([bulletinid], csv.reader(StringIO.StringIO(database)))
        
        if ARGS.verbose:
          ALERT("   removing %s (total of %s MS ids), because of its impact %s" % (bulletinid, len(remove), impact))

        bulletinids = bulletinids.difference(remove)
        potential.remove(row)

  # search remote exploits only
  if ARGS.remote:
    ALERT("searching for remote exploits only")
    for row in list(potential):
      bulletinid = row[1]
      impact = row[4]

      if bulletinid in bulletinids and not "remote code execution" in impact.lower():

        remove = getlinkedms([bulletinid], csv.reader(StringIO.StringIO(database)))
        
        if ARGS.verbose:
          ALERT("   removing %s (total of %s MS ids), because of its impact %s" % (bulletinid, len(remove), impact))

        bulletinids = bulletinids.difference(remove)
        potential.remove(row)
  
  # print windows version
  version=getversion(name, release, servicepack, architecture)

  ALERT("[E] exploitdb PoC, [M] Metasploit module, [*] missing bulletin", ALERT.GOOD)
  ALERT("windows version identified as '%s'" % version, ALERT.GOOD)

  # spacer
  ALERT("")

  # vulns, the dictionary of the bulletins based off of the potential bulletins
  # also, a good opportunity to remove false-positives due to the
  # differences in the technet post and bulletin
  for row in potential:
    id = row[1]

    # start removing vulns because of false-positives
    # Manual override for MS11-011 to reduce false positives. The article was updated, but the bulletin database wasn't (https://technet.microsoft.com/en-us/library/security/ms11-011.aspx)
    # V1.2 (March 18, 2011): Added Windows 7 for 32-bit Systems Service Pack 1, Windows 7 for x64-based Systems Service Pack 1, Windows Server 2008 R2 for x64-based Systems Service Pack 1, and Windows Server 2008 R2 for Itanium-based Systems Service Pack 1 to Non-Affected Software. This is an informational change only. There were no changes to the security update files or detection logic.
    if id == 'MS11-011':
        ms11_011 = ['Windows 7 for 32-bit Systems Service Pack 1', 'Windows 7 for x64-based Systems Service Pack 1', 'Windows Server 2008 R2 for x64-based Systems Service Pack 1','Windows Server 2008 R2 for Itanium-based Systems Service Pack 1']
        for not_affected in ms11_011:
            compare_version = getversion(getname(not_affected),getrelease(not_affected),getservicepack(not_affected),getarchitecture(not_affected))
            if version == compare_version:
                if ARGS.verbose: ALERT("Ignoring MS11-011 false positive due to it not affecting '%s'" % compare_version)
                id = False
        
    for bulletinid in bulletinids:
      if bulletinid == id:
        title = row[5]
        kb = row[2]
        severity = row[3]
        if id not in ids:
          vulns[id] = [title,kb,severity]
          ids.add(id)

  # alerted, if a bulletin has been alerted to the user so that it doesn't appear twice
  #          this occurs when a bulletin has multiple parents
  # msids, the actual data for all of the relevant msids (the row from the CSV)
  alerted = set()
  msids = sorted(vulns, reverse=True)
  
  # loop through the bulletinids which is the set of the actual bulletins that are to
  # be alerted
  for msid in msids:

    ## don't alert twice, no matter the case
    if msid not in alerted: 

      # get the msid, exploitability alert rating, and resources
      m,exploit,resources = getexploit(msid)
      
      # only display the message, if the exploit flag isn't used
      # or if it is used, and the alert level is MSF or EXP
      if ARGS.audit or (exploit == ALERT.MSF or exploit == ALERT.EXP):

        alert = ALERT.NORMAL
        if exploit: alert = exploit
      
        ALERT("%s: %s (%s) - %s" % (msid, vulns[msid][0], vulns[msid][1], vulns[msid][2]), alert)
        if resources and not ARGS.quiet:
            for resource in resources:
                ALERT("  %s" % resource)
            ALERT("")
                
        alerted.add(msid)

        # only attempt to display linked/sub msids based on cli arguments
        if ARGS.sub:

          # linked ms, the children of this msid
          linked = set(getlinkedms([msid], csv.reader(StringIO.StringIO(database))))
          linked = linked.intersection(msids)
          
	      # loop through the linked msids, and only display those that qualify and
          # those that have not been alerted yet
          for lmsid in sorted(linked, reverse=True):
            if lmsid in msids and lmsid not in alerted:
              lexploit = getexploit(lmsid)
              lalert = ALERT.NORMAL
              if ARGS.audit or (lexploit == ALERT.MSF or lexploit == ALERT.EXP):
                if lexploit: lalert = lexploit
                ALERT("|_%s: %s (%s) - %s" % (lmsid, vulns[lmsid][0], vulns[lmsid][1], vulns[lmsid][2]), lalert)
                
		# only allow duplicate events to be displayed when command-line args passed
		if not ARGS.duplicates: alerted.add(lmsid)

  # end run()


# attempt to detect character encoding of a file
# otherwise return None
# https://stackoverflow.com/questions/3323770/character-detection-in-a-text-file-in-python-using-the-universal-encoding-detect
def detect_encoding(filename):
  try:
    import chardet
    data = open(filename, "r").read()
    result = chardet.detect(data)
    encoding = result['encoding']
    return encoding
  except:
    return None

# the trace command is used to determine linked MS bulletins
# TODO much of this is duplicated from run(). should be merged
def trace(database):

  # convert to upper
  bulletinid = ARGS.trace.upper()
  ALERT("searching for bulletin id %s" % bulletinid)

  # get linked msids
  lmsids =  getlinkedms([bulletinid], csv.reader(StringIO.StringIO(database)))

  msids = []

  if ARGS.ostext: 
    ALERT("getting OS information from command line text")

    name=getname(ARGS.ostext)
    release=getrelease(ARGS.ostext)
    servicepack=getservicepack(ARGS.ostext)
    architecture=getarchitecture(ARGS.ostext)

    if ARGS.verbose:
      ALERT("name: %s" % name)
      ALERT("release: %s" % release)
      ALERT("service pack: %s" % servicepack)
      ALERT("architecture: %s" % architecture)

    # the os name at least has to be identified
    if not name:
      ALERT("unable to determine the windows version command line text from '%s'" % ARGS.ostext, ALERT.BAD)
      exit(1)

    # get linked msids, loop through the row
    for row in csv.reader(StringIO.StringIO(database)):
      msid = row[1]
      affected = row[6]

      if msid in lmsids:  
        # debug
        #print ("%s,%s,%s,%s,%s,%s" % (msid, name, release, servicepack, architecture, affected))

        if isaffected(name, release, servicepack, architecture, affected) and msid not in msids: msids.append(msid)
    
 
  else: msids = lmsids

  ALERT("linked msids %s" % msids, ALERT.GOOD)

  
def patches(database):
  
  kbs = []

  # convert to upper
  bulletinid = ARGS.patches.upper()
  ALERT("searching all kb's for bulletin id %s" % bulletinid)

  # get linked msids, loop through the row
  for row in csv.reader(StringIO.StringIO(database)):
      
    bulletinkb=row[2]
    componentkb=row[7]
    
    # if there's a match
    if bulletinid in row[1]:
      kbs.append(bulletinkb)
      kbs.append(componentkb)

  ALERT("relevant kbs %s" % (sorted(set(kbs), reverse=True)), ALERT.GOOD)

def getversion(name, release, servicepack, architecture):
    
  version = "Windows " + name

  # append release first
  if release: version += " R" + release
      
  # then service pack
  if servicepack: version += " SP" + servicepack
  
  # architecture
  if architecture == "Itanium":  version += " Itanium-based"
  else: version += " %s-bit" % architecture
    
  return version


def getname(ostext):

  if ostext == False:
    return False
      
  osname=False

  osnamearray=[["xp","XP"],
               ["2000","2000"],
               ["2003","2003"],
               ["vista","Vista"],
               ["2008","2008"],
               [" 7","7"],
               [" 8","8"],
               ["2012","2012"],
               ["8.1","8.1"],
               [" 10","10"]]

  for needle in osnamearray:
    ostext = ostext.lower()
    if "windows" + needle[0] in ostext or "windows " + needle[0] in ostext or "server" + needle[0] in ostext or "server " + needle[0] in ostext:
      osname = needle[1]

  # the first loop is a more restrictive detection of the OS name, but it does not detect the following
  # > Microsoft Windows\xFF7 Entreprise 
  # so if there is no detection from the first attempt, then search on a more loosely based string of 
  # needle and space
  if not osname:
    for needle in osnamearray:
      if needle[0] + " " in ostext.lower():
        osname = needle[1]

  return osname


def getrelease(ostext):    
    
  if ostext == False:
    return False
      
  osrelease=False
  
  regex="( r| rc|release|rel)[ ]*(\d)"
  m=re.search(regex, ostext.lower())
  
  if m and m.group(2):    
    osrelease=m.group(2)
      
  return osrelease
  
def getservicepack(ostext):
    
  if ostext == False:
    return False
      
  servicepack=False
  
  regex="(sp|pack|pack:)[ ]*(\d)"
  m=re.search(regex, ostext.lower())
  if m and m.group(2):
    servicepack=m.group(2)

  return servicepack


 # architecture defaults to 32, but can be 64-bit
 # or itanium based
def getarchitecture(ostext):
  
  # default to 32-bit
  architecture="32"

  # haystack
  s = ostext.lower()
  
  # attempt to be as flexible as possible
  # matching '64-based', 'x64', ' 64', 'i64', '64bit', '64 bit', '64-bit'
  if ("64-based" in s) or ("x64" in s) or (" 64" in s) or ("i64" in s) or ("64bit" in s) or ("64 bit" in s) or ("64-bit" in s): architecture="64"

  # target Itanium with a simple search for 'tani'
  if "tani" in s: architecture="Itanium"
        
  if getname(ostext) == "2008" and getrelease(ostext) == "2" and architecture == "32":
    if ARGS.verbose:
      ALERT("forcing unidentified architecture to 64-bit because OS identified as Windows 2008 R2 (although could be Itanium and wasn't detected?)")
    architecture = "64"

  # windows server 2012 is only 64-bit arch
  if getname(ostext) == "2012" and architecture == "32":
    if ARGS.verbose:
      ALERT("forcing unidentified architecture to 64-bit because OS identified as Windows Server 2012 does not support 32-bit")
    architecture = "64"  

  return architecture

# itanium build search string
def getitanium(ostext):
    
  if ostext == False:
    return False

  regex="(tanium)"
  m=re.search(regex, ostext.lower())

  if m:
    return True

  return False

def getpatch(ostext):
    
  patch=False
  
  regex="(\d){5,10}"
  m=re.search(regex, ostext.lower())
  if m and m.group():
    patch=m.group()
  
  return patch

# get the bulletin ids from the haystack
# these are typically in the form of: 
#   MS14-009[2898860]
#   MS13-052[2833940],MS14-009[2898856]
# will return a list if found, otherwise false
def getbulletinids(haystack):
  regex="MS[\d]{2,3}-[\d]{2,3}"
  m = re.findall(regex, haystack)
  if len(m) > 0: return m
  return False

def isaffected(name, release, servicepack, architecture, haystack):

  if name == getname(haystack):

    # ensure None are set to False
    # example, if getservicepack() does not get called in the systeminfo parsing
    # then servicepack will be None. this will then fail when comparing to False. 
    if release == None: release = False
    if servicepack == None: servicepack = False
    if architecture == None: architecture = False

#    print "%s,%s,%s,%s" % (name, release, servicepack, architecture)
#    print "%s,%s,%s,%s" % (getname(haystack),getrelease(haystack),getservicepack(haystack),getarchitecture(haystack))

    n = (name == getname(haystack))
    r = (release == getrelease(haystack))
    s = (servicepack == getservicepack(haystack))
    a = (architecture == getarchitecture(haystack))

    # we ignore the architecture for 2012 servers, as there is only 64-bit
    if name == "2012": return r and s

#    print "%s,%s,%s,%s,%s" % (name, release, servicepack, architecture, (a and r and s))

    return a and r and s
    
# search entire database for linked msids
# this will also search the superseded column (11)
def getlinkedms(msids, database):

  lmsids = []

  # go through each row in the database
  for row in database:
  
    # base MS-XX
    rowid=row[1]
    
    # superseded MS-XX
    
    # first try row 12, and then row 11 for the supercedes column due to
    # differences in csv and xlrd parsing. this was a bug that might be
    # fixed now
    rowidsuper = getbulletinids(row[12])
    if rowidsuper == False: rowidsuper=getbulletinids(row[11])  
    
    rowidsuper = merge_list(rowidsuper)

    # loop through each msid for each row
    for msid in msids:
      
      # debug output, what we're working with
      #print "%s,%s,%s" % (msid, rowid, rowidsuper)
      # MS14-053,MS14-053,['MS13-052', 'MS14-009']
      # MS14-053,MS14-053,['MS13-004']
      # MS14-053,MS14-053,['MS13-004']
      # MS14-053,MS14-053,['MS13-004']
      # MS14-053,MS14-053,['MS13-004']
      # MS14-053,MS14-053,[]

      # if the msid matches the row, get the supercedes column (which is a list)
      if msid == rowid or rowid in lmsids:
        #print "%s,%s,%s" % (msid, rowid, rowidsuper)
        lmsids.append(msid)
        lmsids = lmsids + rowidsuper

  return sorted(set(lmsids), reverse=True)

# determines whether or not an msid is in a list of exploits. if msid = 0
# then it will just return the count
def getexploit(msid = 0):
# search using searchsploit
#MS Windows (ListBox/ComboBox Control) Local Exploit (MS03-045)        /windows/local/122.c
#MS Windows Utility Manager Local SYSTEM Exploit (MS04-011)          /windows/local/271.c
#MS Windows 2000 Utility Manager Privilege Elevation Exploit (MS04-019)    /windows/local/350.c
#MS Windows 2K POSIX Subsystem Privilege Escalation Exploit (MS04-020)     /windows/local/351.c
#MS Windows 2000 Universal Language Utility Manager Exploit (MS04-019)     /windows/local/352.c
#MS Windows 2K/XP Task Scheduler .job Exploit (MS04-022)           /windows/local/353.c
#MS Windows 2k Utility Manager (All-In-One) Exploit (MS04-019)         /windows/local/355.c
#MS Windows XP Task Scheduler (.job) Universal Exploit (MS04-022)      /windows/local/368.c
#MS Windows (HTA) Script Execution Exploit (MS05-016)            /windows/local/938.cpp
#MS Windows COM Structured Storage Local Exploit (MS05-012)          /windows/local/1019.c
#MS Windows CSRSS Local Privilege Escalation Exploit (MS05-018)        /windows/local/1198.c
#MS Windows 2k Kernel APC Data-Free Local Escalation Exploit (MS05-055)    /windows/local/1407.c
#MS Windows Telephony Service Command Execution Exploit (MS05-040)       /windows/local/1584.cpp
#MS Windows (NtClose DeadLock) Vulnerability PoC (MS06-030)          /windows/local/1910.c
#MS Windows XP/2K (Mrxsmb.sys) Privilege Escalation PoC (MS06-030)       /windows/local/1911.c
#Microsoft IIS ASP Stack Overflow Exploit (MS06-034)             /windows/local/2056.c
#MS Windows (Windows Kernel) Privilege Escalation Exploit (MS06-049)     /windows/local/2412.c
#MS Windows GDI Local Privilege Escalation Exploit (MS07-017)        /windows/local/3688.c
#MS Windows GDI Local Privilege Escalation Exploit (MS07-017) 2        /windows/local/3755.c
#Kodak Image Viewer TIF/TIFF Code Execution Exploit PoC (MS07-055)       /windows/local/4584.c
#Microsoft Office .WPS File Stack Overflow Exploit (MS08-011)        /windows/local/5107.c
#Microsoft Office Excel Code Execution Exploit (MS08-014)          /windows/local/5287.txt
#Microsoft Office XP SP3 PPT File Buffer Overflow Exploit (ms08-016)     /windows/local/5320.txt
#MS Windows GDI Image Parsing Stack Overflow Exploit (MS08-021)        /windows/local/5442.cpp

#MS Word Record Parsing Buffer Overflow (MS09-027)               /windows/local/14693.py
#MS Excel Malformed FEATHEADER Record Exploit (MS09-067)           /windows/local/14706.py
#MS Word Record Parsing Buffer Overflow MS09-027 (meta)            /windows/local/17177.rb
#MS Internet Explorer Object Tag Exploit (MS03-020)              /windows/remote/37.pl
#MS Windows Media Services Remote Exploit (MS03-022)             /windows/remote/48.c
#Microsoft WordPerfect Document Converter Exploit (MS03-036)         /windows/remote/92.c
#MS Windows (RPC DCOM) Scanner (MS03-039)                  /windows/remote/97.c
#MS Windows (RPC DCOM) Long Filename Overflow Exploit (MS03-026)       /windows/remote/100.c
#MS Windows (RPC DCOM2) Remote Exploit (MS03-039)              /windows/remote/103.c
#MS Windows (RPC2) Universal Exploit & DoS (RPC3) (MS03-039)         /windows/remote/109.c
#MS Windows 2000/XP Workstation Service Overflow (MS03-049)          /windows/remote/119.c
#MS Frontpage Server Extensions fp30reg.dll Exploit (MS03-051)         /windows/remote/121.c
#MS Windows Workstation Service WKSSVC Remote Exploit (MS03-049)       /windows/remote/123.c
#MS Windows XP Workstation Service Remote Exploit (MS03-049)         /windows/remote/130.c
#MS Windows Messenger Service Remote Exploit FR (MS03-043)           /windows/remote/135.c
#MS Internet Explorer URL Injection in History List (MS04-004)         /windows/remote/151.txt
#MS Windows IIS 5.0 SSL Remote buffer overflow Exploit (MS04-011)      /windows/remote/275.c
#MS Windows Lsasrv.dll RPC Remote Buffer Overflow Exploit (MS04-011)     /windows/remote/293.c
#MS Windows XP/2K Lsasrv.dll Remote Universal Exploit (MS04-011)       /windows/remote/295.c
#MS Windows JPEG GDI+ Overflow Administrator Exploit (MS04-028)        /windows/remote/475.sh
#MS Windows JPEG GDI+ Overflow Download Shellcode Exploit (MS04-028)     /windows/remote/478.c
#MS Windows JPEG GDI+ Remote Heap Overflow Exploit (MS04-028)        /windows/remote/480.c
#MS Windows Metafile (.emf) Heap Overflow Exploit (MS04-032)         /windows/remote/584.c
#MS Windows Compressed Zipped Folders Exploit (MS04-034)           /windows/remote/640.c
#MS Windows NetDDE Remote Buffer Overflow Exploit (MS04-031)         /windows/remote/734.c
#MS Internet Explorer .ANI files handling Universal Exploit (MS05-002)     /windows/remote/765.c
#MS Internet Explorer .ANI files handling Downloader Exploit (MS05-002)    /windows/remote/771.cpp
#MS Exchange Server Remote Code Execution Exploit (MS05-021)         /windows/remote/947.pl
#MS Outlook Express NNTP Buffer Overflow Exploit (MS05-030)          /windows/remote/1066.cpp
#MS Windows Message Queuing BoF Universal Exploit (MS05-017) (v.0.3)     /windows/remote/1075.c
#MS Internet Explorer (blnmgr.dll) COM Object Remote Exploit (MS05-038)    /windows/remote/1144.html
#MS Windows Plug-and-Play Service Remote Overflow (MS05-039)         /windows/remote/1146.c
#MS Windows Plug-and-Play Service Remote  Universal Exploit (MS05-039)     /windows/remote/1149.c
#Microsoft Windows DTC Remote Exploit (PoC) (MS05-051) (updated)       /windows/remote/1352.cpp
#Windows Media Player 7.1 <= 10 BMP Heap Overflow PoC (MS06-005) (2)     /windows/remote/1502.py
#MS Windows Media Player 9 Plugin Overflow Exploit (MS06-006) (meta)     /windows/remote/1504.pm
#MS Windows Media Player 10 Plugin Overflow Exploit (MS06-006)         /windows/remote/1505.html
#MS Windows Color Management Module Overflow Exploit (MS05-036) (2)      /windows/remote/1506.c
#MS Windows Media Player Plugin Overflow Exploit (MS06-006)(3)         /windows/remote/1520.pl
#MS Windows RRAS Remote Stack Overflow Exploit (MS06-025)          /windows/remote/1940.pm
#MS Windows RRAS RASMAN Registry Stack Overflow Exploit (MS06-025)       /windows/remote/1965.pm
#MS Internet Explorer (MDAC) Remote Code Execution Exploit (MS06-014)    /windows/remote/2052.sh
#MS Windows DHCP Client Broadcast Attack Exploit (MS06-036)          /windows/remote/2054.txt
#MS Windows NetpIsRemote() Remote Overflow Exploit (MS06-040)        /windows/remote/2162.pm
#Internet Explorer (MDAC) Remote Code Execution Exploit (MS06-014) (2)     /windows/remote/2164.pm
#MS Windows CanonicalizePathName() Remote Exploit (MS06-040)         /windows/remote/2223.c
#MS Windows NetpIsRemote() Remote Overflow Exploit (MS06-040) (2)      /windows/remote/2265.c
#MS Windows NetpIsRemote() Remote Overflow Exploit (MS06-040) (2k3)      /windows/remote/2355.pm
#MS Windows NetpManageIPCConnect Stack Overflow Exploit (MS06-070)       /windows/remote/2789.cpp
#MS Windows Wkssvc NetrJoinDomain2 Stack Overflow Exploit (MS06-070)     /windows/remote/2800.cpp
#MS Windows ASN.1 Remote Exploit (MS04-007)                  /windows/remote/3022.txt
#MS Internet Explorer VML Remote Buffer Overflow Exploit (MS07-004)      /windows/remote/3137.html
#MS Internet Explorer VML Download and Execute Exploit (MS07-004)      /windows/remote/3148.pl
#MS Internet Explorer Recordset Double Free Memory Exploit (MS07-009)    /windows/remote/3577.html
#MS Windows (.ANI) GDI Remote Elevation of Privilege Exploit (MS07-017)    /windows/remote/3804.txt
#MS Internet Explorer <= 7 Remote Arbitrary File Rewrite PoC (MS07-027)    /windows/remote/3892.html
#Microsoft Internet Explorer TIF/TIFF Code Execution (MS07-055)        /windows/remote/4616.pl
#MS Windows Message Queuing Service RPC BOF Exploit (MS07-065)         /windows/remote/4745.cpp
#MS Windows 2000 AS SP4 Message Queue Exploit (MS07-065)           /windows/remote/4760.txt
#Windows Media Encoder wmex.dll ActiveX BOF Exploit (MS08-053)         /windows/remote/6454.html
#MS Windows GDI (EMR_COLORMATCHTOTARGETW) Exploit MS08-021           /windows/remote/6656.txt
#MS Windows Server Service Code Execution Exploit (MS08-067) (Univ)      /windows/remote/6841.txt
#MS Windows Server Service Code Execution Exploit (MS08-067)         /windows/remote/7104.c
#SmbRelay3 NTLM Replay Attack Tool/Exploit (MS08-068)            /windows/remote/7125.txt
#MS Windows Server Service Code Execution Exploit (MS08-067) (2k/2k3)    /windows/remote/7132.py
#Microsoft XML Core Services DTD Cross-Domain Scripting PoC MS08-069     /windows/remote/7196.html
#MS Internet Explorer 7 Memory Corruption Exploit (MS09-002) (xp sp2)    /windows/remote/8079.html
#MS Internet Explorer 7 Memory Corruption Exploit (MS09-002) (py)      /windows/remote/8080.py
#MS Internet Explorer 7 Memory Corruption PoC (MS09-002) (win2k3sp2)     /windows/remote/8082.html
#MS Internet Explorer 7 Memory Corruption Exploit (MS09-002) (fast)      /windows/remote/8152.py
#Microsoft SRV2.SYS SMB Negotiate ProcessID Function Table Dereference (MS09-050) /windows/remote/14674.txt
#Microsoft Services MS06-066 nwwks.dll                     /windows/remote/16369.rb
#Microsoft Services MS06-066 nwapi32.dll                   /windows/remote/16373.rb
#MS03-020 Internet Explorer Object Type                    /windows/remote/16581.rb
#MS03-046 Exchange 2000 XEXCH50 Heap Overflow                /windows/remote/16820.rb

# no ms number yet?
#MS??-???,http://www.exploit-db.com/exploits/30014/,P,??2914486
  # bulletin, type, details
  exploits = [

    ['MS16-135', ALERT.EXP, [ # CVE-2016-7255
    "https://www.exploit-db.com/exploits/40745/ -- Microsoft Windows Kernel - win32k Denial of Service (MS16-135)",
    "https://www.exploit-db.com/exploits/41015/ -- Microsoft Windows Kernel - 'win32k.sys' 'NtSetWindowLongPtr' Privilege Escalation (MS16-135) (2)",
    "https://github.com/tinysec/public/tree/master/CVE-2016-7255"]],

    ['MS16-129', ALERT.EXP, [ # CVE 2016-7200, CVE-2016-7201
    "https://www.exploit-db.com/exploits/40990/ -- Microsoft Edge (Windows 10) - 'chakra.dll' Info Leak / Type Confusion Remote Code Execution",
    "https://github.com/theori-io/chakra-2016-11"]],

    ['MS16-098', ALERT.EXP, [
    "https://www.exploit-db.com/exploits/41020/ -- Microsoft Windows 8.1 (x64) - RGNOBJ Integer Overflow (MS16-098)"]],

    ['MS16-075', ALERT.MSF, [
    "https://github.com/foxglovesec/RottenPotato",
	"https://github.com/Kevin-Robertson/Tater",
	"https://bugs.chromium.org/p/project-zero/issues/detail?id=222 -- Windows: Local WebDAV NTLM Reflection Elevation of Privilege",
    "https://foxglovesecurity.com/2016/01/16/hot-potato/ -- Hot Potato - Windows Privilege Escalation"]],

    ['MS16-074', ALERT.EXP, [ # CVE 2016-3216
     "https://www.exploit-db.com/exploits/39990/ -- Windows - gdi32.dll Multiple DIB-Related EMF Record Handlers Heap-Based Out-of-Bounds Reads/Memory Disclosure (MS16-074), PoC",
     "https://www.exploit-db.com/exploits/39991/ -- Windows Kernel - ATMFD.DLL NamedEscape 0x250C Pool Corruption (MS16-074), PoC"]], # CVE 2016-3220

    ['MS16-063', ALERT.EXP, [ # CVE 2016-0199
     "https://www.exploit-db.com/exploits/39994/ -- Internet Explorer 11 - Garbage Collector Attribute Type Confusion (MS16-063), PoC"]],

    ['MS16-042', ALERT.EXP, [ # CVE 2016-0122
     "https://www.exploit-db.com/exploits/39694/ -- Microsoft Office Excel Out-of-Bounds Read Remote Code Execution (MS16-042), PoC"]],

    ['MS16-059', ALERT.EXP, [ # CVE 2016-0185
     "https://www.exploit-db.com/exploits/39805/ -- Microsoft Windows Media Center - .MCL File Processing Remote Code Execution (MS16-059), PoC"]],

    ['MS16-056', ALERT.EXP, [ # CVE-2015-1730
     "https://www.exploit-db.com/exploits/40881/ -- Microsoft Internet Explorer - jscript9 Java­Script­Stack­Walker Memory Corruption (MS15-056)",
     "http://blog.skylined.nl/20161206001.html -- MSIE jscript9 Java­Script­Stack­Walker memory corruption"]],

    ['MS16-032', ALERT.EXP, [ # CVE 2016-0099
     "https://www.exploit-db.com/exploits/40107/ -- MS16-032 Secondary Logon Handle Privilege Escalation, MSF",
     "https://www.exploit-db.com/exploits/39574/ -- Microsoft Windows 8.1/10 - Secondary Logon Standard Handles Missing Sanitization Privilege Escalation (MS16-032), PoC",
     "https://www.exploit-db.com/exploits/39719/ -- Microsoft Windows 7-10 & Server 2008-2012 (x32/x64) - Local Privilege Escalation (MS16-032) (PowerShell), PoC",
     "https://www.exploit-db.com/exploits/39809/ -- Microsoft Windows 7-10 & Server 2008-2012 (x32/x64) - Local Privilege Escalation (MS16-032) (C#)"]],

    ['MS16-016', ALERT.MSF, [ # CVE 2016-0051
     "https://www.exploit-db.com/exploits/40085/ -- MS16-016 mrxdav.sys WebDav Local Privilege Escalation, MSF",
     "https://www.exploit-db.com/exploits/39788/ -- Microsoft Windows 7 - WebDAV Privilege Escalation Exploit (MS16-016) (2), PoC",
     "https://www.exploit-db.com/exploits/39432/ -- Microsoft Windows 7 SP1 x86 - WebDAV Privilege Escalation (MS16-016) (1), PoC"]],

    ['MS16-014', ALERT.EXP, [ # CVE 2016-0400
     "Windows 7 SP1 x86 - Privilege Escalation (MS16-014), https://www.exploit-db.com/exploits/40039/, PoC"]],

    ['MS16-007', ALERT.EXP, [ # CVE 2016-0015, CVE 2016-0016
     "https://www.exploit-db.com/exploits/39232/ -- Microsoft Windows devenum.dll!DeviceMoniker::Load() - Heap Corruption Buffer Underflow (MS16-007), PoC",
     "https://www.exploit-db.com/exploits/39233/ -- Microsoft Office / COM Object DLL Planting with WMALFXGFXDSP.dll (MS-16-007), PoC"]],

    ['MS15-134', ALERT.EXP, [ # CVE 2015-6131
     "https://www.exploit-db.com/exploits/38911/ -- Microsoft Windows Media Center Library Parsing RCE Vulnerability aka self-executing' MCL File, PoC",
     "https://www.exploit-db.com/exploits/38912/ -- Microsoft Windows Media Center Link File Incorrectly Resolved Reference, PoC",
     "https://www.exploit-db.com/exploits/38918/ -- Microsoft Office / COM Object - 'els.dll' DLL Planting (MS15-134)",
     "https://code.google.com/p/google-security-research/issues/detail?id=514 -- Microsoft Office / COM Object DLL Planting with els.dll"]],

    ['MS15-132', ALERT.EXP, [ # CVE 2015-6132, CVE 2015-6128
     "https://www.exploit-db.com/exploits/38968/ -- Microsoft Office / COM Object DLL Planting with comsvcs.dll Delay Load of mqrt.dll (MS15-132), PoC",
     "https://www.exploit-db.com/exploits/38918/ -- Microsoft Office / COM Object els.dll DLL Planting (MS15-134), PoC"]],

    ['MS15-112', ALERT.EXP, [ # CVE 2015-6086
     "https://www.exploit-db.com/exploits/39698/ -- Internet Explorer 9/10/11 - CDOMStringDataList::InitFromString Out-of-Bounds Read (MS15-112)"]],

    ['MS15-111', ALERT.EXP, [ # CVE 2015-2553
     "https://www.exploit-db.com/exploits/38474/ -- Windows 10 Sandboxed Mount Reparse Point Creation Mitigation Bypass (MS15-111), PoC"]],

    ['MS15-102', ALERT.EXP, [ # CVE 2015-2524, CVE 2015-2525, CVE 2015-2528
     "https://www.exploit-db.com/exploits/38202/ -- Windows CreateObjectTask SettingsSyncDiagnostics Privilege Escalation, PoC",
     "https://www.exploit-db.com/exploits/38200/ -- Windows Task Scheduler DeleteExpiredTaskAfter File Deletion Privilege Escalation, PoC",
     "https://www.exploit-db.com/exploits/38201/ -- Windows CreateObjectTask TileUserBroker Privilege Escalation, PoC"]],

    ['MS15-100', ALERT.MSF, [ # CVE 2015-2509
     "https://www.exploit-db.com/exploits/38195/ -- MS15-100 Microsoft Windows Media Center MCL Vulnerability, MSF",
     "https://www.exploit-db.com/exploits/38151/ -- Windows Media Center - Command Execution (MS15-100), PoC"]],

    ['MS15-097', ALERT.EXP, [ # CVE 2015-2508, CVE 2015-2527
     "https://www.exploit-db.com/exploits/38198/ -- Windows 10 Build 10130 - User Mode Font Driver Thread Permissions Privilege Escalation, PoC",
     "https://www.exploit-db.com/exploits/38199/ -- Windows NtUserGetClipboardAccessToken Token Leak, PoC"]],

    ['MS15-078', ALERT.MSF, [ # CVE 2015-2426, CVE 2015-2433
     "https://www.exploit-db.com/exploits/38222/ -- MS15-078 Microsoft Windows Font Driver Buffer Overflow"]],

    ['MS15-052', ALERT.EXP, [ # CVE 2015-1674
     "https://www.exploit-db.com/exploits/37052/ -- Windows - CNG.SYS Kernel Security Feature Bypass PoC (MS15-052), PoC"]],

    ['MS15-051', ALERT.MSF, [ # CVE 2015-1701
     "https://github.com/hfiref0x/CVE-2015-1701, Win32k Elevation of Privilege Vulnerability, PoC",
     "https://www.exploit-db.com/exploits/37367/ -- Windows ClientCopyImage Win32k Exploit, MSF"]],

    ['MS15-022', ALERT.EXP, [ # CVE 2015-0097
     "https://www.exploit-db.com/exploits/37657/ -- Microsoft Word Local Machine Zone Remote Code Execution Vulnerability, PoC",
     "https://github.com/offensive-security/exploit-database-bin-sploits/raw/master/sploits/37657.zip"]],

    ['MS15-010', ALERT.EXP, [ # CVE 2015-0057
     "https://www.exploit-db.com/exploits/39035/ -- Microsoft Windows 8.1 - win32k Local Privilege Escalation (MS15-010), PoC",
     "https://www.exploit-db.com/exploits/37098/ -- Microsoft Windows - Local Privilege Escalation (MS15-010), PoC",
     "https://www.exploit-db.com/exploits/39035/ -- Microsoft Windows win32k Local Privilege Escalation (MS15-010), PoC"]],

    ['MS15-001', ALERT.EXP, [ # CVE 2015-0002
     "http://www.exploit-db.com/exploits/35661/ -- Windows 8.1 (32/64 bit) - Privilege Escalation (ahcache.sys/NtApphelpCacheControl), PoC"]],

    ['MS14-070', ALERT.EXP, [ # CVE 2014 4076
     "http://www.exploit-db.com/exploits/35936/ -- Microsoft Windows Server 2003 SP2 - Privilege Escalation, PoC"]],

    ['MS14-068', ALERT.EXP, [ # CVE 2014-6324
     "http://www.exploit-db.com/exploits/35474/ -- Windows Kerberos - Elevation of Privilege (MS14-068), PoC"]],

    ['MS14-064', ALERT.MSF, [ # CVE 2014-6332
     "https://www.exploit-db.com/exploits/37800// -- Microsoft Windows HTA (HTML Application) - Remote Code Execution (MS14-064), PoC",
     "http://www.exploit-db.com/exploits/35308/ -- Internet Explorer OLE Pre-IE11 - Automation Array Remote Code Execution / Powershell VirtualAlloc (MS14-064), PoC",
     "http://www.exploit-db.com/exploits/35229/ -- Internet Explorer <= 11 - OLE Automation Array Remote Code Execution (#1), PoC",
     "http://www.exploit-db.com/exploits/35230/ -- Internet Explorer < 11 - OLE Automation Array Remote Code Execution (MSF), MSF",
     "http://www.exploit-db.com/exploits/35235/ -- MS14-064 Microsoft Windows OLE Package Manager Code Execution Through Python, MSF",
     "http://www.exploit-db.com/exploits/35236/ -- MS14-064 Microsoft Windows OLE Package Manager Code Execution, MSF"]],

    ['MS14-062', ALERT.MSF, [ # CVE 2014-4971
     "http://www.exploit-db.com/exploits/34112/ -- Microsoft Windows XP SP3 MQAC.sys - Arbitrary Write Privilege Escalation, PoC",
     "http://www.exploit-db.com/exploits/34982/ -- Microsoft Bluetooth Personal Area Networking (BthPan.sys) Privilege Escalation"]],

    ['MS14-060', ALERT.MSF, [ # CVE 2014-4114
     "http://www.exploit-db.com/exploits/35055/ -- Windows OLE - Remote Code Execution 'Sandworm' Exploit (MS14-060), PoC",
     "http://www.exploit-db.com/exploits/35020/ -- MS14-060 Microsoft Windows OLE Package Manager Code Execution, MSF"]],

    ['MS14-058', ALERT.MSF, [ # CVE 2014-4113
     "http://www.exploit-db.com/exploits/35101/ -- Windows TrackPopupMenu Win32k NULL Pointer Dereference, MSF"]],

    ['MS14-040', ALERT.EXP, [ # CVE 2014-1767
     "https://www.exploit-db.com/exploits/39525/ -- Microsoft Windows 7 x64 - afd.sys Privilege Escalation (MS14-040), PoC",
     "https://www.exploit-db.com/exploits/39446/ -- Microsoft Windows - afd.sys Dangling Pointer Privilege Escalation (MS14-040), PoC"]],

    ['MS14-035', ALERT.EXP],
    ['MS14-029', ALERT.EXP, [
     "http://www.exploit-db.com/exploits/34458/"]],

    ['MS14-026', ALERT.EXP, [ # CVE 2014-1806
     "http://www.exploit-db.com/exploits/35280/, -- .NET Remoting Services Remote Command Execution, PoC"]],

    ['MS14-017', ALERT.MSF],
    ['MS14-012', ALERT.MSF],
    ['MS14-009', ALERT.MSF],
    ['MS14-002', ALERT.EXP],
    ['MS13-101', ALERT.EXP],
    ['MS13-097', ALERT.MSF],
    ['MS13-096', ALERT.MSF],
    ['MS13-090', ALERT.MSF],
    ['MS13-080', ALERT.MSF],
    ['MS13-071', ALERT.MSF],
    ['MS13-069', ALERT.MSF],
    ['MS13-067', ALERT.EXP],
    ['MS13-059', ALERT.MSF],
    ['MS13-055', ALERT.MSF],
    ['MS13-053', ALERT.MSF],
    ['MS13-009', ALERT.MSF],
    ['MS13-005', ALERT.MSF],
    ['MS12-037', ALERT.EXP, [ # CVE 2012-1876
     "http://www.exploit-db.com/exploits/35273/ -- Internet Explorer 8 - Fixed Col Span ID Full ASLR, DEP & EMET 5., PoC",
     "http://www.exploit-db.com/exploits/34815/ -- Internet Explorer 8 - Fixed Col Span ID Full ASLR, DEP & EMET 5.0 Bypass (MS12-037), PoC"]],

    ['MS12-022', ALERT.MSF],
    ['MS11-080', ALERT.MSF],
    ['MS11-011', ALERT.EXP],
    ['MS10-073', ALERT.MSF],
    ['MS10-061', ALERT.MSF],
    ['MS10-059', ALERT.EXP],
    ['MS10-047', ALERT.EXP],
    ['MS10-015', ALERT.MSF],
    ['MS10-002', ALERT.MSF],
    ['MS09-072', ALERT.MSF],
    ['MS09-067', ALERT.MSF],
    ['MS09-065', ALERT.MSF],
    ['MS09-053', ALERT.MSF],
    ['MS09-050', ALERT.MSF, [
    "https://www.rapid7.com/db/modules/exploit/windows/smb/ms09_050_smb2_negotiate_func_index -- MS09-050 Microsoft SRV2.SYS SMB Negotiate ProcessID Function Table Dereference"]],
    
    ['MS09-050', ALERT.MSF],
    ['MS09-043', ALERT.MSF],
    ['MS09-020', ALERT.MSF],
    ['MS09-004', ALERT.MSF],
    ['MS09-002', ALERT.MSF],
    ['MS09-001', ALERT.MSF],
    ['MS08-078', ALERT.MSF],
    ['MS08-070', ALERT.MSF],
    ['MS08-067', ALERT.MSF],
    ['MS08-067', ALERT.MSF],
    ['MS08-053', ALERT.MSF],
    ['MS08-041', ALERT.MSF],
    ['MS08-025', ALERT.EXP],
    ['MS07-065', ALERT.MSF],
    ['MS07-065', ALERT.MSF],
    ['MS07-064', ALERT.MSF],
    ['MS07-029', ALERT.MSF],
    ['MS07-029', ALERT.MSF],
    ['MS07-017', ALERT.MSF],
    ['MS06-071', ALERT.MSF],
    ['MS06-070', ALERT.MSF],
    ['MS06-070', ALERT.MSF],
    ['MS06-067', ALERT.MSF],
    ['MS06-066', ALERT.MSF],
    ['MS06-066', ALERT.MSF],
    ['MS06-063', ALERT.MSF],
    ['MS06-057', ALERT.MSF],
    ['MS06-055', ALERT.MSF],
    ['MS06-049', ALERT.EXP],
    ['MS06-040', ALERT.MSF],
    ['MS06-040', ALERT.MSF],
    ['MS06-035', ALERT.MSF],
    ['MS06-025', ALERT.MSF],
    ['MS06-025', ALERT.MSF],
    ['MS06-019', ALERT.MSF],
    ['MS06-013', ALERT.MSF],
    ['MS06-001', ALERT.MSF],
    ['MS05-054', ALERT.MSF],
    ['MS05-047', ALERT.MSF],
    ['MS05-039', ALERT.MSF],
    ['MS05-039', ALERT.MSF],
    ['MS05-030', ALERT.MSF],
    ['MS05-017', ALERT.MSF],
    ['MS05-017', ALERT.MSF],
    ['MS04-045', ALERT.MSF],
    ['MS04-031', ALERT.MSF],
    ['MS04-031', ALERT.MSF],
    ['MS04-011', ALERT.MSF],
    ['MS04-011', ALERT.MSF],
    ['MS04-007', ALERT.MSF],
    ['MS04-007', ALERT.MSF],
    ['MS03-051', ALERT.MSF],
    ['MS03-049', ALERT.MSF],
    ['MS03-049', ALERT.MSF],
    ['MS03-046', ALERT.MSF],
    ['MS03-026', ALERT.MSF],
    ['MS03-026', ALERT.MSF],
    ['MS03-022', ALERT.MSF],
    ['MS03-020', ALERT.MSF],
    ['MS03-007', ALERT.MSF],
    ['MS02-065', ALERT.MSF],
    ['MS02-063', ALERT.MSF],
    ['MS02-056', ALERT.MSF],
    ['MS02-039', ALERT.MSF],
    ['MS02-018', ALERT.MSF],
    ['MS01-033', ALERT.MSF],
    ['MS01-026', ALERT.MSF],
    ['MS01-023', ALERT.MSF],
    ['MS00-094', ALERT.MSF]
  ]

  # return the count of exploits  
  if msid == 0: return len(exploits)

  for exploit in exploits:
    if msid == exploit[0]:
      # need 3 values to unpack, in case there are resources
      if len(exploit) == 2: 
          exploit.append(None)
          return exploit
      
      # otherwise there are 3 values
      return exploit
  
  return [False,False,False]

# the update function
def update():

  # compute the filenames to be used
  filenames = '%s-mssb' % datetime.datetime.now().strftime('%Y-%m-%d')
  xlsFile = '%s.%s' % (filenames, 'xls')
  csvFile = '%s.%s' % (filenames, 'csv')

  # url request opener with user-agent
  opener = urllib2.build_opener()
  opener.addheaders = [('User-agent', 'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/31.0.1650.57 Safari/537.36')]

  # grab the new data from ms and scrape the site
  #try:
  #  response = opener.open(MSSB_URL)
  #except urllib2.URLError, e:
  #  ALERT("error getting url %s" % MSSB_URL, ALERT.BAD)
  #  exit(1)
  #
  #ALERT("successfully requested base url")
  
  # 2016-02-10, ms changed link to http://download.microsoft.com/download/6/7/3/673E4349-1CA5-40B9-8879-095C72D5B49D/BulletinSearch.xlsx
  #
  # now parse the data, ensure we have an mssb link
  # <td>BulletinSearch_20131111_151603.xlsx <span class="green-sniff-recommend">(recommended)</span></td>
  #html = response.read()
  #m = re.findall('url=(.*BulletinSearch.*.xls[x]*)', html)
  # m = re.findall('href="(.*BulletinSearch.*.xlsx)"', html) # old bulletin request url, 20140502

  # ensure we get the bulletin search
  #if m and m[0]:
  bulletinUrl = BULLETIN_URL
  #  ALERT("scraped ms download url")
    # if the file was xlsx, add an x to the extension
  #  if "xlsx" in bulletinUrl: xlsFile += "x"
  #else:
  #  ALERT("error finding the ms download url from previous response", ALERT.BAD)
  #  exit(1)
    
  # now download the mssb file, with a random sleep
  try:    
    #sleep(randint(1,3))
    response = opener.open(bulletinUrl)
  except urllib2.URLError, e:
    ALERT("error getting ms sb url %s" % bulletinUrl, ALERT.BAD)
    exit(1)
    
  bulletinData = response.read()
  
  ALERT("writing to file %s" % xlsFile, ALERT.GOOD)
  f = open(xlsFile, 'wb')
  f.write(bulletinData)
  f.close

# modified ALERT class for exploit and metasploit level logging
class ALERT(object):
  
  def __init__(self, message, level=0, ansi=True):

    # default to ansi alerting, if it's detected as windows platform then disable
    if platform.system() is "Windows": ansi = False

    good = '[+]'
    bad = '[-]'
    normal = '[*]'
  
    msf = '[M]'
    exploit = '[E]'
    
    if ansi == True:
      if level == ALERT.GOOD: print("%s%s%s" % ('\033[1;32m',good,"\033[0;0m")),
      elif level == ALERT.BAD: print("%s%s%s" % ('\033[1;31m',bad,"\033[0;0m")),
      elif level == ALERT.MSF: print("%s%s%s" % ('\033[1;32m',msf,"\033[0;0m")),
      elif level == ALERT.EXP: print("%s%s%s" % ('\033[1;32m',exploit,"\033[0;0m")),
      else: print("%s%s%s" % ('\033[1;34m',normal,"\033[0;0m")),
      
    else:
      if level == ALERT.GOOD: print('%s' % good),
      elif level == ALERT.BAD: print('%s' % bad),
      elif level == ALERT.MSF: print('%s' % msf),
      elif level == ALERT.EXP: print('%s' % exploit),
      else: print('%s' % normal),
      
    print message
  
  @staticmethod
  @property
  def BAD(self): return -1
    
  @staticmethod
  @property
  def NORMAL(self): return 0
    
  @staticmethod
  @property
  def GOOD(self): return 1
    
  @staticmethod
  @property
  def MSF(self): return 2
    
  @staticmethod
  @property
  def EXP(self): return 3

# this helper function will merge a list of lists into one sorted set
def merge_list(li):
  s = []
  if li:
    for l in li:
        if isinstance(l, list): s = s + l
        else: s.append(l)
  return s

if __name__ == '__main__':
  main()

