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

	//ע�������ļ���ʽ�ͱ�����
	av_register_all();
	//��������Ƶ��
	avformat_network_init();
	//��AVFormatContext���г�ʼ����������NULL���г�ʼ��
	pFormatCtx = avformat_alloc_context();

	//������������
	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0)
	{
		std::cout << "Can't open input stream." << std::endl;
		return -1;
	}
	//��ȡ��������Ϣ
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		std::cout << "Can't find stram information." << std::endl;
		return -1;
	}
	//Ѱ���������е���Ƶ������
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
	//Ѱ�ҽ�����
	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		std::cout << "Can't find Codec." << std::endl;
		avformat_close_input(&pFormatCtx);
		return -1;
	}
	//�򿪽�����
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		std::cout << "Can't open the selected Codec." << std::endl;
		return -1;
	}

	std::cout << "Time of this video: " << pFormatCtx->duration << " us." << std::endl;

	//����ռ䱣��֡����
	pFrame = av_frame_alloc();
	pFrameBGR = av_frame_alloc();

	//����ͼ���С
	int size = avpicture_get_size(AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
	out_buffer = (uint8_t *)av_malloc(size);
	avpicture_fill((AVPicture *)pFrameBGR, out_buffer, AV_PIX_FMT_BGR24, pCodecCtx->width, pCodecCtx->height);
	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	//�����ϸ��Ϣ-----------------------------
	std::cout << "--------------- File Information ----------------" << std::endl;
	av_dump_format(pFormatCtx, 0, filepath, 0);
	std::cout << "-------------------------------------------------" << std::endl;

	//��ʽת��
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_BGR24, SWS_BICUBIC, NULL, NULL, NULL);

	frame_cnt = 0;
	//��֡
	while (av_read_frame(pFormatCtx, packet) >= 0) {
		if (packet->stream_index == videoindex) {
			//����
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

				//��ʾͼ��
				cv::Mat pCvMat;
				if (pCvMat.empty())
					pCvMat.create(pCodecCtx->height, pCodecCtx->width, CV_8UC3);
				memcpy(pCvMat.data, out_buffer, size);
				cv::namedWindow("��Ƶ");
				cv::imshow("��Ƶ", pCvMat);
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

