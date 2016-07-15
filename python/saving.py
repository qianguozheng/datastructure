 #-*- coding: utf-8 -*-
import urllib2
import json
import time
from sgmllib import SGMLParser

class ListName(SGMLParser):
	def __init__(self):
		SGMLParser.__init__(self)
		self.is_h4 = ""
		self.name = []

	def start_h4(self, attrs):
		self.is_h4 = 1
	def end_h4(self):
		self.is_h4 = ""
	def handle_data(self, text):
		if self.is_h4 == 1 :
			self.name.append(text)

shadowsocks = [
{
    "server": "139.162.58.57",
    "server_port": 443,
    "local_port": 1080,
    "password": "helloweeds",
    "timeout": 60,
    "method": "aes-256-cfb"
}
]

while True:

	content = urllib2.urlopen('http://www.ishadowsocks.net/').read()
	#print content
	listname = ListName()
	listname.feed(content)

	A = []
	B = []
	C = []

	a, b, c = '', '', ''
	for item in listname.name:
		#print item.decode('gbk').encode('utf8')
		#print item
		#t = item.find('A服务器地址')
		#print t
		if item.find('A服务器地址') >= 0:
			 a = item.split(':')
			 #print a
			 #print a[1]
			 A.append(a[1])
		if len(a) > 0:
			if item.find('端口') >= 0:
				a2 = item.split(':')
				A.append(a2[1])
			if item.find('A密码') >= 0:
				a3 = item.split(':')
				A.append(a3[1])
			if item.find('加密方式') >= 0:
				a4 = item.split(':')
				A.append(a4[1])
				a = ''

		#----B server
		if item.find('B服务器地址') >= 0:
			 b = item.split(':')
			 #print b
			 #print b[1]
			 B.append(b[1])

		if len(b) > 0:
			if item.find('端口') >= 0:
				b2 = item.split(':')
				B.append(b2[1])
			if item.find('B密码') >= 0:
				b3 = item.split(':')
				B.append(b3[1])
			if item.find('加密方式') >= 0:
				b4 = item.split(':')
				B.append(b4[1])
				b = ''

		#----C server
		if item.find('C服务器地址') >= 0:
			 c = item.split(':')
			 #print c
			 #print c[1]
			 C.append(c[1])

		if len(c) > 0:
			if item.find('端口') >= 0:
				c2 = item.split(':')
				C.append(c2[1])
			if item.find('C密码') >= 0:
				c3 = item.split(':')
				C.append(c3[1])
			if item.find('加密方式') >= 0:
				c4 = item.split(':')
				C.append(c4[1])
				c = ''
		#print item
	'''
	print A
	print B
	print C

	print A[0]
	print A[1]
	print A[2]
	print A[3]
	'''

	data = {}
	data['server'] = A[0]
	data['server_port'] = int(A[1])
	data['local_port'] = 1080
	data['password'] = A[2]
	data['timeout'] = 60
	data['method'] = A[3]


	with open('/var/www/my_blog/shadowsocks.json', 'w') as f:
		f.write(json.dumps(data))

#	print 'sleep start'
	time.sleep(120);
#	print 'sleep over'
