#include "pcapGrapper.h"



pcapGrepper::pcapGrepper()
{
    //ctor
}

pcapGrepper::~pcapGrepper()
{
    //dtor
}

void pcapGrepper::start()
{
    m_Thread = boost::thread(&pcapGrepper::run, this);
}

void pcapGrepper::run()
{
    printf("running!\n");
    /* grab a device to peak into... */
    dev = pcap_lookupdev(errbuf);
    if(dev)
    {
        printf("success: device: %s\n", dev);
    }
    if(dev == NULL)
    {
        printf("%s\n",errbuf);
        exit(1);
    }
    /* open device for reading */
    descr = pcap_open_live(dev,65536,1,-1,errbuf);
    if(descr == NULL)
    {
        printf("pcap_open_live(): %s\n",errbuf);
        exit(1);
    }

    /* ask pcap for the network address and mask of the device */
    pcap_lookupnet(dev,&netp,&maskp,errbuf);

    /* Lets try and compile the program.. non-optimized */
    if(pcap_compile(descr,&fp,"udp",0,netp) == -1)
    {
        fprintf(stderr,"Error calling pcap_compile\n");
        exit(1);
    }

    /* set the compiled program as the filter */
    if(pcap_setfilter(descr,&fp) == -1)
    {
        fprintf(stderr,"Error setting filter\n");
        exit(1);
    }
    /* allright here we call pcap_loop(..) and pass in our callback function */
    /* int pcap_loop(pcap_t *p, int cnt, pcap_handler callback, u_char *user)*/
    /* If you are wondering what the user argument is all about, so am I!!   */
    pcap_loop(descr,NULL,pcapGrepper::capture_callback,(u_char *)cvPtr_.get());

    fprintf(stdout,"\nDone processing packets... wheew!\n");

}

void pcapGrepper::capture_callback(u_char *useless,const struct pcap_pkthdr* header,const u_char* pkt_data)
{


    if (!((pkt_data[42] == 0x80) && ((pkt_data[43] == 0x6A) || (pkt_data[43] == 0xeA) || (pkt_data[43] == 0x60) || (pkt_data[43] == 0xe0) || (pkt_data[43] == 0x69) || (pkt_data[43] == 0xe9))))
    {
        return;
    }

    std::vector<channel>* myPtr = (std::vector<channel> *)useless;

    int position = 0;
    bool found = false;
    unsigned char *ip = (unsigned char *)(&pkt_data[26]);
    unsigned char port[2];
    port[0] = pkt_data[37];
    port[1] = pkt_data[36];
    unsigned short *portNum = (unsigned short *)&port;

    for (int i = 0; i < CHANNELNUM; ++i)
    {
        if((*myPtr)[i].port == (*portNum))
        {
            position = i;
            found = true;
            break;
        }
    }

//    for (int i = 0; i < CHANNELNUM; ++i)
//    {
//        int count = 0;
//        for (int j = 0; j < 4; ++j)
//        {
//            if ((*myPtr)[i].ip[j] == ip[j])
//            {
//                ++count;
//            }
//        }
//        if (count == 4)
//        {
//            position = i;
//            found = true;
//            break;
//        }
//    }
    if (!found)
    {
        return;
    }

    if(use_webrtc)
    {
        (*myPtr)[position].pb_->insertPacket(pkt_data, header->len);
        return;
    }


}



