#include "protobufServer.h"

protobufServer::protobufServer(io_service &io,ip::tcp::endpoint &ep):ios(io),acceptor(io,ep)
{
    //ctor

    start();
}

protobufServer::~protobufServer()
{
    //dtor

}
