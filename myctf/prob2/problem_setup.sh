#!/bin/bash

sudo apt-get install libc6
sudo cp /usr/include/openssl/conf.h /usr/include/openssl/opensslconf.h
sudo ln -s /lib32/libcrypt.so.1 /usr/lib/libcrypto.so
sudo dpkg --add-architecture i386
sudo apt-get -y update
sudo apt-get -y install libssl-dev:i386 libmcrypt-dev:i386


sudo useradd iVault -m
sudo chown -R root:iVault /home/iVault
sudo chmod 755 /home/iVault
make -C /vagrant/problems/rajivk/prob2/src
sudo cp /vagrant/problems/rajivk/prob2/src/iVault.c /home/iVault/
sudo cp /vagrant/problems/rajivk/prob2/src/iVault /home/iVault/
sudo cp /vagrant/problems/rajivk/prob2/src/flag.txt /home/iVault
sudo chown root:root /home/iVault
sudo chgrp iVault /home/iVault/iVault
sudo chgrp iVault /home/iVault/flag.txt
sudo chmod 440 /home/iVault/flag.txt
sudo chmod 2755 /home/iVault/iVault
sudo sh -c "echo 0 > /proc/sys/kernel/randomize_va_space"
