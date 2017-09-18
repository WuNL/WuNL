#include "fmDecoder.h"
#include <math.h>

FILE *fp_out;
int mycount = 0;
fmDecoder::fmDecoder()
{
    //ctor
    codecId= AV_CODEC_ID_H264;
    pCodec = NULL;
    pCodecCtx = NULL;
    pCodecParserCtx = NULL;

    screanNum = WINDOW_STYLE;
    screanNum_old = WINDOW_STYLE;
    sws_width_ = 1920/sqrt(WINDOW_STYLE);
    sws_height_ = 1080/sqrt(WINDOW_STYLE);
    sws_seq = sqrt(WINDOW_STYLE) - 2;

    fp_out = fopen("out.yuv", "wb");

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
        av_frame_free(&pFrameYUV[0]);
    if(NULL!=pCodecCtx)
        avcodec_close(pCodecCtx);
    if(NULL!=fp_out)
        fclose(fp_out);
}

void fmDecoder::setThreadSeq(int seq)
{
    threadSeq_ = seq;
    Init();
}

boost::mutex mt;

void fmDecoder::run()
{
    //Set pthread_getaffinity_np
    int rc, i;
    static int cnt =0;
    cpu_set_t cpuset;
    pthread_t thread;

    thread = pthread_self();

    //Check no. of cores on the machine
    std::cout << "position:"<<threadSeq_<<"  "<<std::thread::hardware_concurrency() << std::endl;

    /* Set affinity mask */
    CPU_ZERO(&cpuset);
    int mask = threadSeq_;
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
            std::cout << " CPU " << i << std::endl;
            std::cout << "This program (main thread) is on CPU " << sched_getcpu() << std::endl;
        }
    }
    for(int i=2;i<5;++i)
    {
        unsigned char *out_buffer1;
        pFrameYUV[i-2]=av_frame_alloc();
        out_buffer1=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_NV12,  1920/i,1080/i,1));
        av_image_fill_arrays(pFrameYUV[i-2]->data, pFrameYUV[i-2]->linesize,out_buffer1,
                             AV_PIX_FMT_NV12,1920/i,1080/i,1);
        convertCtx[i-2] = sws_getContext(1920, 1080, AV_PIX_FMT_NV12,
                                    1920/i,1080/i, AV_PIX_FMT_NV12, SWS_BICUBIC, NULL, NULL, NULL);
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
            usleep(100);
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
//            switch(pCodecParserCtx->pict_type)
//            {
//            case AV_PICTURE_TYPE_I:
//                printf("Type:I\t\n");
//                break;
//                case AV_PICTURE_TYPE_P: printf("Type:P\t");break;
//                case AV_PICTURE_TYPE_B: printf("Type:B\t");break;
//                default: printf("Type:Other\t");break;
//            }
            AVFrame	*pFrame = av_frame_alloc();
            int got_picture = 0;

            int ret = -1;
            ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
            if(ret < 0)
            {
                printf("DECODE ERROR---------%d\n",ret);
                continue;
                //return ret;
            }
            if(!got_picture)
            {
                continue;
            }
            if (got_picture)
            {
                if(screanNum!=1)
                {
                    pFrameYUV[sws_seq]->format = AV_PIX_FMT_NV12;
                    pFrameYUV[sws_seq]->width = sws_width_;
                    pFrameYUV[sws_seq]->height = sws_height_;
                    int rev = sws_scale(convertCtx[sws_seq], (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                                        pFrameYUV[sws_seq]->data, pFrameYUV[sws_seq]->linesize);
                    AVFrame *copyFrame = av_frame_alloc();

                    copyFrame->format = pFrameYUV[sws_seq]->format;
                    copyFrame->width = pFrameYUV[sws_seq]->width;
                    copyFrame->height = pFrameYUV[sws_seq]->height;
                    av_frame_get_buffer(copyFrame, 32);
                    av_frame_copy(copyFrame, pFrameYUV[sws_seq]);
                    av_frame_copy_props(copyFrame, pFrameYUV[sws_seq]);

                    if((*pFrameQueueVecPtr_)[threadSeq_].size()<=30)
                    {
                        (*pFrameQueueVecPtr_)[threadSeq_].push(copyFrame);
                    }
                    else
                    {
//                        printf("buffer full!\n");
                        AVFrame* tmp = (*pFrameQueueVecPtr_)[threadSeq_].back();
                        av_frame_free(&tmp);
                        (*pFrameQueueVecPtr_)[threadSeq_].back() = copyFrame;
                    }

                    av_frame_free(&pFrame);
                }
                else
                {
                    AVFrame *copyFrame = av_frame_alloc();

                    copyFrame->format = pFrame->format;
                    copyFrame->width = pFrame->width;
                    copyFrame->height = pFrame->height;
                    av_frame_get_buffer(copyFrame, 32);
                    av_frame_copy(copyFrame, pFrame);
                    av_frame_copy_props(copyFrame, pFrame);

                    if((*pFrameQueueVecPtr_)[threadSeq_].size()<=30)
                    {
                        (*pFrameQueueVecPtr_)[threadSeq_].push(copyFrame);
                    }
                    else
                    {
//                        printf("buffer full!\n");
                        AVFrame* tmp = (*pFrameQueueVecPtr_)[threadSeq_].back();
                        av_frame_free(&tmp);
                        (*pFrameQueueVecPtr_)[threadSeq_].back() = copyFrame;
                    }

                    av_frame_free(&pFrame);
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
    av_log_set_level(AV_LOG_QUIET);
    //pCodec = avcodec_find_decoder(codecId);
    pCodec = avcodec_find_decoder_by_name("h264_cuvid");
    if (!pCodec)
    {
        printf("Codec not found\n");
        return -1;
    }
    //pCodec->capabilities &= AV_CODEC_CAP_FRAME_THREADS;
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (pCodecCtx->codec_id == AV_CODEC_ID_H264)
    {
        if(threadSeq_<8)
        {
            printf("seq %d using gpu 0\n",threadSeq_);
            av_opt_set(pCodecCtx->priv_data, "gpu", "0", 0);
        }

        if(threadSeq_>=8)
        {
            printf("seq %d using gpu 1\n",threadSeq_);
            av_opt_set(pCodecCtx->priv_data, "gpu", "1", 1);
        }
    }

    if (!pCodecCtx)
    {
        printf("Could not allocate video codec context\n");
        return -1;
    }
    printf("pCodecCtx->codec->id=%d----%d\n",pCodecCtx->codec->id,pCodec->id);
    pCodecParserCtx=av_parser_init(pCodec->id);


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
//    static int mc = 0;
    int got_picture = 0;

    int ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, &packet);
    if(ret < 0)
    {
        printf("DECODE ERROR----%d\n",ret);
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
        av_frame_free(&copyFrame);
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
    switch(screanNum)
    {
    case 1:
        {
            sws_width_ = 1920;
            sws_height_ = 1080;
            sws_seq = -1;
            break;
        }
    case 4:
        {
            sws_width_ = 1920/2;
            sws_height_ = 1080/2;
            sws_seq = 0;
            break;
        }
    case 9:
        {
            sws_width_ = 1920/3;
            sws_height_ = 1080/3;
            sws_seq = 1;
            break;
        }
    case 16:
        {
            sws_width_ = 1920/4;
            sws_height_ = 1080/4;
            sws_seq = 2;
            break;
        }
    default:
        {
            sws_width_ = 1920/4;
            sws_height_ = 1080/4;
            screanNum = 16;
            sws_seq = 2;
        }
    }
}
