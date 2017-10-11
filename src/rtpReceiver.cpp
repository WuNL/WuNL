#include "rtpReceiver.h"

rtpReceiver::rtpReceiver():
    ip_("127.0.0.1"),
    port_(8080),
    threadSeq_(0),
    recvSock_(io_service_,ip::udp::endpoint(ip::udp::v4(),port_))
{
    //ctor
    boost::asio::socket_base::receive_buffer_size recv_option(200000);
    recvSock_.set_option(recv_option);
}

rtpReceiver::~rtpReceiver()
{
    //dtor
}

rtpReceiver::rtpReceiver(const char* ip,int port,int threadSeq):
    ip_(strdup(ip)),
    port_(port),
    threadSeq_(threadSeq),
    recvSock_(io_service_,ip::udp::endpoint(ip::udp::v4(),port_))
{
    boost::asio::socket_base::receive_buffer_size recv_option(200000);
    recvSock_.set_option(recv_option);
    std::cout<<threadSeq_<<"rtpReceiver has been constructed\n";
}

void rtpReceiver::setChannelVecPtr(boost::shared_ptr<std::vector<channel> > cvPtr)
{
    cvPtr_ = cvPtr;

}

void rtpReceiver::start()
{
    std::cout<<threadSeq_<<"rtpReceiver has been started\n";
    m_Thread = boost::thread(&rtpReceiver::run, this);
}

void rtpReceiver::run()
{
    //Set pthread_getaffinity_np
    int rc, i;
    static int cnt =0;
    cpu_set_t cpuset;
    pthread_t thread;

    thread = pthread_self();

    //Check no. of cores on the machine

    /* Set affinity mask */
    CPU_ZERO(&cpuset);
    int mask = 20;
    //for (i = 0; i < 8; i++) //I have 4 cores with 2 threads per core so running it for 8 times, modify it according to your lscpu o/p
    CPU_SET(mask, &cpuset);
    cnt++;

    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    /* Assign affinity mask to the thread */
    rc = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (rc != 0)
        std::cout << "Error calling pthread_getaffinity_np !!!";
    for (i = 0; i < CPU_SETSIZE; i++)
    {
        if (CPU_ISSET(i, &cpuset))
        {
            std::cout << "rtpRecver is on CPU " << sched_getcpu() << std::endl;
        }
    }

    while(true)
    {
        std::array<char, 2000> recv_buf;
        ip::udp::endpoint remote_endpoint;
        boost::system::error_code error;

        // 接收一个字符，这样就得到了远程端点(remote_endpoint)
        int bytes = recvSock_.receive_from(boost::asio::buffer(recv_buf),remote_endpoint, 0, error);
        if (error && error != boost::asio::error::message_size)
        {
            std::cout<<error.message()<<std::endl;
            throw boost::system::system_error(error);
        }

        unsigned char seq[2];
        seq[0] = recv_buf[45 - 42];
        seq[1] = recv_buf[44 - 42];
        unsigned short *seqNum = (unsigned short *)&seq;
        char naluHeader[4] = {0, 0, 0, 1};
        unsigned char head1 = recv_buf[54 - 42];
        unsigned char head2 = recv_buf[55 - 42];
        unsigned char nal = head1 & 0x1f;
        unsigned char flag = head2 & 0xe0;
        unsigned char nal_fu = (head1 & 0xe0) | (head2 & 0x1f);
        int len = 0;
        (*cvPtr_)[threadSeq_].packetNumber=*seqNum;
        //printf("nal:%X\n",nal);
        //说明被分包
        if(nal == 0x1c)
        {
            //开始
            if (flag == 0x80)
            {
                memcpy((*cvPtr_)[threadSeq_].showBuffer[((*cvPtr_)[threadSeq_].writeIndex)] + len, &naluHeader, 4);
                len += 4;
                memcpy((*cvPtr_)[threadSeq_].showBuffer[((*cvPtr_)[threadSeq_].writeIndex)] + len, &nal_fu, 1);
                len += 1;
            }

            memcpy((*cvPtr_)[threadSeq_].showBuffer[((*cvPtr_)[threadSeq_].writeIndex)] + len, &recv_buf[56 - 42], bytes - 56 + 42);
            (*cvPtr_)[threadSeq_].showBufferLen[((*cvPtr_)[threadSeq_].writeIndex)] = bytes - 56 + 42 + len;
            ++((*cvPtr_)[threadSeq_].writeIndex);
        }
        //单包
        else
        {
            memcpy((*cvPtr_)[threadSeq_].showBuffer[((*cvPtr_)[threadSeq_].writeIndex)] + len, &naluHeader, 4);
            len += 4;
            memcpy((*cvPtr_)[threadSeq_].showBuffer[((*cvPtr_)[threadSeq_].writeIndex)] + len, &recv_buf[54 - 42], bytes - 54 + 42);
            (*cvPtr_)[threadSeq_].showBufferLen[((*cvPtr_)[threadSeq_].writeIndex)] = bytes - 54 + 42 + len;
            ++((*cvPtr_)[threadSeq_].writeIndex);
        }
        //std::cout<<"(*cvPtr_)[threadSeq_].writeIndex="<<(*cvPtr_)[threadSeq_].writeIndex<<std::endl;
        if ((*cvPtr_)[threadSeq_].writeIndex == BUFFER_LEN)
        {
            (*cvPtr_)[threadSeq_].writeIndex = 0;
        }
    }
}
