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

taMsgToSend = hvs_pb2.WrapperMessage()
for i in range(1,21):
	tmpTerminal = taMsgToSend.ta.terminal.add()
	tmpTerminal.id = i
	tmpTerminal.ip = "192.168.1.%d" % i
	tmpTerminal.name = ("192.168.1.%d" % i)+(u":缓冲区".encode('utf8'))
	tmpTerminal.tstatus = True

taContent = taMsgToSend.SerializeToString()



msgToSend = hvs_pb2.WrapperMessage()
msgToSend.cpl.id = 1
msgToSend.cpl.saperateNumber = num
msgToSend.cpl.style = 4
msgToSend.cpl.isPolling = False


for i in range(1,17):
	msgToSend.cpl.terminalID.append(str(i))
pollingContent = msgToSend.SerializeToString()



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