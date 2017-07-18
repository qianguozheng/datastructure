#!/usr/bin/env python

import socket

TCP_IP = '127.0.0.1'
TCP_PORT = 8081

BUFFER_SIZE = 1024

s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

conn,addr = s.accept()
print 'Connection address:', addr

while 1:
	
	#try:
	data = conn.recv(BUFFER_SIZE)
	#except:
		#print ""
		#if not data:
		#conn,addr = s.accept()
		#print 'Connection address:', addr
	print "received data: ", data
	if (data):
		conn.send(data) #echo

#conn.close()
