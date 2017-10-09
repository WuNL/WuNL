#ifndef PROTOBUFSERVER_H
#define PROTOBUFSERVER_H
#include "hvs.pb.h"
#include <iostream>
#include <stdio.h>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <boost/asio.hpp>
using namespace boost::asio;

class protobufServer
{
    public:
        protobufServer();
        virtual ~protobufServer();

    protected:

    private:
};

#endif // PROTOBUFSERVER_H
