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
    sws_width_ = 1920/3;
    sws_height_ = 1080/3;
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
        av_frame_free(&pFrameYUV);
    if(NULL!=pCodecCtx)
        avcodec_close(pCodecCtx);
    if(NULL!=fp_out)
        fclose(fp_out);
    if(nullptr!=fp_out_264)
        fclose(fp_out_264);

}

void fmDecoder::setThreadSeq(int seq)
{
    threadSeq_ = seq;
    Init();

    std::string file_name = "/home/sdt/Videos/" + std::to_string(threadSeq_) + "_test.264";
    fp_out_264=fopen(file_name.c_str(),"wb+");
    if(fp_out==NULL)
    {
        printf("Error open output file.\n");
        return;
    }
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
    int mask = threadSeq_/3;
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

    convertCtx = sws_getContext(1920, 1080, AV_PIX_FMT_NV12,
                                sws_width_,sws_height_, AV_PIX_FMT_NV12, SWS_POINT, NULL, NULL, NULL);
    convertCtx720P = sws_getContext(1280, 720, AV_PIX_FMT_NV12,
                                    sws_width_,sws_height_, AV_PIX_FMT_NV12, SWS_POINT, NULL, NULL, NULL);
    convertCtx576P = sws_getContext(1024, 576, AV_PIX_FMT_NV12,
                                    sws_width_,sws_height_, AV_PIX_FMT_NV12, SWS_POINT, NULL, NULL, NULL);
    unsigned char *out_buffer1;
    out_buffer1=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_NV12,  1920,1080,1));

    int sws_seqTMP = 0;
    int widthTMP = 0;
    int heightTMP = 0;

    while(1)
    {
        int cur_size = 0;
        uint8_t *cur_ptr = NULL;


        std::unique_ptr<frameObject> frame;
        if(use_webrtc)
        {
            frame =  (*cvPtr_)[threadSeq_].fb_->get_available_frame();
            if(frame == nullptr)
            {
                usleep(5);
                continue;
            }

            uint16_t last_seq_in_frame = frame->get_last_seq_num();
            (*cvPtr_)[threadSeq_].fb_->ClearTo(last_seq_in_frame);
            (*cvPtr_)[threadSeq_].pb_->ClearTo(last_seq_in_frame);
            fwrite(frame->data(), 1, frame->buffer_len_, fp_out_264);

            cur_ptr = frame->data();
            cur_size = static_cast<int>(frame->buffer_len_);
        }
        else
        {
//            if ((*cvPtr_)[threadSeq_].writeIndex != (*cvPtr_)[threadSeq_].readIndex)
//            {
//                cur_size = (*cvPtr_)[threadSeq_].showBufferLen[((*cvPtr_)[threadSeq_].readIndex)];
//                cur_ptr = (*cvPtr_)[threadSeq_].showBuffer[((*cvPtr_)[threadSeq_].readIndex)];
//                ++((*cvPtr_)[threadSeq_].readIndex);
//                if ((*cvPtr_)[threadSeq_].readIndex == BUFFER_LEN)
//                {
//                    (*cvPtr_)[threadSeq_].readIndex = 0;
//                }
//            }
//            else
//            {
//                usleep(100);
//                continue;
//            }
//            if (cur_size == 0)
//                break;
        }

        int ret = 0;
        while (cur_size>0)
        {
            int len = 0;
            try
            {
                len = av_parser_parse2(
                          pCodecParserCtx, pCodecCtx,
                          &packet.data, &packet.size,
                          cur_ptr, cur_size,
                          AV_NOPTS_VALUE, AV_NOPTS_VALUE, AV_NOPTS_VALUE);
            }
            catch(...)
            {
                printf(" av_parser_parse2 error occus! \n");
            }
            cur_ptr += len;
            cur_size -= len;

            if(packet.size)
            {
                try
                {
                    ret = avcodec_send_packet(pCodecCtx, &packet);
                }
                catch(...)
                {
                    printf(" avcodec_send_packet error occus! \n");
                }
                if (ret < 0)
                {
                    fprintf(stderr, "Error sending a packet for decoding\n");
                    continue;
                }
                while (ret >= 0)
                {
                    AVFrame	*pFrame = av_frame_alloc();
                    try
                    {
                        ret = avcodec_receive_frame(pCodecCtx, pFrame);
                    }
                    catch(...)
                    {
                        printf(" avcodec_receive_frame error occus! \n");
                    }
                    if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                    {
//                        char buf[128]= {0};
//                        av_make_error_string(buf,128,ret);
//                        fprintf(stderr, "Error during decoding 0: %s\n", buf);
                        av_frame_free(&pFrame);
                        continue;
                    }

                    else if (ret < 0)
                    {
                        char buf[128]= {0};
                        av_make_error_string(buf,128,ret);
                        fprintf(stderr, "Error during decoding 1: %s\n", buf);
                        av_frame_free(&pFrame);
                        continue;
                    }
                    widthTMP = sws_width_;
                    heightTMP = sws_height_;
                    if(1)
                    {

                        pFrameYUV=av_frame_alloc();

                        av_image_fill_arrays(pFrameYUV->data, pFrameYUV->linesize,out_buffer1,
                                             AV_PIX_FMT_NV12,sws_width_,sws_height_,1);

                        pFrameYUV->width = widthTMP;
                        pFrameYUV->height = heightTMP;
                        pFrameYUV->format = AV_PIX_FMT_NV12;

                        if(pFrame->width==1920 && pFrame->height==1080)
                        {
                            sws_scale(convertCtx, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                                      pFrameYUV->data, pFrameYUV->linesize);
                        }
                        else if(pFrame->width==1280 && pFrame->height==720)
                        {
                            sws_scale(convertCtx720P, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                                      pFrameYUV->data, pFrameYUV->linesize);
                        }
                        else if(pFrame->width==1024 && pFrame->height==576)
                        {
                            sws_scale(convertCtx576P, (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                                      pFrameYUV->data, pFrameYUV->linesize);
                        }
                        else
                        {
                            printf("%dX%d is unsupported resultion!\n",pFrame->width,pFrame->height);
                            av_frame_free(&pFrame);
                            continue;
                        }
                        av_frame_free(&pFrame);
//                        av_frame_free(&pFrameYUV);

                        struct timeval t_start;
                        gettimeofday(&t_start,NULL);
                        uint64_t start = ((uint64_t)t_start.tv_sec)*1000+(uint64_t)t_start.tv_usec/1000;
                        pFrameYUV->pts = start;


                        if((*pFrameQueueVecPtr_)[threadSeq_].first.size()<=FRAME_BUFFER)
                        {
                            mutexPtr_->lock();
                            (*pFrameQueueVecPtr_)[threadSeq_].first.push(std::move(pFrameYUV));
                            mutexPtr_->unlock();
                        }
                        else
                        {
                            mutexPtr_->lock();
                            AVFrame* tmp = (*pFrameQueueVecPtr_)[threadSeq_].first.front();
                            (*pFrameQueueVecPtr_)[threadSeq_].first.pop();
                            av_frame_free(&tmp);

                            (*pFrameQueueVecPtr_)[threadSeq_].first.push(std::move(pFrameYUV));
                            mutexPtr_->unlock();
                        }


                    }
//                    else
//                    {
//                        if(pFrame->width==1280 && pFrame->height==720)
//                        {
//                            pFrameYUV1080P->width = 1920;
//                            pFrameYUV1080P->height = 1080;
//                            sws_scale(convertCtx720P[0], (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
//                                      pFrameYUV1080P->data, pFrameYUV1080P->linesize);
//
//
//                            if((*pFrameQueueVecPtr_)[threadSeq_].first.size()<=FRAME_BUFFER)
//                            {
//                                mutexPtr_->lock();
//                                (*pFrameQueueVecPtr_)[threadSeq_].first.push(std::move(pFrame));
//                                mutexPtr_->unlock();
//                            }
//                            else
//                            {
//                                mutexPtr_->lock();
//                                AVFrame* tmp = (*pFrameQueueVecPtr_)[threadSeq_].first.front();
//                                (*pFrameQueueVecPtr_)[threadSeq_].first.pop();
//                                av_frame_free(&tmp);
//
//                                (*pFrameQueueVecPtr_)[threadSeq_].first.push(std::move(pFrame));
//                                mutexPtr_->unlock();
//                            }
//                        }
//                        else if(pFrame->width==1920 && pFrame->height==1080)
//                        {
//
//                            if((*pFrameQueueVecPtr_)[threadSeq_].first.size()<=FRAME_BUFFER)
//                            {
//                                mutexPtr_->lock();
//                                (*pFrameQueueVecPtr_)[threadSeq_].first.push(std::move(pFrame));
//                                mutexPtr_->unlock();
//                            }
//                            else
//                            {
//                                mutexPtr_->lock();
//                                AVFrame* tmp = (*pFrameQueueVecPtr_)[threadSeq_].first.front();
//                                (*pFrameQueueVecPtr_)[threadSeq_].first.pop();
//                                av_frame_free(&tmp);
//                                (*pFrameQueueVecPtr_)[threadSeq_].first.push(std::move(pFrame));
//                                mutexPtr_->unlock();
//                            }
//                        }
//                        else if(pFrame->width==1024 && pFrame->height==576)
//                        {
//                            pFrameYUV1080P->width = 1920;
//                            pFrameYUV1080P->height = 1080;
//                            sws_scale(convertCtx576P[0], (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
//                                      pFrameYUV1080P->data, pFrameYUV1080P->linesize);
//
//                            if((*pFrameQueueVecPtr_)[threadSeq_].first.size()<=FRAME_BUFFER)
//                            {
//                                mutexPtr_->lock();
//                                (*pFrameQueueVecPtr_)[threadSeq_].first.push(std::move(pFrame));
//                                mutexPtr_->unlock();
//                            }
//                            else
//                            {
//                                mutexPtr_->lock();
//                                AVFrame* tmp = (*pFrameQueueVecPtr_)[threadSeq_].first.front();
//                                (*pFrameQueueVecPtr_)[threadSeq_].first.pop();
//                                av_frame_free(&tmp);
//
//                                (*pFrameQueueVecPtr_)[threadSeq_].first.push(std::move(pFrame));
//                                mutexPtr_->unlock();
//                            }
//                        }
//                    }

                }
            }
            else
                continue;





        }
    }
    return;
}

void fmDecoder::startDecode()
{
    m_Thread = boost::thread(&fmDecoder::run, this);
}

void fmDecoder::setPtr(boost::shared_ptr<std::vector<channel> > cvPtr,
                       boost::shared_ptr<std::vector<int> >readIndex,
                       boost::shared_ptr<std::vector<int> > writeIndex)
{
    cvPtr_ = cvPtr;
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
//    pCodecCtx->hwaccel = ff_find_hwaccel();
    if (pCodecCtx->codec_id == AV_CODEC_ID_H264)
    {
//        if(threadSeq_<8)
        {
            printf("seq %d using gpu 0\n",threadSeq_);
            av_opt_set(pCodecCtx->priv_data, "gpu", "0", 0);
        }

//        if(threadSeq_>=8)
//        {
//            printf("seq %d using gpu 1\n",threadSeq_);
//            av_opt_set(pCodecCtx->priv_data, "gpu", "1", 1);
//        }
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
        (*pFrameQueueVecPtr_)[threadSeq_].first.push(copyFrame);
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
    screanNum = 4;
    switch(screanNum)
    {
    case 1:
    {
        sws_width_ = 1920/2;
        sws_height_ = 1080/2;
        sws_seq = 0;
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
