# -*- coding: UTF-8 -*- 
import hvs_pb2
from socket import *
import time
import os
import sys

num = int(sys.argv[1])
if num not in (1,4,9,16):
	num = 16
print num
num1 = int(sys.argv[2])
if num1 not in (1,4,9,16):
	num1 = 16
print num1

taMsgToSend = hvs_pb2.WrapperMessage()
for i in range(1,21):
	tmpTerminal = taMsgToSend.ta.terminal.add()
	tmpTerminal.id = i
	tmpTerminal.ip = "192.168.1.%d" % (99+i)
	tmpTerminal.name = ("192.168.1.%d" % (99+i))+(u":缓冲区".encode('utf8'))
	tmpTerminal.tstatus = True
print taMsgToSend
taContent = taMsgToSend.SerializeToString()



msgToSend = hvs_pb2.WrapperMessage()
msgToSend.cpl.id = 1
msgToSend.cpl.active = False
msgToSend.cpl.saperateNumber = num
msgToSend.cpl.style = 4
msgToSend.cpl.isPolling = False


for i in range(2,17):
	msgToSend.cpl.terminalID.append(str(i))
	# if i%2 == 0:
	# 	msgToSend.cpl.terminalID.append(str(i))
	# else:
	# 	msgToSend.cpl.terminalID.append(str(-1))
print msgToSend
pollingContent = msgToSend.SerializeToString()


msgToSend1 = hvs_pb2.WrapperMessage()
msgToSend1.cpl.id = 2
msgToSend1.cpl.active = False
msgToSend1.cpl.saperateNumber = num1
msgToSend1.cpl.style = 4
msgToSend1.cpl.isPolling = False


for i in range(1,2):
	msgToSend1.cpl.terminalID.append(str(i))
print msgToSend1
pollingContent1 = msgToSend1.SerializeToString()



PORT = 12310
HOST="localhost"
ADDR=(HOST,PORT)
tcpCliSock = socket(AF_INET,SOCK_STREAM)
tcpCliSock.connect(ADDR)
print tcpCliSock.send(taContent)
tcpCliSock.close()
# print tcpCliSock.send("pollingContent")
# time.sleep(3)
tcpCliSock = socket(AF_INET,SOCK_STREAM)
tcpCliSock.connect(ADDR)
print tcpCliSock.send(pollingContent)
tcpCliSock.close()

tcpCliSock = socket(AF_INET,SOCK_STREAM)
tcpCliSock.connect(ADDR)
print tcpCliSock.send(pollingContent1)
tcpCliSock.close()