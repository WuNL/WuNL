#ifndef PROTOBUFSERVER_H
#define PROTOBUFSERVER_H
#include "hvs.pb.h"

#include "boost/asio.hpp"
#include "boost/shared_ptr.hpp"
#include "boost/thread.hpp"


using namespace boost::asio;



class protobufServer
{
public:
    protobufServer(io_service &io,ip::tcp::endpoint &ep);
    virtual ~protobufServer();
    //启动异步接受客户端连接
    void start()
    {
        sock_ptr sock(new ip::tcp::socket(ios));
        //当有连接进入时回调accept_handler函数
        acceptor.async_accept(*sock,
                              boost::bind(&protobufServer::accept_handler,this,placeholders::error,sock));
    }
protected:

private:
    io_service &ios;
    ip::tcp::acceptor acceptor;
    hvs::WrapperMessage wm;

    typedef boost::shared_ptr<ip::tcp::socket> sock_ptr;
    void accept_handler(const boost::system::error_code &ec, sock_ptr sock)
    {
        if(ec)
            return;
        //输出客户端连接信息
        std::cout <<"remote ip:"<<sock->remote_endpoint().address()<<std::endl;
        std::cout <<"remote port:"<<sock->remote_endpoint().port() << std::endl;

        char * buff= new char[512];
        memset(buff,0x00,512*sizeof(char));
        sock->async_receive(buffer(buff, 512), boost::bind(&protobufServer::on_read,this,buff,_1,_2));

        //异步向客户端发送数据，发送完成时调用write_handler
        sock->async_write_some(buffer("I heard you!"),
                               bind(&protobufServer::write_handler,this,placeholders::error));
        //再次启动异步接受连接
        start();
    }

    void write_handler(const boost::system::error_code& err)
    {
        std::cout<<"send msg complete!"<<std::endl;
    }

    void on_read(char * ptr, const boost::system::error_code & err, std::size_t read_bytes) {
        printf("recving %d\n",read_bytes);

        wm.ParseFromArray(ptr,read_bytes);

        switch(wm.msg_case())
        {
        case 1:
            break;
        case 2:
            break;
        case 3:
            {
                std::cout<<"has cpl or not: "<<wm.has_cpl()<<std::endl;
                for(int i = 0; i<wm.cpl().terminalid_size();++i)
                {
                    std::cout<<"terminal id: "<<wm.cpl().terminalid(i)<<std::endl;
                }
                break;
            }
        case 4:
            break;
        default:
            break;
        }


    }
};

#endif // PROTOBUFSERVER_H
