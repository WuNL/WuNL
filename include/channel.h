#ifndef CHANNEL_H
#define CHANNEL_H

#define BUFFER_LEN  1000
#define BUFFER_SIZE 1600
#include <boost/asio.hpp>
#include "params.h"
using namespace boost::asio;

class channel
{
public:
    channel();
    virtual ~channel();

    unsigned char ip[4];
    unsigned int port;
    unsigned int lostPacketNumber;
    unsigned int recvPacketNumber;
    unsigned short packetNumber;
    unsigned int audio;
    bool isStart;
    int writeIndex;
    int readIndex;
    int showBufferLen[BUFFER_LEN];
    unsigned char showBuffer[BUFFER_LEN][BUFFER_SIZE];
private:
};

#endif // CHANNEL_H
