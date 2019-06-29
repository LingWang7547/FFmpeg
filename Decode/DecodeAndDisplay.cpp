/**
 * Simplest FFmpeg Decoder
 *
 * Decode an input video and display it with opencv
 *
 */
#include "pch.h"
#include <stdio.h>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>

extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
};


int main()
{
	AVFormatContext	*pFormatCtx;
	AVCodecContext	*pCodecCtx;
	AVCodec			*pCodec;
	AVFrame	*pFrame, *pFrameBGR;
	AVPacket *packet;
	uint8_t *out_buffer;
	int	videoindex;
	int y_size;
	int ret, got_picture;
	struct SwsContext *img_convert_ctx;
	int frame_cnt;

	//file path
	const char *filepath = "Titanic.ts";

	//注册所有文件格式和编解码库
	av_register_all();
	//打开网络视频流
	avformat_network_init();
	//对AVFormatContext进行初始化，或者用NULL进行初始化
	pFormatCtx = avformat_alloc_context();

	//打开输入数据流
	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0)
	{
		std::cout << "Can't open input stream." << std::endl;
		return -1;
	}
	//获取数据流信息
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		std::cout << "Can't find stram information." << std::endl;
		return -1;
	}
	//寻找输入流中的视频流部分
	videoindex = -1;
	for (int i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			videoindex = i;
			break;
		}
	}
	if (videoindex == -1)
	{
		std::cout << "Can't find a video stream." << std::endl;
		avformat_close_input(&pFormatCtx);
		return -1;
	}
	//寻找解码器
	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		std::cout << "Can't find Codec." << std::endl;
		avformat_close_input(&pFormatCtx);
		return -1;
	}
	//打开解码器
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		std::cout << "Can't open the selected Codec." << std::endl;
		return -1;
	}

	std::cout << "Time of this video: " << pFormatCtx->duration << " us." << std::endl;

	//分配空间保存帧数据
	pFrame = av_frame_alloc();
	pFrameBGR = av_frame_alloc();

	//设置图像大小
	int size = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
	out_buffer = (uint8_t *)av_malloc(size);
	avpicture_fill((AVPicture *)pFrameBGR, out_buffer, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	//输出详细信息-----------------------------
	std::cout << "--------------- File Information ----------------" << std::endl;
	av_dump_format(pFormatCtx, 0, filepath, 0);
	std::cout << "-------------------------------------------------" << std::endl;

	//格式转换
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

	frame_cnt = 0;
	//读帧
	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if (packet->stream_index == videoindex) {
			//解码
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			if (ret < 0) {
				std::cout << "Decode Error." << std::endl;
				return -1;
			}
			if (got_picture) {
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameBGR->data, pFrameBGR->linesize);

				std::cout << "Decode frame index: " << frame_cnt << std::endl;
				frame_cnt++;

				//显示图像
				cv::Mat pCvMat;
				if (pCvMat.empty())
					pCvMat.create(pCodecCtx->height, pCodecCtx->width, CV_8UC3);
				memcpy(pCvMat.data, out_buffer, size);
				cv::namedWindow("视频");
				cv::imshow("视频", pCvMat);
				cv::waitKey(40);
			}
		}
		av_free_packet(packet);
	}
	cv::destroyAllWindows();

	sws_freeContext(img_convert_ctx);

	av_frame_free(&pFrameBGR);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}

