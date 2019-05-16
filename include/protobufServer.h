#ifndef PROTOBUFSERVER_H
#define PROTOBUFSERVER_H
#include "hvs.pb.h"
#include "params.h"
#include "viewer.h"
#include "fmDecoder.h"
#include "manager.h"
#include <sstream>

using namespace boost::asio;
typedef std::pair<std::queue<AVFrame*>,std::pair<std::string,std::string> > BUFFERPAIR;
typedef std::vector<boost::shared_ptr<fmDecoder> > fmDecoderPtrVec;

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
    void setPtr(boost::shared_ptr<std::vector<BUFFERPAIR> > pFrameQueueVecPtr, viewer* vr,fmDecoderPtrVec fv)
    {
        pFrameQueueVecPtr_ = pFrameQueueVecPtr;
        vr_ = vr;
        fv_ = fv;
    }
protected:

private:
    io_service &ios;
    ip::tcp::acceptor acceptor;
    hvs::WrapperMessage wm;
    viewer* vr_;
    fmDecoderPtrVec fv_;
    boost::shared_ptr<std::vector<BUFFERPAIR> > pFrameQueueVecPtr_;

    manager myBoss;

    void timerCallBack(const boost::system::error_code& e,boost::asio::deadline_timer* t)
    {
        std::cout<<"ddd"<<std::endl;
        t->expires_at(t->expires_at()+ boost::posix_time::seconds(5));
        t->async_wait(boost::bind(&protobufServer::timerCallBack,this,boost::asio::placeholders::error,t));
    }

    void startTimer()
    {
        boost::asio::deadline_timer t(ios, boost::posix_time::seconds(5));
        t.async_wait(boost::bind(&protobufServer::timerCallBack,this,boost::asio::placeholders::error,&t));
        ios.run();
    }

    typedef boost::shared_ptr<ip::tcp::socket> sock_ptr;
    void accept_handler(const boost::system::error_code &ec, sock_ptr sock)
    {
        if(ec)
            return;
        //输出客户端连接信息
        std::cout <<"remote ip:"<<sock->remote_endpoint().address()<<std::endl;
        std::cout <<"remote port:"<<sock->remote_endpoint().port() << std::endl;

        char * buff= new char[5120];
        memset(buff,0x00,5120*sizeof(char));
        sock->async_receive(buffer(buff, 5120), boost::bind(&protobufServer::on_read,this,buff,_1,_2));

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

    template <class T>
    void convertFromString(T &value, const std::string &s)
    {
        std::stringstream ss(s);
        ss >> value;
    }

    void SplitString(const std::string& s, std::vector<std::string>& v, const std::string& c)
    {
        std::string::size_type pos1, pos2;
        pos2 = s.find(c);
        pos1 = 0;
        while(std::string::npos != pos2)
        {
            v.push_back(s.substr(pos1, pos2-pos1));

            pos1 = pos2 + c.size();
            pos2 = s.find(c, pos1);
        }
        if(pos1 != s.length())
            v.push_back(s.substr(pos1));
    }

    void on_read(char * ptr, const boost::system::error_code & err, std::size_t read_bytes)
    {
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
            int windowIndex = wm.cpl().id();
            std::cout<<"has cpl or not: "<<wm.has_cpl()<<std::endl;
            if(!wm.cpl().active() && !wm.cpl().inuse())
            {
                printf("window %d deactive!\n",windowIndex);
                myBoss.shutdownViewer(windowIndex);
                break;
            }
            if(!wm.cpl().inuse())
                break;
            if(wm.cpl().active() && wm.cpl().inuse())
                myBoss.startViewer(windowIndex);

            // is polling message
            if(wm.cpl().ispolling())
            {

                std::string splitChar=" ";
                std::vector<std::vector<std::string> > pollingVec(wm.cpl().terminalid_size());

                for(int i = 0; i<wm.cpl().terminalid_size(); ++i)
                {
                    std::cout<<"terminal id: "<<wm.cpl().terminalid(i)<<std::endl;


                    SplitString(wm.cpl().terminalid(i),pollingVec[i],splitChar);
                    for(int j = 0; j < pollingVec[i].size(); ++j)
                    {
                        int tid = -1;
                        convertFromString(tid,pollingVec[i][j]);
                        myBoss.setViewerPosition(wm.cpl().id(),i,tid);
                        myBoss.setDecoderPara(tid,wm.cpl().saperatenumber());
                    }

                }
                myBoss.startTimer(wm.cpl().id(),wm.cpl().pollingtime(),pollingVec);

            }
            else
            {
                if(wm.cpl().saperatenumber()==1 || wm.cpl().saperatenumber()==4 || wm.cpl().saperatenumber()==9 || wm.cpl().saperatenumber()==16 || wm.cpl().saperatenumber()==20)
                {
                }
                else
                {
                    break;
                }

                myBoss.stopTimer(wm.cpl().id());
                for(int i = 0; i<wm.cpl().terminalid_size(); ++i)
                {
                    std::cout<<"terminal id: "<<wm.cpl().terminalid(i)<<std::endl;
                    int tid = -1;
                    convertFromString(tid,wm.cpl().terminalid(i));
                    myBoss.setViewerPosition(wm.cpl().id(),i,tid);
                    myBoss.setDecoderPara(tid,wm.cpl().saperatenumber());
                }
            }


            myBoss.setViewerPara(wm.cpl().id(),wm.cpl().saperatenumber());
//                for(std::vector<boost::shared_ptr<fmDecoder> >::iterator pos =fv_.begin(); pos!=fv_.end()-4; ++pos)
//                {
//                    (*pos)->SetScreanNum(wm.cpl().saperatenumber());
//
//                }
            break;
        }
        case 4:
        {
            std::cout<<"has ta or not: "<<wm.has_ta()<<" size = "<< wm.ta().terminal_size()<<std::endl;
            for(int i = 0; i<wm.ta().terminal_size(); ++i)
            {
                std::cout<<"terminal ip: "<<wm.ta().terminal(i).ip()<<std::endl;
                std::cout<<"terminal name: "<<wm.ta().terminal(i).name()<<std::endl;
                //if(pFrameQueueVecPtr_!=nullptr)
                {
//                        (*pFrameQueueVecPtr_)[i].second = wm.ta().terminal(i).name();
                    myBoss.setChannelBuffer(wm.ta().terminal(i).id(),wm.ta().terminal(i).ip());
                    myBoss.setFrameQueue(wm.ta().terminal(i).id(),wm.ta().terminal(i).name());
                }
            }

            break;
        }
        case 8:
        {
            printf("recv text setting message!\n");
            std::cout<<wm.textstyle().size()<<" "<<wm.textstyle().location()<<std::endl;
            myBoss.setTextStyle(wm.textstyle().size(),wm.textstyle().location(),wm.textstyle().color(),wm.textstyle().showfps());
        }
        default:
            break;
        }
        delete[] ptr;

    }
};

#endif // PROTOBUFSERVER_H
