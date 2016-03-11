//============================================================================
// Name        : tssdl.cpp
// Author      : zaqc
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <unistd.h>
#include <stdlib.h>
#include <wchar.h>
#include <locale.h>

#include <sys/time.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
using namespace std;

static SDL_Window *window = NULL;
static SDL_GLContext gl_context;
static float r = 0.0f;

void render() {

	SDL_GL_MakeCurrent(window, gl_context);

	r = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);

	glClearColor(r, 0.4f, 0.1f, 1.0f);
	glClear( GL_COLOR_BUFFER_BIT);

	SDL_GL_SwapWindow(window);

} //render

bool quitting = false;
int SDLCALL watch(void *userdata, SDL_Event* event) {

	if (event->type == SDL_APP_WILLENTERBACKGROUND) {
		quitting = true;
	}

	return 1;
}

int main(int argc, char *argv[]) {
	SDL_Window *window;
	SDL_Renderer *renderer;
	SDL_Texture *texture;
	SDL_Event event;
	SDL_Rect r;

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
				"Couldn't initialize SDL: %s\n", SDL_GetError());
		return 3;
	}

	window = SDL_CreateWindow("SDL_CreateTexture",
	SDL_WINDOWPOS_UNDEFINED,
	SDL_WINDOWPOS_UNDEFINED, 1024, 768, SDL_WINDOW_RESIZABLE);

	r.x = 0;
	r.y = 0;
	r.w = 100;
	r.h = 50;

	renderer = SDL_CreateRenderer(window, -1, 0);

	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888,
			SDL_TEXTUREACCESS_TARGET, 1024, 768);

	TTF_Init();
	TTF_Font *font = TTF_OpenFont(
			"/usr/share/fonts/truetype/freefont/FreeSans.ttf", 32);
	SDL_Color blue = { 0, 255, 255 };

	int fc = 0;
	wchar_t fc_val[128];
	char fc_tst[256];
	memset(fc_val, 0, sizeof(fc_val));
	memcpy(fc_tst, "none\0", 5);
	timeval ts, ts_prev;
	gettimeofday(&ts, 0);
	ts_prev = ts;

	int sss = sizeof(wchar_t);
	printf("%i \n", sss);

	setlocale(LC_ALL, "ru_RU.utf8");

	while (1) {
		fc++;
		gettimeofday(&ts, 0);
		float delta = (ts.tv_sec * 1000000 + ts.tv_usec)
				- (ts_prev.tv_sec * 1000000 + ts_prev.tv_usec);
		if (delta >= 1000000.0f) {
			swprintf(fc_val, 128, L"всего %.2f кадров в секунду", (float) fc * 1000000.0 / delta);
			wcstombs(fc_tst, fc_val, wcslen(fc_val) * sizeof(Uint16));

			fc = 0;
			ts_prev = ts;
		}
		SDL_PollEvent(&event);
		if (event.type == SDL_QUIT)
			break;
		r.x++;
		r.x = r.x > 1024 ? 0 : r.x;
		r.y++;
		r.y = r.y > 768 ? 0 : r.y;

		SDL_SetRenderTarget(renderer, texture);
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(renderer);
		SDL_RenderDrawRect(renderer, &r);
		SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0x00);
		SDL_RenderFillRect(renderer, &r);

		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopy(renderer, texture, NULL, NULL);

		//SDL_Surface *text_surf = TTF_RenderUNICODE_Solid(font, fc_res,
		if (mblen(fc_tst, 128)) {
			SDL_Surface *text_surf = TTF_RenderUTF8_Solid(font, fc_tst, blue);

			if (text_surf) {
				SDL_Rect src_rect;
				src_rect.x = 0;
				src_rect.y = 0;
				src_rect.w = text_surf->w;
				src_rect.h = text_surf->h;
				SDL_Rect dst_rect;
				dst_rect.x = 100;
				dst_rect.y = 100;
				dst_rect.w = text_surf->w;
				dst_rect.h = text_surf->h;
				SDL_Texture *text_text = SDL_CreateTextureFromSurface(renderer,
						text_surf);
				SDL_RenderCopy(renderer, text_text, &src_rect, &dst_rect);
				SDL_DestroyTexture(text_text);
				SDL_FreeSurface(text_surf);
			}
		}

		SDL_RenderPresent(renderer);
	}
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	return 0;
}

//int main() {
//	SDL_Init(SDL_INIT_EVERYTHING);
//
//	window = SDL_CreateWindow("assd", SDL_WINDOWPOS_UNDEFINED,
//			SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
//
//	gl_context = SDL_GL_CreateContext(window);
//
//	SDL_Delay(1000);
//
//	while (!quitting) {
//
//		SDL_Event event;
//		while (SDL_PollEvent(&event)) {
//			if (event.type == SDL_QUIT) {
//				quitting = true;
//			}
//		}
//
//		render();
//		SDL_Delay(250);
//
//	}
//	SDL_DestroyWindow (window);
//
//	cout << "!!!Hello World!!!" << endl; // prints !!!Hello World!!!
//
//	SDL_Quit();
//	return 0;
//}
