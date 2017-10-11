import hvs_pb2
from socket import *
import time


msgToSend = hvs_pb2.WrapperMessage()
msgToSend.cpl.id = 1
msgToSend.cpl.saperateNumber = 4
msgToSend.cpl.style = 4
msgToSend.cpl.isPolling = False


for i in range(1,17):
	msgToSend.cpl.terminalID.append(str(i))
pollingContent = msgToSend.SerializeToString()
print msgToSend


PORT = 12310
HOST="localhost"
ADDR=(HOST,PORT)
tcpCliSock = socket(AF_INET,SOCK_STREAM)
tcpCliSock.connect(ADDR)
tcpCliSock.send(pollingContent)
tcpCliSock.close()