#!/usr/bin/bash
gcc -I. udpclient.c -o client
gcc -I. udpserver.c -o server
g++ -I. camudp.cpp `pkg-config --cflags --libs opencv` -o cam
g++ -I. recvimg.cpp `pkg-config --cflags --libs opencv` -o recv