#include "pcapGrapper.h"
#include <math.h>
#include <stdint.h>

#define	SIGN_BIT	(0x80)		/* Sign bit for a A-law byte. */
#define	QUANT_MASK	(0xf)		/* Quantization field mask. */
#define	NSEGS		(8)		/* Number of A-law segments. */
#define	SEG_SHIFT	(4)		/* Left shift for segment number. */
#define	SEG_MASK	(0x70)		/* Segment field mask. */
#define	ULAW_BIAS		(0x84)		/* Bias for linear code. */
#define CLIP        8159

int16_t ulaw_to_linear(uint8_t ulaw) {
  int t;
  /* Complement to obtain normal u-law value. */
  ulaw = ~ulaw;
  /*
   * Extract and bias the quantization bits. Then
   * shift up by the segment number and subtract out the bias.
   */
  t = (((ulaw & 0x0F) << 3) + ULAW_BIAS) << (((int) ulaw & 0x70) >> 4);
  return (int16_t)((ulaw & 0x80) ? (ULAW_BIAS - t) : (t - ULAW_BIAS));
}


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
    std::vector<channel>* myPtr = (std::vector<channel> *)useless;

    if (!((pkt_data[42] == 0x80) && ((pkt_data[43] == 0x6A) || (pkt_data[43] == 0xeA) || (pkt_data[43] == 0x00) || (pkt_data[43] == 0x08) )))
    {
        return;
    }
    int position = 0;
    bool found = false;
    unsigned char *ip = (unsigned char *)(&pkt_data[26]);
    unsigned char port[2];
    port[0] = pkt_data[37];
    port[1] = pkt_data[36];
    unsigned short *portNum = (unsigned short *)&port;

//    for (int i = 0; i < CHANNELNUM; ++i)
//    {
//        if((*myPtr)[i].port == (*portNum))
//        {
//            position = i;
//            found = true;
//            break;
//        }
//    }

    for (int i = 0; i < CHANNELNUM; ++i)
    {
        int count = 0;
        for (int j = 0; j < 4; ++j)
        {
            if ((*myPtr)[i].ip[j] == ip[j])
            {
                ++count;
            }
        }
        if (count == 4)
        {
            position = i;
            found = true;
            break;
        }
    }
    if (!found)
    {
        return;
    }

	if (pkt_data[43] == 0x00 || pkt_data[43] == 0x08)
	{
		std::vector<uint8_t> vc;
		for(int i = 54; i < header->len - 54; ++i)
		{
			{
				vc.push_back((uint8_t)abs(ulaw_to_linear((uint8_t)pkt_data[i])));
			}
		}
//		uint16_t sum = 0;
//		for(int i = 0; i < vc.size(); ++i)
//		{
//			sum += vc[i];
//		}
//		if(vc.size() != 0)
//		{
//            (*myPtr)[position].audio = sum / vc.size();
//		}
//		//isAudio = true;
//		return;



        short int value;

        int i;
        long v = 0;
        for(i=0; i<vc.size(); i+=2)
        {
            memcpy((char*)&value, &vc+i, 1);
            memcpy((char*)&value+1, &vc+i+1, 1);
            v += abs(value);
        }

        v = v/(vc.size()/2);
		(*myPtr)[position].audio = v;
        return;




	}




    //Í³¼Æ¶ª°üÇé¿ö
    unsigned char seq[2];
    seq[0] = pkt_data[45];
    seq[1] = pkt_data[44];
    unsigned short *seqNum = (unsigned short *)&seq;
    if (((*myPtr)[position].packetNumber != *seqNum - 1))
    {
        if (*seqNum == 0)
        {

        }
        else
        {
            if ((*myPtr)[position].packetNumber < *seqNum - 1)
            {
                (*myPtr)[position].lostPacketNumber += *seqNum - 1 - (*myPtr)[position].packetNumber;
                printf("loss packet : %d\n", *seqNum - 1 - (*myPtr)[position].packetNumber);
            }
        }
    }
    (*myPtr)[position].packetNumber = *seqNum;
    (*myPtr)[position].recvPacketNumber++;

    //Êý¾Ý°ü½øÐÐÖØ×é²¢·ÅÈë¶ÔÓ¦µÄ»·ÐÎ»º³åÇø
    char naluHeader[4] = {0, 0, 0, 1};
    unsigned char head1 = pkt_data[54];
    unsigned char head2 = pkt_data[55];
    unsigned char nal = head1 & 0x1f;
    unsigned char flag = head2 & 0xe0;
    unsigned char nal_fu = (head1 & 0xe0) | (head2 & 0x1f);

    int len = 0;
    int flagLen = 54;
//	flagLen += 54;

    //if(nal != 28)
    //printf("%d ", nal&0x1f);

    if(nal == 0x1c)
    {
        if (flag == 0x80)
        {
            memcpy((*myPtr)[position].showBuffer[((*myPtr)[position].writeIndex)] + len, &naluHeader, 4);
            len += 4;
            memcpy((*myPtr)[position].showBuffer[((*myPtr)[position].writeIndex)] + len, &nal_fu, 1);
            len += 1;
        }

        memcpy((*myPtr)[position].showBuffer[((*myPtr)[position].writeIndex)] + len, &pkt_data[56], header->len - 56);
        (*myPtr)[position].showBufferLen[((*myPtr)[position].writeIndex)] = header->len - 56 + len;
        ++((*myPtr)[position].writeIndex);
    }
    else if(nal == 0x18)
    {
        flagLen++;
        while(flagLen < header -> len)
        {
            unsigned char seq[2];
            seq[0] = pkt_data[flagLen + 1];
            seq[1] = pkt_data[flagLen];
            unsigned short *pNalSize = (unsigned short *)&seq;

            flagLen += 2;

            memcpy((*myPtr)[position].showBuffer[((*myPtr)[position].writeIndex)] + len, &naluHeader, 4);
            len += 4;
            memcpy((*myPtr)[position].showBuffer[((*myPtr)[position].writeIndex)] + len, &pkt_data[flagLen], *pNalSize);
            (*myPtr)[position].showBufferLen[((*myPtr)[position].writeIndex)] = *pNalSize + 4;

            flagLen += *pNalSize;
            ++((*myPtr)[position].writeIndex);
            len = 0;

            if ((*myPtr)[position].writeIndex == BUFFER_LEN)
            {
                (*myPtr)[position].writeIndex = 0;
            }
        }
    }
    else
    {
        memcpy((*myPtr)[position].showBuffer[((*myPtr)[position].writeIndex)] + len, &naluHeader, 4);
        len += 4;
        memcpy((*myPtr)[position].showBuffer[((*myPtr)[position].writeIndex)] + len, &pkt_data[54], header->len - 54);
        (*myPtr)[position].showBufferLen[((*myPtr)[position].writeIndex)] = header->len - 54 + len;
        ++((*myPtr)[position].writeIndex);
    }
    if ((*myPtr)[position].writeIndex == BUFFER_LEN)
    {
        (*myPtr)[position].writeIndex = 0;
    }

}



