// SDL.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "pch.h"
#include <iostream>
#define SDL_MAIN_HANDLED

extern "C"
{
#include <SDL.h>
#include <SDL_main.h>
}

const int bpp = 12;

int screen_w = 640, screen_h = 360;
const int pixel_w = 640, pixel_h = 360;

unsigned char buffer[pixel_w*pixel_h*bpp / 8];


//Event
//Refresh Event
#define REFRESH_EVENT (SDL_USEREVENT + 1)
#define BREAK_EVENT (SDL_USEREVENT + 2)

int thread_exit = 0;

int refresh_video(void *opaque)
{
	thread_exit = 0;
	while (thread_exit == 0)
	{
		SDL_Event event;
		event.type = REFRESH_EVENT;
		SDL_PushEvent(&event);
		SDL_Delay(40);
	}

	thread_exit = 0;
	//Break
	SDL_Event event;
	event.type = BREAK_EVENT;
	SDL_PushEvent(&event);
	return 0;
}

int main(int argc, char* argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO))
	{
		std::cout << "Could not initialize the SDL: " << SDL_GetError() << std::endl;
		return -1;
	}
	//创建一个显示窗口
	SDL_Window *screen = SDL_CreateWindow("Simple Video Player.", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		screen_w, screen_h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
	if (!screen) {
		std::cout << "SDL: could not create window, " << SDL_GetError() << std::endl;
		return -1;
	}
	//创建一个渲染器
	SDL_Renderer* sdlRenderer = SDL_CreateRenderer(screen, -1, 0);

	Uint32 pixformat = 0;
	//IYUV: Y + U + V  (3 planes)
	//YV12: Y + V + U  (3 planes)
	pixformat = SDL_PIXELFORMAT_IYUV;
	//创建纹理
	SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer, pixformat, SDL_TEXTUREACCESS_STREAMING, pixel_w, pixel_h);

	//打开yuv视频流文件
	FILE *fp = fopen("sintel_640_360.yuv", "rb+");
	if (fp == NULL) {
		std::cout << "Cannot open this file." << std::endl;
		return -1;
	}

	//SDLRect 设置数据显示的区域以及大小
	SDL_Rect sdlRect;

	SDL_Thread *refresh_thread = SDL_CreateThread(refresh_video, NULL, NULL);
	SDL_Event event;

	while (1) {
		//Wait
		SDL_WaitEvent(&event);
		if (event.type == REFRESH_EVENT)
		{
			if (fread(buffer, 1, pixel_w*pixel_h*bpp / 8, fp) != pixel_w * pixel_h*bpp / 8) {
				// Loop
				fseek(fp, 0, SEEK_SET);
				fread(buffer, 1, pixel_w*pixel_h*bpp / 8, fp);
			}
			//更新纹理区的数据
			SDL_UpdateTexture(sdlTexture, NULL, buffer, pixel_w);

			sdlRect.x = 0;
			sdlRect.y = 0;
			sdlRect.w = screen_w;
			sdlRect.h = screen_h;

			SDL_RenderClear(sdlRenderer);
			SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
			SDL_RenderPresent(sdlRenderer);
		}
		else if (event.type == SDL_WINDOWEVENT)
		{
			//If Resize
			SDL_GetWindowSize(screen, &screen_w, &screen_h);
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
	SDL_Quit();
	return 0;
}


