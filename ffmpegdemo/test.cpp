////
////  main.cpp
////  ffmpegdemo
////
////  Created by 王晓鹏 on 2020/6/22.
////  Copyright © 2020 王晓鹏. All rights reserved.
////
//
//#include <iostream>
//#include <thread>
//#include <netinet/in.h> //for sockaddr_in
//#include <arpa/inet.h>  //for socket
//
//#include "rtptcpaddress.h"
//#include "rtpsession.h"
//#include "rtpsessionparams.h"
//#include "rtptcptransmitter.h"
//#include "rtpipv4address.h"
//#include "rtptimeutilities.h"
//#include "rtppacket.h"
//#include "rtpabortdescriptors.h"
//
//
//extern "C" {
//#include "libavformat/avformat.h"
//#include "libavcodec/avcodec.h"
//#include "libavutil/opt.h"
//#include "libavutil/mathematics.h"
//#include "libswscale/swscale.h"
//#include "libswresample/swresample.h"
//}
//
//
//using namespace jrtplib;
//using namespace std;
//
//
//
//int rtsp2mp4()
//{
//    AVFormatContext *i_fmt_ctx; AVStream *i_video_stream;
//    AVFormatContext *o_fmt_ctx; AVStream *o_video_stream;
//    bool bStop = false;
//    
//    avcodec_register_all();
//    av_register_all();
//    avformat_network_init();
//    
//    /* should set to NULL so that avformat_open_input() allocate a new one */
//    i_fmt_ctx = NULL;
//    char rtspUrl[] = "rtsp://admin:ad53937301@58.221.84.90:5540/h264/ch33/main/av_stream";
//    const char *filename = "./1.mp4";
//    if (avformat_open_input(&i_fmt_ctx, rtspUrl, NULL, NULL)!=0)
//    {
//        fprintf(stderr, "could not open input file\n");
//        return -1;
//    }
//    
//    if (avformat_find_stream_info(i_fmt_ctx, NULL)<0)
//    {
//        fprintf(stderr, "could not find stream info\n");
//        return -1;
//    }
//    
//    av_dump_format(i_fmt_ctx, 0, rtspUrl, 0);
//    
//    /* find first video stream */
//    for (unsigned i=0; i<i_fmt_ctx->nb_streams; i++)
//    {
//        if (i_fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
//        {
//            i_video_stream = i_fmt_ctx->streams[i];
//            break;
//        }
//    }
//    if (i_video_stream == NULL)
//    {
//        fprintf(stderr, "didn't find any video stream\n");
//        return -1;
//    }
//    
//    avformat_alloc_output_context2(&o_fmt_ctx, NULL, NULL, filename);
//    
//    /*
//     * since all input files are supposed to be identical (framerate, dimension, color format, ...)
//     * we can safely set output codec values from first input file
//     */
//    o_video_stream = avformat_new_stream(o_fmt_ctx, NULL);
//    {
//        AVCodecContext *c;
//        c = o_video_stream->codec;
//        c->bit_rate = 400000;
//        c->codec_id = i_video_stream->codec->codec_id;
//        c->codec_type = i_video_stream->codec->codec_type;
//        c->time_base.num = i_video_stream->time_base.num;
//        c->time_base.den = i_video_stream->time_base.den;
//        fprintf(stderr, "time_base.num = %d time_base.den = %d\n", c->time_base.num, c->time_base.den);
//        c->width = i_video_stream->codec->width;
//        c->height = i_video_stream->codec->height;
//        c->pix_fmt = i_video_stream->codec->pix_fmt;
//        printf("%d %d %d", c->width, c->height, c->pix_fmt);
//        c->flags = i_video_stream->codec->flags;
//        c->flags |= CODEC_FLAG_GLOBAL_HEADER;
//        c->me_range = i_video_stream->codec->me_range;
//        c->max_qdiff = i_video_stream->codec->max_qdiff;
//        
//        c->qmin = i_video_stream->codec->qmin;
//        c->qmax = i_video_stream->codec->qmax;
//        
//        c->qcompress = i_video_stream->codec->qcompress;
//    }
//    
//    avio_open(&o_fmt_ctx->pb, filename, AVIO_FLAG_WRITE);
//    
//    avformat_write_header(o_fmt_ctx, NULL);
//    
//    int last_pts = 0;
//    int last_dts = 0;
//    
//    int64_t pts, dts;
//    while (!bStop)
//    {
//        AVPacket i_pkt;
//        av_init_packet(&i_pkt);
//        i_pkt.size = 0;
//        i_pkt.data = NULL;
//        if (av_read_frame(i_fmt_ctx, &i_pkt) <0 )
//            break;
//        
//        i_pkt.data;
//        i_pkt.size;
//        
//        /*
//         * pts and dts should increase monotonically
//         * pts should be >= dts
//         */
//        i_pkt.flags |= AV_PKT_FLAG_KEY;
//        pts = i_pkt.pts;
//        i_pkt.pts += last_pts;
//        dts = i_pkt.dts;
//        i_pkt.dts += last_dts;
//        i_pkt.stream_index = 0;
//        
//        //printf("%lld %lld\n", i_pkt.pts, i_pkt.dts);
//        static int num = 1;
//        printf("frame %d\n", num++);
//        av_interleaved_write_frame(o_fmt_ctx, &i_pkt);
//        //av_free_packet(&i_pkt);
//        //av_init_packet(&i_pkt);
//        
//        last_dts += dts;
//        last_pts += pts;
//    }
//    
//    
//    avformat_close_input(&i_fmt_ctx);
//    
//    av_write_trailer(o_fmt_ctx);
//    
//    avcodec_close(o_fmt_ctx->streams[0]->codec);
//    av_freep(&o_fmt_ctx->streams[0]->codec);
//    av_freep(&o_fmt_ctx->streams[0]);
//    
//    avio_close(o_fmt_ctx->pb);
//    av_free(o_fmt_ctx);
//    
//    return 0;
//}
//
//int OpenRtspStream(const char* url,AVFormatContext **ic)
//{
//    AVDictionary* options = NULL;
//    int ret=-1;
//    ret=av_dict_set(&options,"rtsp_transport", "tcp", 0);
//    if(ret<0)
//        return -1;
//    ret=av_dict_set(&options,"stimeout","10000000",0);
//    if(ret<0)
//        return -1;
//    if(avformat_open_input(ic,url,NULL,&options)!=0)          //avformat_close_input 关闭
//    {
//        if(!(*ic))
//            avformat_free_context(*ic);
//        return -1;
//    }
//    if(avformat_find_stream_info(*ic,NULL)<0)
//    {
//        if(!(*ic))
//        {
//            avformat_close_input(ic);
//            avformat_free_context(*ic);
//        }
//        return -1;
//    }
//    
//    printf("-----------rtsp流输入信息--------------\n");
//    av_dump_format(*ic, 0, url,0);
//    printf("---------------------------------------\n");
//    printf("\n");
//    return 0;
//}
//
//
//
//
//
//
//RTPSession session;
//
//int sendRTP()
//{
//    avcodec_register_all();
//    av_register_all();
//    avformat_network_init();
//    
//    AVFormatContext *i_fmt_ctx;
//    AVStream *i_video_stream;
//    
//    /* should set to NULL so that avformat_open_input() allocate a new one */
//    i_fmt_ctx = NULL;
//    char rtspUrl[] = "rtsp://admin:ad53937301@58.221.84.90:5540/h264/ch33/main/av_stream";
//    if (avformat_open_input(&i_fmt_ctx, rtspUrl, NULL, NULL)!=0)
//    {
//        fprintf(stderr, "could not open input file\n");
//        return -1;
//    }
//    
//    if (avformat_find_stream_info(i_fmt_ctx, NULL)<0)
//    {
//        fprintf(stderr, "could not find stream info\n");
//        return -1;
//    }
//    
//    av_dump_format(i_fmt_ctx, 0, rtspUrl, 0);
//    
//    /* find first video stream */
//    for (unsigned i=0; i<i_fmt_ctx->nb_streams; i++)
//    {
//        if (i_fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
//        {
//            i_video_stream = i_fmt_ctx->streams[i];
//            break;
//        }
//    }
//    if (i_video_stream == NULL)
//    {
//        fprintf(stderr, "didn't find any video stream\n");
//        return -1;
//    }
//    
//    while (true)
//    {
//        AVPacket i_pkt;
//        av_init_packet(&i_pkt);
//        i_pkt.size = 0;
//        i_pkt.data = NULL;
//        if (av_read_frame(i_fmt_ctx, &i_pkt) <0 )
//            break;
//        
//        //发送数据
//        session.SendPacket(i_pkt.data,i_pkt.size);
//    }
//    
//    avformat_close_input(&i_fmt_ctx);
//    
//    return 0;
//}
//
//
//void initRTPSession() {
//    RTPSession session;
//    RTPAbortDescriptors m_descriptors;
//    
//    RTPSessionParams sessionparams;
//    sessionparams.SetAcceptOwnPackets(true);
//    sessionparams.SetOwnTimestampUnit(1.0/10.0);
//    
//    m_descriptors.Init();
//    
//    RTPTCPTransmissionParams transparams;
//    transparams.SetCreatedAbortDescriptors(&m_descriptors);
//    int status = session.Create(sessionparams,&transparams,RTPTransmitter::TCPProto);
//    
//    if (status < 0)
//    {
//        return;
//    }
//    
//    //初始化socket
//    struct sockaddr_in platform_addr;
//    memset(&platform_addr, 0, sizeof(platform_addr));
//    platform_addr.sin_port = htons(2099);
//    platform_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
//    platform_addr.sin_family = AF_INET; //设置为IP通信
//    
//    /*创建客户端套接字--IPv4协议，面向连接通信，TCP协议*/
//    int socket_fd;
//    if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
//    {
//        perror("socket_fd error");
//        return;
//    }
//    
//    //连接服务器
//    if (connect(socket_fd, (struct sockaddr *)&platform_addr, sizeof(platform_addr)) < 0)
//    {
//        perror("socket_fd connect error");
//        return;
//    }
//    
//    RTPTCPAddress addr(socket_fd);
//    
//    status = session.AddDestination(addr);
//    if (status < 0)
//    {
//        return;
//    }
//    
//    session.SetDefaultPayloadType(96);
//    session.SetDefaultMark(false);
//    session.SetDefaultTimestampIncrement(160);
//    
//    //    //发送数据
//    //    session.SendPacket(str.data(),str.length());
//    //
//    //    RTPTime delay(0.020);
//    //    session.BYEDestroy(delay,"Client End",9);
//}
//
//
//int main(int argc, const char * argv[]) {
//    
//    //    int ret = 0;
//    //    std::string url = "rtsp://admin:ad53937301@58.221.84.90:5540/h264/ch33/main/av_stream";
//    
//    initRTPSession();
//    
//    sendRTP();
//    
//    return 0;
//    
//    
//    //    AVFormatContext *ifmt = NULL;
//    //    ifmt = avformat_alloc_context();
//    //    if(!ifmt)
//    //    {
//    //        cout<<"avformatcontext alloc error"<<endl;
//    //        return -1;
//    //    }
//    //    ret = OpenRtspStream(url.c_str(), &ifmt);
//    //    if(ret<0)
//    //    {
//    //        cout<<"摄像机网络不通"<<endl;
//    //        return -1;
//    //    }
//    //
//    
//    
//    
//    
//    
//    
//    
//    
//    //    av_register_all();
//    //
//    //    //使用RTSP网络流时必须先执行
//    //    ret = avformat_network_init();
//    //    printf("avformat_network_init ret = %d", ret);
//    //
//    //    AVFrame * pFrame = av_frame_alloc();
//    //    AVFormatContext *fmtctx = avformat_alloc_context();
//    //
//    //    //设置流格式为RTP
//    //    fmtctx->oformat = av_guess_format("rtp", NULL, NULL);
//    //
//    //    //用指定IP和端口构造输出流地址
//    //    sprintf(fmtctx->filename,"rtp://%s:%d", "127.0.0.1",55555);
//    //
//    //    //打开输出流
//    //    avio_open(&fmtctx->pb,fmtctx->filename, AVIO_FLAG_WRITE);
//    //
//    //
//    //    //查找编码器
//    //    AVCodec *codec = avcodec_find_encoder(AV_CODEC_ID_H264);
//    //
//    //    //初始化AVStream
//    //    AVStream *st = avformat_new_stream(fmtctx, codec);
//    //
//    //    //设置AVCodecContext编码参数
//    //    AVCodecContext* codecContext = avcodec_alloc_context3(NULL);
//    //    avcodec_get_context_defaults3(codecContext, codec);
//    //    codecContext->codec_id = AV_CODEC_ID_H264;
//    //    codecContext->codec_type = AVMEDIA_TYPE_VIDEO;
//    //    //    codecContext->width = capWidth;
//    //    //    codecContext->height = capHeight;
//    //    //    codecContext->time_base.den = frameRate;
//    //    codecContext->time_base.num = 1;
//    //    codecContext->pix_fmt = AV_PIX_FMT_YUV420P;   //PIX_FMT_YUV420P;
//    //    if(fmtctx->oformat->flags & AVFMT_GLOBALHEADER)
//    //        codecContext->flags|= AV_CODEC_FLAG_GLOBAL_HEADER;
//    //    av_opt_set(codecContext->priv_data, "preset", "slow", 0);
//    //    av_opt_set(codecContext->priv_data, "tune","zerolatency",0);
//    //    av_opt_set(codecContext->priv_data, "x264opts","crf=26:vbv-maxrate=728:vbv-bufsize=3640:keyint=25",0);
//    //
//    //    //打开编码器
//    //    ret = avcodec_open2(codecContext, codec, NULL);
//    //    printf("avcodec_open2 ret = %d", ret);
//    //
//    //    //写文件头
//    //    ret = avformat_write_header(fmtctx, NULL);
//    //    printf("avformat_write_header ret = %d", ret);
//    //
//    //    //打印SDP信息, 该信息可用于Rtp流接收解码
//    //    std::string path = "./test.sdp";
//    //    av_sdp_create(&fmtctx, 1, (char*)path.c_str(), (int)path.length());
//    
//    return 0;
//}
//
//
