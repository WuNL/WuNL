#ifndef PCAPGRAPPER_H
#define PCAPGRAPPER_H

#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/tss.hpp>
#include <boost/asio.hpp>

#include "channel.h"
#include "params.h"
#include "packetBuffer.h"

class basePcap
{
protected:
    int i;
    char *dev;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t* descr;
    const u_char *packet;
    struct pcap_pkthdr hdr;     /* pcap.h                    */
    struct ether_header *eptr;  /* net/ethernet.h            */
    struct bpf_program fp;      /* hold compiled program     */
    bpf_u_int32 maskp;          /* subnet mask               */
    bpf_u_int32 netp;           /* ip                        */
    boost::thread m_Thread;
};


class pcapGrepper:basePcap
{
public:
    pcapGrepper();
    virtual ~pcapGrepper();

    void start();
    void setChannelVecPtr(boost::shared_ptr<std::vector<channel> > cvPtr)
    {
        cvPtr_ = cvPtr;
    };

protected:

private:
    void run();
    static void capture_callback(u_char *useless,const struct pcap_pkthdr* pkthdr,const u_char* packet);

    boost::shared_ptr<std::vector<channel> > cvPtr_;
};

#endif // PCAPGRAPPER_H
