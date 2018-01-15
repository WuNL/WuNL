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
    avfilter_graph_free(&filter_graph[0]);
    avfilter_graph_free(&filter_graph[1]);
    avfilter_graph_free(&filter_graph[2]);
}

void fmDecoder::setThreadSeq(int seq)
{
    threadSeq_ = seq;
    Init();
}

boost::mutex mt;

int fmDecoder::testFun()
{
    int ret;
    AVFrame *frame_in;
    AVFrame *frame_out;
    unsigned char *frame_buffer_in;
    unsigned char *frame_buffer_out;

    AVFilterContext *buffersink_ctx;
    AVFilterContext *buffersrc_ctx;
    AVFilterGraph *filter_graph;
    static int video_stream_index = -1;

    //Input YUV
    FILE *fp_in=fopen("sintel_480x272_yuv420p.yuv","rb+");
    if(fp_in==NULL)
    {
        printf("Error open input file.\n");
        return -1;
    }
    int in_width=480;
    int in_height=272;

    //Output YUV
    FILE *fp_out=fopen("output.yuv","wb+");
    if(fp_out==NULL)
    {
        printf("Error open output file.\n");
        return -1;
    }

    //const char *filter_descr = "lutyuv='u=128:v=128'";
    //const char *filter_descr = "boxblur";
    //const char *filter_descr = "hflip";
    //const char *filter_descr = "hue='h=60:s=-3'";
    //const char *filter_descr = "crop=2/3*in_w:2/3*in_h";
    //const char *filter_descr = "drawbox=x=100:y=100:w=100:h=100:color=pink@0.5";
    //const char *filter_descr = "drawtext=fontfile=arial.ttf:fontcolor=green:fontsize=30:text='Lei Xiaohua'";
    const char *filter_descr = "hwupload_cuda,scale_npp=w=1316:h=1080:format=yuv420p:interp_algo=lanczos,hwdownload,format=yuv420p";

    avfilter_register_all();

    char args[512];
    AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
    AVBufferSinkParams *buffersink_params;

    filter_graph = avfilter_graph_alloc();

    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
             "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
             in_width,in_height,AV_PIX_FMT_YUV420P,
             1, 25,1,1);

    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
                                       args, NULL, filter_graph);
    if (ret < 0)
    {
        printf("Cannot create buffer source\n");
        return ret;
    }

    /* buffer video sink: to terminate the filter chain. */
    buffersink_params = av_buffersink_params_alloc();
    buffersink_params->pixel_fmts = pix_fmts;
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
                                       NULL, buffersink_params, filter_graph);
    av_free(buffersink_params);
    if (ret < 0)
    {
        printf("Cannot create buffer sink\n");
        return ret;
    }

    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_descr,
                                        &inputs, &outputs, NULL)) < 0)
        return ret;

    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0)
        return ret;

    frame_in=av_frame_alloc();
    frame_buffer_in=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, in_width,in_height,1));
    av_image_fill_arrays(frame_in->data, frame_in->linesize,frame_buffer_in,
                         AV_PIX_FMT_YUV420P,in_width, in_height,1);

    frame_out=av_frame_alloc();
    frame_buffer_out=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_YUV420P, in_width,in_height,1));
    av_image_fill_arrays(frame_out->data, frame_out->linesize,frame_buffer_out,
                         AV_PIX_FMT_YUV420P,in_width, in_height,1);

    frame_in->width=in_width;
    frame_in->height=in_height;
    frame_in->format=AV_PIX_FMT_YUV420P;

    while (1)
    {

        if(fread(frame_buffer_in, 1, in_width*in_height*3/2, fp_in)!= in_width*in_height*3/2)
        {
            break;
        }
        //input Y,U,V
        frame_in->data[0]=frame_buffer_in;
        frame_in->data[1]=frame_buffer_in+in_width*in_height;
        frame_in->data[2]=frame_buffer_in+in_width*in_height*5/4;

        if (av_buffersrc_add_frame(buffersrc_ctx, frame_in) < 0)
        {
            printf( "Error while add frame.\n");
            break;
        }

        /* pull filtered pictures from the filtergraph */
        ret = av_buffersink_get_frame(buffersink_ctx, frame_out);
        if (ret < 0)
            break;

        //output Y,U,V
        if(frame_out->format==AV_PIX_FMT_YUV420P)
        {
            for(int i=0; i<frame_out->height; i++)
            {
                fwrite(frame_out->data[0]+frame_out->linesize[0]*i,1,frame_out->width,fp_out);
            }
            for(int i=0; i<frame_out->height/2; i++)
            {
                fwrite(frame_out->data[1]+frame_out->linesize[1]*i,1,frame_out->width/2,fp_out);
            }
            for(int i=0; i<frame_out->height/2; i++)
            {
                fwrite(frame_out->data[2]+frame_out->linesize[2]*i,1,frame_out->width/2,fp_out);
            }
        }
        printf("Process 1 frame!\n");
        av_frame_unref(frame_out);
    }

    fclose(fp_in);
    fclose(fp_out);

    av_frame_free(&frame_in);
    av_frame_free(&frame_out);
    avfilter_graph_free(&filter_graph);
}

int fmDecoder::initFilter()
{
    int ret;

    int in_width=1920;
    int in_height=1080;

//    const char *filter_descr[3] = {"boxblur",
//                                   "boxblur",
//                                   "boxblur"
//                                  };
    const char *filter_descr[3] = {"hwupload_cuda,scale_npp=w=960:h=540:format=nv12:interp_algo=linear,hwdownload,format=nv12",
                                   "hwupload_cuda,scale_npp=w=640:h=360:format=nv12:interp_algo=linear,hwdownload,format=nv12",
                                   "hwupload_cuda,scale_npp=w=480:h=270:format=nv12:interp_algo=linear,hwdownload,format=nv12"
                                  };
    avfilter_register_all();

    for(int i=0; i<3; ++i)
    {
        char args[512];
        buffersrc[i]  = avfilter_get_by_name("buffer");
        buffersink[i] = avfilter_get_by_name("buffersink");
        outputs[i] = avfilter_inout_alloc();
        inputs[i]  = avfilter_inout_alloc();
        enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_NV12, AV_PIX_FMT_NONE };


        filter_graph[i] = avfilter_graph_alloc();

        /* buffer video source: the decoded frames from the decoder will be inserted here. */
        snprintf(args, sizeof(args),
                 "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
                 in_width,in_height,AV_PIX_FMT_NV12,
                 1, 60,1,1);

        ret = avfilter_graph_create_filter(&buffersrc_ctx[i], buffersrc[i], "in",
                                           args, NULL, filter_graph[i]);
        if (ret < 0)
        {
            printf("Cannot create buffer source\n");
            return ret;
        }

        /* buffer video sink: to terminate the filter chain. */
        buffersink_params[i] = av_buffersink_params_alloc();
        buffersink_params[i]->pixel_fmts = pix_fmts;
        ret = avfilter_graph_create_filter(&buffersink_ctx[i], buffersink[i], "out",
                                           NULL, buffersink_params[i], filter_graph[i]);
        av_free(buffersink_params[i]);
        if (ret < 0)
        {
            printf("Cannot create buffer sink\n");
            return ret;
        }

        /* Endpoints for the filter graph. */
        outputs[i]->name       = av_strdup("in");
        outputs[i]->filter_ctx = buffersrc_ctx[i];
        outputs[i]->pad_idx    = 0;
        outputs[i]->next       = NULL;

        inputs[i]->name       = av_strdup("out");
        inputs[i]->filter_ctx = buffersink_ctx[i];
        inputs[i]->pad_idx    = 0;
        inputs[i]->next       = NULL;

        if ((ret = avfilter_graph_parse_ptr(filter_graph[i], filter_descr[i],
                                            &inputs[i], &outputs[i], NULL)) < 0)
        {
            printf("avfilter_graph_parse_ptr error!\n");
            return ret;
        }

        if ((ret = avfilter_graph_config(filter_graph[i], NULL)) < 0)
        {
            printf("avfilter_graph_config error!\n");
            return ret;
        }
    }



    return ret;
}

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
    if(useNpp)
        initFilter();

    for(int i=2; i<5; ++i)
    {
        unsigned char *out_buffer1;
        pFrameYUV[i-2]=av_frame_alloc();
        out_buffer1=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_NV12,  1920/i,1080/i,1));
        av_image_fill_arrays(pFrameYUV[i-2]->data, pFrameYUV[i-2]->linesize,out_buffer1,
                             AV_PIX_FMT_NV12,1920/i,1080/i,1);
        convertCtx[i-2] = sws_getContext(1920, 1080, AV_PIX_FMT_NV12,
                                         1920/i,1080/i, AV_PIX_FMT_NV12, SWS_POINT, NULL, NULL, NULL);
        convertCtx720P[i-1] = sws_getContext(1280, 720, AV_PIX_FMT_NV12,
                                             1920/i,1080/i, AV_PIX_FMT_NV12, SWS_POINT, NULL, NULL, NULL);
        pFrameYUV[i-2]->format = AV_PIX_FMT_NV12;
    }
    //    unsigned char *out_buffer1080P;
//    out_buffer1080P=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_NV12,  1920,1080,1));
    pFrameYUV1080P = av_frame_alloc();
//    av_image_fill_arrays(pFrameYUV1080P->data, pFrameYUV1080P->linesize,out_buffer1080P,
//                         AV_PIX_FMT_NV12,1920,1080,1);
//    pFrameYUV1080P->format = AV_PIX_FMT_NV12;
//    convertCtx720P[0] = sws_getContext(1280, 720, AV_PIX_FMT_NV12,
//                                       1920,1080, AV_PIX_FMT_NV12, SWS_POINT, NULL, NULL, NULL);

    int sws_seqTMP = 0;
    int widthTMP = 0;
    int heightTMP = 0;
    AVFrame	*pFrame = av_frame_alloc();
//    unsigned char *out_buffer1080P_;
//    out_buffer1080P_=(unsigned char *)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_NV12,  1920,1080,1));
//    av_image_fill_arrays(pFrame->data, pFrame->linesize,out_buffer1080P_,
//                         AV_PIX_FMT_NV12,1920,1080,1);
//    pFrame->format = AV_PIX_FMT_NV12;
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
                        continue;
                    }

                    else if (ret < 0)
                    {
                        char buf[128]= {0};
                        av_make_error_string(buf,128,ret);
                        fprintf(stderr, "Error during decoding 1: %s\n", buf);
                        continue;
                    }
                    sws_seqTMP = sws_seq;
                    widthTMP = sws_width_;
                    heightTMP = sws_height_;
                    if(screanNum!=1)
                    {


                        pFrameYUV[sws_seqTMP]->width = widthTMP;
                        pFrameYUV[sws_seqTMP]->height = heightTMP;


                        AVFrame *copyFrame = av_frame_alloc();
                        if(useNpp)
                        {
                            pFrame->pts = av_frame_get_best_effort_timestamp(pFrame);
                            /* push the decoded frame into the filtergraph */
                            if (av_buffersrc_add_frame_flags(buffersrc_ctx[sws_seqTMP], pFrame, AV_BUFFERSRC_FLAG_KEEP_REF) < 0)
                            {
                                av_log(NULL, AV_LOG_ERROR, "Error while feeding the filtergraph\n");
                                break;
                            }

                            /* pull filtered frames from the filtergraph */

                            while (1)
                            {
                                ret = av_buffersink_get_frame(buffersink_ctx[sws_seqTMP], pFrameYUV[sws_seqTMP]);
                                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF)
                                    break;
                                if (ret < 0)
                                    break;
                                copyFrame->format = pFrameYUV[sws_seqTMP]->format;
                                copyFrame->width = pFrameYUV[sws_seqTMP]->width;
                                copyFrame->height = pFrameYUV[sws_seqTMP]->height;
                                av_frame_get_buffer(copyFrame, 32);
                                av_frame_copy(copyFrame, pFrameYUV[sws_seqTMP]);
                                av_frame_copy_props(copyFrame, pFrameYUV[sws_seqTMP]);

                                av_frame_unref(pFrameYUV[sws_seqTMP]);
                            }
                            av_frame_unref(pFrame);

                        }
                        else
                        {
                            if(pFrame->width==1920 && pFrame->height==1080)
                            {
                                sws_scale(convertCtx[sws_seqTMP], (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                                          pFrameYUV[sws_seqTMP]->data, pFrameYUV[sws_seqTMP]->linesize);
                            }
                            else if(pFrame->width==1280 && pFrame->height==720)
                            {
                                sws_scale(convertCtx720P[sws_seqTMP+1], (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                                          pFrameYUV[sws_seqTMP]->data, pFrameYUV[sws_seqTMP]->linesize);
                            }
                            else
                            {
                                continue;
                            }



                            copyFrame->format = pFrameYUV[sws_seqTMP]->format;
                            copyFrame->width = pFrameYUV[sws_seqTMP]->width;
                            copyFrame->height = pFrameYUV[sws_seqTMP]->height;
                            av_frame_get_buffer(copyFrame, 32);
                            av_frame_copy(copyFrame, pFrameYUV[sws_seqTMP]);
                            av_frame_copy_props(copyFrame, pFrameYUV[sws_seqTMP]);


                        }

                        if((*pFrameQueueVecPtr_)[threadSeq_].first.size()<=FRAME_BUFFER)
                        {
                            mutexPtr_->lock();
                            (*pFrameQueueVecPtr_)[threadSeq_].first.push(copyFrame);
                            mutexPtr_->unlock();
                        }
                        else
                        {
//                            printf("%d decoder buffer is full!\n",threadSeq_);
                            AVFrame* tmp = (*pFrameQueueVecPtr_)[threadSeq_].first.back();
                            mutexPtr_->lock();
                            av_frame_free(&tmp);
                            (*pFrameQueueVecPtr_)[threadSeq_].first.back() = copyFrame;
                            mutexPtr_->unlock();
                        }

                        //av_frame_free(&pFrame);
                    }
                    else
                    {
                        if(pFrame->width==1280 && pFrame->height==720)
                        {
                            pFrameYUV1080P->width = 1920;
                            pFrameYUV1080P->height = 1080;
                            sws_scale(convertCtx720P[0], (const unsigned char* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
                                      pFrameYUV1080P->data, pFrameYUV1080P->linesize);
                            AVFrame *copyFrame = av_frame_alloc();

                            copyFrame->format = pFrameYUV1080P->format;
                            copyFrame->width = pFrameYUV1080P->width;
                            copyFrame->height = pFrameYUV1080P->height;
                            av_frame_get_buffer(copyFrame, 32);
                            av_frame_copy(copyFrame, pFrameYUV1080P);
                            av_frame_copy_props(copyFrame, pFrameYUV1080P);

                            //av_frame_free(&pFrame);

                            if((*pFrameQueueVecPtr_)[threadSeq_].first.size()<=FRAME_BUFFER)
                            {
                                mutexPtr_->lock();
                                (*pFrameQueueVecPtr_)[threadSeq_].first.push(copyFrame);
                                mutexPtr_->unlock();
                            }
                            else
                            {
                                //                        printf("buffer full!\n");
                                AVFrame* tmp = (*pFrameQueueVecPtr_)[threadSeq_].first.back();
                                mutexPtr_->lock();
                                av_frame_free(&tmp);
                                (*pFrameQueueVecPtr_)[threadSeq_].first.back() = copyFrame;
                                mutexPtr_->unlock();
                            }
                        }
                        else if(pFrame->width==1920 && pFrame->height==1080)
                        {
                            AVFrame *copyFrame = av_frame_alloc();

                            copyFrame->format = pFrame->format;
                            copyFrame->width = pFrame->width;
                            copyFrame->height = pFrame->height;
                            av_frame_get_buffer(copyFrame, 32);
                            av_frame_copy(copyFrame, pFrame);
                            av_frame_copy_props(copyFrame, pFrame);

                            //av_frame_free(&pFrame);

                            if((*pFrameQueueVecPtr_)[threadSeq_].first.size()<=FRAME_BUFFER)
                            {
                                mutexPtr_->lock();
                                (*pFrameQueueVecPtr_)[threadSeq_].first.push(copyFrame);
                                mutexPtr_->unlock();
                            }
                            else
                            {
                                //                        printf("buffer full!\n");
                                AVFrame* tmp = (*pFrameQueueVecPtr_)[threadSeq_].first.back();
                                mutexPtr_->lock();
                                av_frame_free(&tmp);
                                (*pFrameQueueVecPtr_)[threadSeq_].first.back() = copyFrame;
                                mutexPtr_->unlock();
                            }
                        }
                    }

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
    screanNum = num;
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
