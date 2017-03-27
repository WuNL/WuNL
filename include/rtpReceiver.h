#ifndef RTPRECEIVER_H
#define RTPRECEIVER_H
#include <iostream>
#include <stdio.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <boost/asio.hpp>
using namespace boost::asio;

#include "channel.h"

class rtpReceiver
{
public:
    rtpReceiver();
    rtpReceiver(const char* ip,int port,int threadSeq);
    virtual ~rtpReceiver();
    void run();
    void start();
    void setChannelVecPtr(boost::shared_ptr<std::vector<channel> > cvPtr);
    void setIP(char* ip)
    {
        ip_=ip;
    }
    void setPort(int port)
    {
        port_ = port;
    }
    void setSeq(int threadSeq)
    {
        threadSeq_ = threadSeq;
    }
    void setEndPoint(boost::asio::ip::udp::endpoint ep)
    {
        //recvSock_.
    }
protected:

private:
    boost::asio::io_service io_service_;
    char * ip_;
    int port_;
    int threadSeq_;
    boost::shared_ptr<std::vector<channel> > cvPtr_;
    boost::asio::ip::udp::socket recvSock_;
    boost::thread m_Thread;
};

#endif // RTPRECEIVER_H
