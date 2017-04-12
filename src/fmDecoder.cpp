#include "fmDecoder.h"

FILE *fp_out;
int mycount = 0;
fmDecoder::fmDecoder()
{
    //ctor
    codecId=AV_CODEC_ID_H264;
    pCodec = NULL;
    pCodecCtx = NULL;
    pCodecParserCtx = NULL;
    pFrameYUV = av_frame_alloc();
    screanNum = 16;

    out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  VIDEO_WIDE, VIDEO_HEIGHT,1));

    if(out_buffer == 0)
    {
        printf("out_buffer == 0\n");
    }
    fp_out = fopen("out.yuv", "wb");
    Init();
}

fmDecoder::~fmDecoder()
{
    //dtor
    if(NULL!=img_convert_ctx4)
        sws_freeContext(img_convert_ctx4);
    if(NULL!=img_convert_ctx9)
        sws_freeContext(img_convert_ctx9);
    if(NULL!=img_convert_ctx16)
        sws_freeContext(img_convert_ctx16);
    if(NULL!=pFrameYUV)
        av_frame_free(&pFrameYUV);
    if(NULL!=pCodecCtx)
        avcodec_close(pCodecCtx);
    if(NULL!=out_buffer)
        av_free(out_buffer);
}

void fmDecoder::setThreadSeq(int seq)
{
    threadSeq_ = seq;
}

boost::mutex mt;

void fmDecoder::run()
{
//    Set pthread_getaffinity_np
    int rc, i;
    cpu_set_t cpuset;
    pthread_t thread;

    thread = pthread_self();

    //Check no. of cores on the machine
    std::cout << "position:"<<threadSeq_<<"  "<<std::thread::hardware_concurrency() << std::endl;

    /* Set affinity mask */
    CPU_ZERO(&cpuset);
    int mask = threadSeq_%4;
    //for (i = 0; i < 8; i++) //I have 4 cores with 2 threads per core so running it for 8 times, modify it according to your lscpu o/p
    CPU_SET(mask, &cpuset);
    pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    /* Assign affinity mask to the thread */
    rc = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (rc != 0)
        std::cout << "Error calling pthread_getaffinity_np !!!";

    for (i = 0; i < CPU_SETSIZE; i++)
    {
        if (CPU_ISSET(i, &cpuset))
        {
            std::cout << " CPU " << i << std::endl;
            std::cout << "This program (main thread) is on CPU " << sched_getcpu() << std::endl;
        }
    }
    while(1)
    {
        int cur_size = 0;
        uint8_t *cur_ptr = NULL;

        if ((*cvPtr_)[threadSeq_].writeIndex != (*cvPtr_)[threadSeq_].readIndex)
        {
            cur_size = (*cvPtr_)[threadSeq_].showBufferLen[((*cvPtr_)[threadSeq_].readIndex)];
            cur_ptr = (*cvPtr_)[threadSeq_].showBuffer[((*cvPtr_)[threadSeq_].readIndex)];
            ++((*cvPtr_)[threadSeq_].readIndex);
            if ((*cvPtr_)[threadSeq_].readIndex == BUFFER_LEN)
            {
                (*cvPtr_)[threadSeq_].readIndex = 0;
            }
        }
        else
        {
            usleep(1000);
            continue;
        }
        if (cur_size == 0)
            break;
        while (cur_size>0)
        {
            int len = av_parser_parse2(
                          pCodecParserCtx, pCodecCtx,
                          &packet.data, &packet.size,
                          cur_ptr, cur_size,
                          AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);

            cur_ptr += len;
            cur_size -= len;
            if(GetPacketSize()==0)
                continue;
            switch(pCodecParserCtx->pict_type)
            {
            case AV_PICTURE_TYPE_I:
                printf("Type:I\t\n");
                break;
                //case AV_PICTURE_TYPE_P: printf("Type:P\t");break;
                //case AV_PICTURE_TYPE_B: printf("Type:B\t");break;
                //default: printf("Type:Other\t");break;
            }
            AVFrame	*pFrame = av_frame_alloc();
            int got_picture = 0;

            int ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
            if(ret < 0)
            {
                printf("DECODE ERROR\n");
                continue;
                //return ret;
            }
            if(!got_picture)
            {
                printf("got_picture == 0\n");
                continue;
                //return ret;
            }
            if (ret)
            {
                //printf("SUCCESS!\n");
                AVFrame *copyFrame = av_frame_alloc();
                copyFrame->format = pFrame->format;
                copyFrame->width = pFrame->width;
                copyFrame->height = pFrame->height;
                copyFrame->channels = pFrame->channels;
                copyFrame->channel_layout = pFrame->channel_layout;
                copyFrame->nb_samples = pFrame->nb_samples;
                av_frame_get_buffer(copyFrame, 32);
                av_frame_copy(copyFrame, pFrame);
                av_frame_copy_props(copyFrame, pFrame);
                (*pFrameQueueVecPtr_)[threadSeq_].push(copyFrame);
                av_frame_free(&pFrame);
                //av_frame_free(&copyFrame);
            }
        }
    }
    return;
}

void fmDecoder::startDecode()
{
    m_Thread = boost::thread(&fmDecoder::run, this);
}

void fmDecoder::setPtr(boost::shared_ptr<std::vector<channel> > cvPtr,
                       boost::shared_ptr<std::vector<std::vector<AVFrame*> > > pFrameVecPtr,
                       boost::shared_ptr<std::vector<int> >readIndex,
                       boost::shared_ptr<std::vector<int> > writeIndex)
{
    cvPtr_ = cvPtr;
    pFrameVecPtr_ = pFrameVecPtr;
    readIndex_=readIndex;
    writeIndex_ = writeIndex;
}

int fmDecoder::Init()
{
    avcodec_register_all();
    av_log_set_level(AV_LOG_QUIET);
    pCodec = avcodec_find_decoder(codecId);

    if (!pCodec)
    {
        printf("Codec not found\n");
        return -1;
    }
    //pCodec->capabilities &= AV_CODEC_CAP_FRAME_THREADS;
    pCodecCtx = avcodec_alloc_context3(pCodec);

    pCodecCtx->extradata = new uint8_t[32];//给extradata成员参数分配内存
    pCodecCtx->extradata_size = 32;//extradata成员参数分配内存大小



//给extradata成员参数设置值
//00 00 00 01
// pCodecCtx->extradata[0] = 0x00;
// pCodecCtx->extradata[1] = 0x00;
// pCodecCtx->extradata[2] = 0x00;
// pCodecCtx->extradata[3] = 0x01;
//
// //67 42 80 1e
// pCodecCtx->extradata[4] = 0x67;
// pCodecCtx->extradata[5] = 0x42;
// pCodecCtx->extradata[6] = 0x80;
// pCodecCtx->extradata[7] = 0x1e;
//
// //88 8b 40 50
// pCodecCtx->extradata[8] = 0x88;
// pCodecCtx->extradata[9] = 0x8b;
// pCodecCtx->extradata[10] = 0x40;
// pCodecCtx->extradata[11] = 0x50;
//
// //1e d0 80 00
// pCodecCtx->extradata[12] = 0x1e;
// pCodecCtx->extradata[13] = 0xd0;
// pCodecCtx->extradata[14] = 0x80;
// pCodecCtx->extradata[15] = 0x00;
//
// //03 84 00 00
// pCodecCtx->extradata[16] = 0x03;
// pCodecCtx->extradata[17] = 0x84;
// pCodecCtx->extradata[18] = 0x00;
// pCodecCtx->extradata[19] = 0x00;
//
// //af c8 02 00
// pCodecCtx->extradata[20] = 0xaf;
// pCodecCtx->extradata[21] = 0xc8;
// pCodecCtx->extradata[22] = 0x02;
// pCodecCtx->extradata[23] = 0x00;
//
// //00 00 00 01
// pCodecCtx->extradata[24] = 0x00;
// pCodecCtx->extradata[25] = 0x00;
// pCodecCtx->extradata[26] = 0x00;
// pCodecCtx->extradata[27] = 0x01;
//
// //68 ce 38 80
// pCodecCtx->extradata[28] = 0x68;
// pCodecCtx->extradata[29] = 0xce;
// pCodecCtx->extradata[30] = 0x38;
// pCodecCtx->extradata[31] = 0x80;


//    optimize work
    pCodecCtx->skip_frame       =  AVDISCARD_NONREF;
    pCodecCtx->skip_idct        =  AVDISCARD_ALL;
    pCodecCtx->idct_algo        =  1;
    pCodecCtx->has_b_frames     =  0;
    pCodecCtx->refs             =  1;

    if(pCodec->capabilities&CODEC_CAP_TRUNCATED)

        pCodecCtx->flags|= CODEC_FLAG_TRUNCATED;

    pCodec->capabilities |= CODEC_CAP_TRUNCATED;

    pCodecCtx->flags     |= CODEC_FLAG_LOW_DELAY;

    //pCodecCtx->thread_count = 4;
    //pCodecCtx->thread_type = FF_THREAD_FRAME;

    if (!pCodecCtx)
    {
        printf("Could not allocate video codec context\n");
        return -1;
    }
    const AVCodec* myTestcodec = pCodecCtx->codec;
    pCodecParserCtx=av_parser_init(codecId);
    if (!pCodecParserCtx)
    {
        printf("Could not allocate video parser context\n");
        return -1;
    }
    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
    {
        printf("Could not open codec\n");
        return -1;
    }
    av_init_packet(&packet);
    return 0;
}

int fmDecoder::Parse(int cur_size, uint8_t *cur_ptr)
{
    return av_parser_parse2(
               pCodecParserCtx, pCodecCtx,
               &packet.data, &packet.size,
               cur_ptr, cur_size,
               AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
}

int fmDecoder::Decode(AVFrame *pFrame)
{
    static int mc = 0;
    int got_picture = 0;

    int ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
    if(ret < 0)
    {
        printf("DECODE ERROR\n");
        return ret;
    }
    if(!got_picture)
    {
        printf("got_picture == 0\n");
        return ret;
    }
    mt.lock();
    if(1)
    {
        AVFrame *copyFrame = av_frame_alloc();
        copyFrame->format = pFrame->format;
        copyFrame->width = pFrame->width;
        copyFrame->height = pFrame->height;
        copyFrame->channels = pFrame->channels;
        copyFrame->channel_layout = pFrame->channel_layout;
        copyFrame->nb_samples = pFrame->nb_samples;
        av_frame_get_buffer(copyFrame, 32);
        av_frame_copy(copyFrame, pFrame);
        av_frame_copy_props(copyFrame, pFrame);
        (*pFrameQueueVecPtr_)[threadSeq_].push(copyFrame);
        av_frame_free(&pFrame);
        //av_frame_free(&copyFrame);
    }
    mt.unlock();
    return 1;
}

int fmDecoder::GetPacketSize()
{
    return packet.size;
}

void fmDecoder::SetScreanNum(int num)
{
    screanNum = num;
}
