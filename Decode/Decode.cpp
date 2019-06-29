/**
 * Simplest FFmpeg Decoder
 *
 * This software is a simplest video decoder based on FFmpeg.
 * Suitable for beginner of FFmpeg.
 *
 */
#include "pch.h"
#include <stdio.h>
#include <iostream>

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
	AVFrame	*pFrame, *pFrameYUV;
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
	pFrameYUV = av_frame_alloc();

	//设置图像大小
	out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	//输出详细信息-----------------------------
	std::cout << "--------------- File Information ----------------" << std::endl;
	av_dump_format(pFormatCtx, 0, filepath, 0);
	std::cout << "-------------------------------------------------" << std::endl;

	//格式转换
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	FILE *fp_264 = fopen("test264.h264", "wb+");
	FILE *fp_yuv = fopen("testyuv.yuv", "wb+");

	frame_cnt = 0;
	//读帧
	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if (packet->stream_index == videoindex) {
			
			//写入h.264的码流中
			fwrite(packet->data, 1, packet->size, fp_264);

			//解码
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
			if (ret < 0) {
				std::cout << "Decode Error." << std::endl;
				return -1;
			}
			if (got_picture) {
				sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height,
					pFrameYUV->data, pFrameYUV->linesize);

				std::cout << "Decode frame index: " << frame_cnt << std::endl;
				frame_cnt++;
				//写入yuv码流中
				fwrite(pFrameYUV->data[0], 1, pCodecCtx->height * pCodecCtx->coded_width, fp_yuv);
				fwrite(pFrameYUV->data[1], 1, pCodecCtx->height * pCodecCtx->coded_width / 4, fp_yuv);
				fwrite(pFrameYUV->data[2], 1, pCodecCtx->height * pCodecCtx->coded_width / 4, fp_yuv);
			}
		}
		av_free_packet(packet);
	}

	fclose(fp_264);
	fclose(fp_yuv);

	sws_freeContext(img_convert_ctx);

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}

