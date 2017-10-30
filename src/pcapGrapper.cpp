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
    { fprintf(stderr,"Error calling pcap_compile\n"); exit(1); }

    /* set the compiled program as the filter */
    if(pcap_setfilter(descr,&fp) == -1)
    { fprintf(stderr,"Error setting filter\n"); exit(1); }
    /* allright here we call pcap_loop(..) and pass in our callback function */
    /* int pcap_loop(pcap_t *p, int cnt, pcap_handler callback, u_char *user)*/
    /* If you are wondering what the user argument is all about, so am I!!   */
    pcap_loop(descr,NULL,pcapGrepper::capture_callback,(u_char *)cvPtr_.get());

    fprintf(stdout,"\nDone processing packets... wheew!\n");

}

void pcapGrepper::capture_callback(u_char *useless,const struct pcap_pkthdr* pkthdr,const u_char* packet)
{
    std::vector<channel>* myPtr = (std::vector<channel> *)useless;
//    boost::shared_ptr<std::vector<channel> > myCVPtr = (boost::shared_ptr<std::vector<channel> >)useless;

    //(*cvPtr_)[threadSeq_].showBuffer[0][0] = 1;
}



