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
    img_convert_ctx1 = sws_getContext(VIDEO_SOURCE_WIDTH, VIDEO_SOURCE_HEIGHT, AV_PIX_FMT_YUV420P,
                                      VIDEO_WIDE, VIDEO_HEIGHT, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    img_convert_ctx4 = sws_getContext(VIDEO_SOURCE_WIDTH, VIDEO_SOURCE_HEIGHT, AV_PIX_FMT_YUV420P,
                                      VIDEO_WIDE/2, VIDEO_HEIGHT/2, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    img_convert_ctx9 = sws_getContext(VIDEO_SOURCE_WIDTH, VIDEO_SOURCE_HEIGHT, AV_PIX_FMT_YUV420P,
                                      VIDEO_WIDE/3, VIDEO_HEIGHT/3, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
    img_convert_ctx16 = sws_getContext(VIDEO_SOURCE_WIDTH, VIDEO_SOURCE_HEIGHT, AV_PIX_FMT_YUV420P,
                                       VIDEO_WIDE/4, VIDEO_HEIGHT/4, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
//    out_buffer=(uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, VIDEO_WIDE, VIDEO_HEIGHT));
    out_buffer=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P,  VIDEO_WIDE, VIDEO_HEIGHT,1));
    //av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer,
    //                     AV_PIX_FMT_YUV420P,VIDEO_WIDE, VIDEO_HEIGHT,1);
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

    rc = pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (rc != 0)
        std::cout << "Error calling pthread_setaffinity_np !!! ";

    /* Assign affinity mask to the thread */
    rc = pthread_getaffinity_np(thread, sizeof(cpu_set_t), &cpuset);
    if (rc != 0)
        std::cout << "Error calling pthread_getaffinity_np !!!";

    std::cout << "pthread_getaffinity_np() returns:\n";
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

//    if(mycount>200 && threadSeq_==0  && !(*pFrameQueueVecPtr_)[threadSeq_].empty())
//    {
//        mt.lock();
//        std::cout<<"M2-------"<<threadSeq_<<"    "<<(*pFrameQueueVecPtr_)[threadSeq_].size()<<std::endl;
//        AVFrame* myF = (*pFrameQueueVecPtr_)[threadSeq_].front();
//
//        for(int i=0;i<myF->height;i++){
//            fwrite(myF->data[0]+myF->linesize[0]*i,1,myF->width,fp_out);
//        }
//        for(int i=0;i<myF->height/2;i++){
//            fwrite(myF->data[1]+myF->linesize[1]*i,1,myF->width/2,fp_out);
//        }
//        for(int i=0;i<myF->height/2;i++){
//            fwrite(myF->data[2]+myF->linesize[2]*i,1,myF->width/2,fp_out);
//        }
//        av_frame_free(&myF);
//        (*pFrameQueueVecPtr_)[threadSeq_].pop();
//        mt.unlock();
//    }


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
            usleep(5000);
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
            //
            //WuNL
            //int ret = Decode( (*pFrameVecPtr_)[threadSeq_][(*writeIndex_)[threadSeq_] ] );
            //av_free(pFrame);

            AVFrame	*pFrame = av_frame_alloc();
            int ret = Decode(pFrame);

//            std::cout<<"width is :"<<(*pFrameQueueVecPtr_)[threadSeq_].front()<<std::endl;
            if (ret < 0)
            {
                //av_free(pFrame);
                continue;
            }
            if (ret)
            {
                //printf("success!\n");
//                (*pFrameQueueVecPtr_)[threadSeq_].push(pFrame);
//                std::cout<<"size is :"<<(*pFrameQueueVecPtr_)[threadSeq_].size()<<std::endl;
                ++(*writeIndex_)[threadSeq_];
                if ((*writeIndex_)[threadSeq_] == FRAME_BUFFER)
                {
                    (*writeIndex_)[threadSeq_] = 0;
                }
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

    pCodec = avcodec_find_decoder(codecId);
    if (!pCodec)
    {
        printf("Codec not found\n");
        return -1;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx)
    {
        printf("Could not allocate video codec context\n");
        return -1;
    }
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

    av_log_set_level(AV_LOG_QUIET);
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
int first_time=1;
int fmDecoder::Decode(AVFrame *pFrame)
{
    static int mc = 0;
    int got_picture = 0;

    //pFrameYUV=av_frame_alloc();
    int ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
    if(ret < 0)
    {
        //printf("%s failed\n", "avcodec_decode_video2()");
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
        //std::cout<<threadSeq_<<"--------"<<(*pFrameQueueVecPtr_)[threadSeq_].size()<<std::endl;
    }
    mt.unlock();

//    printf("got pic !\n");
//    std::cout<<pFrameYUV->height<<std::endl;
//    std::cout<<pFrameYUV->width<<std::endl;
//    printf("got pic !\n");
//    if (got_picture) {
//
//        if(first_time){
//            printf("\nCodec Full Name:%s\n",pCodecCtx->codec->long_name);
//            printf("width:%d\nheight:%d\n\n",pCodecCtx->width,pCodecCtx->height);
//            first_time=0;
//        }
//        //Y, U, V
//        for(int i=0;i<pFrame->height;i++){
//            fwrite(pFrame->data[0]+pFrame->linesize[0]*i,1,pFrame->width,fp_out);
//        }
//        for(int i=0;i<pFrame->height/2;i++){
//            fwrite(pFrame->data[1]+pFrame->linesize[1]*i,1,pFrame->width/2,fp_out);
//        }
//        for(int i=0;i<pFrame->height/2;i++){
//            fwrite(pFrame->data[2]+pFrame->linesize[2]*i,1,pFrame->width/2,fp_out);
//        }
//
//        printf("Succeed to decode 1 frame!\n");
//    }

//    AVFrame* myFrame =av_frame_alloc();
//    memcpy(myFrame->data,pFrameYUV->data,sizeof(pFrameYUV->data));
////    myFrame->data = pFrameYUV->data;
//    myFrame->width = pFrameYUV->width;
//    myFrame->height = pFrameYUV->height;


    //AVFrame* myframe=av_frame_alloc();
    //myframe= av_frame_clone(pFrameYUV);
    //av_frame_free(&pFrameYUV);

//    SwsContext * myimg_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
//                   pCodecCtx->pix_fmt,
//                   1920, 1080, AV_PIX_FMT_RGB24, SWS_BICUBIC,
//                   NULL, NULL, NULL);
//    int a = sws_scale(myimg_convert_ctx, (const unsigned char* const*)pFrameYUV->data,
//                  pFrameYUV->linesize, 0, pCodecCtx->height, myframe->data, myframe->linesize);
//    std::cout<<a<<std::endl;

    return 1;

    if(screanNum == 16)
    {
        avpicture_fill((AVPicture *)pFrame, out_buffer, AV_PIX_FMT_YUV420P, VIDEO_WIDE/4, VIDEO_HEIGHT/4);
        sws_scale(img_convert_ctx16, (const unsigned char* const*)pFrameYUV->data,
                  pFrameYUV->linesize, 0, pCodecCtx->height, pFrame->data, pFrame->linesize);
    }
    else if(screanNum == 9)
    {
        avpicture_fill((AVPicture *)pFrame, out_buffer, AV_PIX_FMT_YUV420P, VIDEO_WIDE/3, VIDEO_HEIGHT/3);
        sws_scale(img_convert_ctx9, (const unsigned char* const*)pFrameYUV->data,
                  pFrameYUV->linesize, 0, pFrameYUV->height, pFrame->data, pFrame->linesize);
    }
    else if(screanNum == 4)
    {
        avpicture_fill((AVPicture *)pFrame, out_buffer, AV_PIX_FMT_YUV420P, VIDEO_WIDE/3, VIDEO_HEIGHT/3);
        sws_scale(img_convert_ctx4, (const unsigned char* const*)pFrameYUV->data,
                  pFrameYUV->linesize, 0, pFrameYUV->height, pFrame->data, pFrame->linesize);
    }
    else if(screanNum == 1)
    {
        avpicture_fill((AVPicture *)pFrame, out_buffer, AV_PIX_FMT_YUV420P, VIDEO_WIDE, VIDEO_HEIGHT);
        sws_scale(img_convert_ctx1, (const unsigned char* const*)pFrameYUV->data,
                  pFrameYUV->linesize, 0, pFrameYUV->height, pFrame->data, pFrame->linesize);
    }

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
