#!/bin/bash
sudo stty -F /dev/ttyUSB0 sane 57600 intr undef quit undef erase undef kill undef eof undef eol undef eol2 undef start undef stop undef susp undef rprnt undef werase undef lnext undef flush undef min 1 time 0 ignbrk -brkint -icrnl -imaxbel -opost -onlcr -isig -icanon -iexten -echo -echoe -echok -echoctl -echoke
sudo chown pi /dev/ttyUSB0

