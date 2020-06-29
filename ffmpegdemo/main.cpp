//
//  main.cpp
//  ffmpegdemo
//
//  Created by 王晓鹏 on 2020/6/22.
//  Copyright © 2020 王晓鹏. All rights reserved.
//

#include <iostream>
#include <thread>
#include <netinet/in.h> //for sockaddr_in
#include <arpa/inet.h>  //for socket

#include "rtptcpaddress.h"
#include "rtpsession.h"
#include "rtpsessionparams.h"
#include "rtptcptransmitter.h"
#include "rtpipv4address.h"
#include "rtptimeutilities.h"
#include "rtppacket.h"
#include "rtpabortdescriptors.h"


extern "C" {
    #include "libavformat/avformat.h"
    #include "libavcodec/avcodec.h"
    #include "libavutil/opt.h"
    #include "libavutil/mathematics.h"
    #include "libswscale/swscale.h"
    #include "libswresample/swresample.h"
}


using namespace jrtplib;
using namespace std;


RTPSession session;

int sendRTP()
{
    avcodec_register_all();
    av_register_all();
    avformat_network_init();
    
    AVFormatContext *i_fmt_ctx;
    AVStream *i_video_stream;
    
    /* should set to NULL so that avformat_open_input() allocate a new one */
    i_fmt_ctx = NULL;
    char rtspUrl[] = "rtsp://admin:ad53937301@58.221.84.90:5540/h264/ch33/main/av_stream";
    
    AVDictionary* options = NULL;
    av_dict_set(&options, "rtsp_transport", "tcp", 0);
    
    if (avformat_open_input(&i_fmt_ctx, rtspUrl, NULL, &options)!=0)
    {
        fprintf(stderr, "could not open input file\n");
        return -1;
    }
    
    if (avformat_find_stream_info(i_fmt_ctx, NULL)<0)
    {
        fprintf(stderr, "could not find stream info\n");
        return -1;
    }
    
    av_dump_format(i_fmt_ctx, 0, rtspUrl, 0);
    
    /* find first video stream */
    for (unsigned i=0; i<i_fmt_ctx->nb_streams; i++)
    {
        if (i_fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            i_video_stream = i_fmt_ctx->streams[i];
            break;
        }
    }
    if (i_video_stream == NULL)
    {
        fprintf(stderr, "didn't find any video stream\n");
        return -1;
    }

    while (true)
    {
        AVPacket i_pkt;
        av_init_packet(&i_pkt);
        i_pkt.size = 0;
        i_pkt.data = NULL;
        if (av_read_frame(i_fmt_ctx, &i_pkt) <0 )
            break;
        
        //发送数据
        session.SendPacket(i_pkt.data,i_pkt.size);
    }
    
    avformat_close_input(&i_fmt_ctx);
    
    return 0;
}


void initRTPSession() {
    
    RTPSession session;
    RTPAbortDescriptors m_descriptors;
    
    RTPSessionParams sessionparams;
    sessionparams.SetAcceptOwnPackets(true);
    sessionparams.SetOwnTimestampUnit(1.0/10.0);
    
    m_descriptors.Init();
    
    RTPTCPTransmissionParams transparams;
    transparams.SetCreatedAbortDescriptors(&m_descriptors);
    int status = session.Create(sessionparams,&transparams,RTPTransmitter::TCPProto);
    
    if (status < 0)
    {
        return;
    }
    
    //初始化socket
    struct sockaddr_in platform_addr;
    memset(&platform_addr, 0, sizeof(platform_addr));
    platform_addr.sin_port = htons(2099);
    platform_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    platform_addr.sin_family = AF_INET; //设置为IP通信
    
    /*创建客户端套接字--IPv4协议，面向连接通信，TCP协议*/
    int socket_fd;
    if ((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("socket_fd error");
        return;
    }
    
    //连接服务器
    if (connect(socket_fd, (struct sockaddr *)&platform_addr, sizeof(platform_addr)) < 0)
    {
        perror("socket_fd connect error");
        return;
    }

    RTPTCPAddress addr(socket_fd);
    
    status = session.AddDestination(addr);
    if (status < 0)
    {
        return;
    }
    
    session.SetDefaultPayloadType(98);
    session.SetDefaultMark(false);
    session.SetDefaultTimestampIncrement(160);
    
//    //发送数据
//    session.SendPacket(str.data(),str.length());
//
//    RTPTime delay(0.020);
//    session.BYEDestroy(delay,"Client End",9);
}


int main(int argc, const char * argv[]) {

    initRTPSession();
    
    sendRTP();
    
    RTPTime delay(0.020);
    session.BYEDestroy(delay,"Client End",9);
    
    return 0;
}


