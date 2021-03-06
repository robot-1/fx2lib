#!/usr/bin/python3
# Copyright (C) 2009 Ubixum, Inc. 
#
# This library is free software; you can redistribute it and/or
#
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
# 
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

from time import sleep
from fx2 import fx2

f=fx2.fx2()

# use this functions 1st

def openfx2(vid=0x04b4,pid=0x0082,idx=0):
    f.open(vid,pid,idx)

def reset_device(reset):
 print (reset and "Put device in reset" or "Set device to run")
 write_ram (0xe600,reset and '\x01' or '\x00', 1)
 
def write_ram(addr,data,length):
 transferred=0
 while(transferred<length):
  this_transfer_size=length-transferred>1024 and 1024 or length-transferred
  buf=data[transferred:]
  ret=f.do_usb_command(buf,
    0x40,
    0xa0,
    addr+transferred, 0,
    this_transfer_size)
  if (ret>0):
   print("wrote {} bytes".format(ret))
   transferred+=ret
  else:
   print("Error: %d".format(ret))
   return

def reset_bix(filename):
 """
  Use this function to reset your firmware.  You'll need to reopen the device afterward.
 """
 reset_device(True)
 bix=open(filename,'rb').read()
 print ("loading bix file of length: {}".format(len(bix))) 
 write_ram( 0, bix,len(bix) );
 reset_device(False)
 f.close()

