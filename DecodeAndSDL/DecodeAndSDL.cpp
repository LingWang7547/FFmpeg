// DecodeAndSDL.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>

#define SDL_MAIN_HANDLED

//头文件
extern "C"
{
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "SDL.h"
#include "SDL_main.h"
}

//Refresh Event
#define REFRESH_EVENT (SDL_USEREVENT + 1)
#define BREAK_EVENT (SDL_USEREVENT + 2)
int thread_exit = 0;
int refresh_thread(void* opaque)
{
	thread_exit = 0;
	while (!thread_exit)
	{
		SDL_Event event;
		event.type = REFRESH_EVENT;
		SDL_PushEvent(&event);
		SDL_Delay(40);
	}
	thread_exit = 0;
	//break
	SDL_Event event;
	event.type = BREAK_EVENT;
	SDL_PushEvent(&event);

	return 0;
}


int main(int argc, char* argv[])
{
	//FFmpeg Variables
	AVFormatContext *pFormatCtx;
	AVCodecContext  *pCodecCtx;
	AVCodec         *pCodec;
	AVFrame         *pFrame, *pFrameYUV;
	AVPacket *packet;
	uint8_t *out_buffer;
	SwsContext *img_convert_ctx;
	int videoindex;
	int ret, got_picture;
    
	//SDL Variables
	int screen_w, screen_h;
	SDL_Window *screen;
	SDL_Renderer *sdlRenderer;
	SDL_Texture *sdlTexture;
	SDL_Rect sdlRect;
	SDL_Thread *video_tid;
	SDL_Event event;

	//file path
	//const char *filepath = "屌丝男士.mov";
	const char *filepath = argv[1];

	//------------FFmpeg Initialization----------
	//register
	av_register_all();
	avformat_network_init();
	pFormatCtx = avformat_alloc_context();
	//open input stream file
	if (avformat_open_input(&pFormatCtx, filepath, NULL, NULL) != 0)
	{
		std::cout << "Can't open input file." << std::endl;
		return -1;
	}
	//get stream information
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0)
	{
		std::cout << "Can't find stream information." << std::endl;
		return -1;
	}
	//find video index
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
	//find decoder and open it
	pCodecCtx = pFormatCtx->streams[videoindex]->codec;
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL)
	{
		std::cout << "Can't find a proper Codec." << std::endl;
		avformat_close_input(&pFormatCtx);
		return -1;
	}
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		std::cout << "Can't open the selected Codec." << std::endl;
		return -1;
	}
	//allocate memory for frames
	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();
	//image size
	out_buffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameYUV, out_buffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	
	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt,
		pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	
	//--------SDL Initialization----------
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER))
	{
		std::cout << "Can't initialize SDL with error: " << SDL_GetError() << std::endl;
		return -1;
	}
	//create window
	screen_w = pCodecCtx->width;
	screen_h = pCodecCtx->height;
	screen = SDL_CreateWindow("Simple FFmpeg Player", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screen_w, screen_h, SDL_WINDOW_OPENGL);
	if (!screen)
	{
		std::cout << "SDL: Can't create window with error: " << SDL_GetError() << std::endl;
		return -1;
	}
	// create renderer and texture;
	sdlRenderer = SDL_CreateRenderer(screen, -1, 0);
	//IYUV: Y + U + V (3 planes)
	//YV12: Y + V + U (3 planes)
	sdlTexture = SDL_CreateTexture(sdlRenderer, SDL_PIXELFORMAT_IYUV, SDL_TEXTUREACCESS_STREAMING, pCodecCtx->width, pCodecCtx->height);

	sdlRect.x = 0;
	sdlRect.y = 0;
	sdlRect.w = screen_w;
	sdlRect.h = screen_h;

	//decode packet
	packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	//video thread
	video_tid = SDL_CreateThread(refresh_thread, NULL, NULL);

	//Event Loop
	for (;;)
	{
		//Wait
		SDL_WaitEvent(&event);
		if (event.type == REFRESH_EVENT)
		{
			//read frame and decode
			if (av_read_frame(pFormatCtx, packet) >= 0)
			{
				if (packet->stream_index == videoindex)
				{
					ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, packet);
					if (ret < 0)
					{
						std::cout << "Decode Error." << std::endl;
						return -1;
					}
					if (got_picture)
					{
						//image convert
						sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0,
							pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
						//SDL 
						SDL_UpdateTexture(sdlTexture, NULL, pFrameYUV->data[0], pFrameYUV->linesize[0]);
						SDL_RenderClear(sdlRenderer);
						SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, NULL);
						SDL_RenderPresent(sdlRenderer);
					}
				}
				av_free_packet(packet);
			}
			else
			{
				//exit Thread
				thread_exit = 1;
			}
		}
		else if (event.type == SDL_QUIT)
		{
			thread_exit = 1;
		}
		else if (event.type == BREAK_EVENT)
		{
			break;
		}
	}

	sws_freeContext(img_convert_ctx);
	SDL_Quit();

	av_frame_free(&pFrame);
	av_frame_free(&pFrameYUV);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

	return 0;
}

