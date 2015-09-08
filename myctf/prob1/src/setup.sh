#!/bin/bash

sudo apt-get install libc6
sudo cp /usr/include/openssl/conf.h /usr/include/openssl/opensslconf.h
sudo ln -s /lib32/libcrypt.so.1 /usr/lib/libcrypto.so
sudo dpkg --add-architecture i386
sudo apt-get -y update
sudo apt-get -y install libssl-dev:i386 libmcrypt-dev:i386


sudo useradd ropmeifyoucan -m
sudo chown -R root:ropmeifyoucan /home/ropmeifyoucan
sudo chmod 755 /home/ropmeifyoucan
make -C /vagrant/problems/rajivk/prob1/src
sudo cp /vagrant/problems/rajivk/prob1/src/register_user /home/ropmeifyoucan/
sudo cp /vagrant/problems/rajivk/prob1/src/flag.txt /home/ropmeifyoucan
sudo chown root:root /home/ropmeifyoucan
sudo chgrp ropmeifyoucan /home/ropmeifyoucan/register_user
sudo chgrp ropmeifyoucan /home/ropmeifyoucan/flag.txt
sudo chmod 440 /home/ropmeifyoucan/flag.txt
sudo chmod 2755 /home/ropmeifyoucan/register_user
