#include "protobufServer.h"

protobufServer::protobufServer(io_service &io,ip::tcp::endpoint &ep):ios(io),acceptor(io,ep),pFrameQueueVecPtr_(nullptr)
{
    //ctor
    myBoss.startViewer();
    start();
}

protobufServer::~protobufServer()
{
    //dtor

}
