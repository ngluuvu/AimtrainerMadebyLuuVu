#include<SDL.h>
#include<iostream>
#include<cstdlib>
#include<ctime>
#include<SDL_ttf.h>
#include<string>
using namespace std;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
struct Circle {
	int x, y, radius;
};
bool isInside(int x, int t, Circle target) {
	int dx = x - target.x;
	int dy = t - target.y;
	return (dx * dx + dy * dy) <= (target.radius * target.radius);
}
#pragma comment(lib, "SDL2.lib")
#pragma comment(lib, "SDL2main.lib")
int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();
	SDL_Window* window = SDL_CreateWindow("Aim Trainer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	Uint32 startTime = SDL_GetTicks();
	const Uint32 GAME_DURATION = 10000;
	Uint32 targetStartTime = SDL_GetTicks();
	const Uint32 TARGET_LIFETIME = 500;
	srand(time(0));
	Circle target = { rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT,30 + rand() % 20 };
	int score = 0;
	bool running = true;
	SDL_Event event;
	while (running) {
		Uint32 elapsedTime = SDL_GetTicks() - startTime;
		if (elapsedTime >= GAME_DURATION) {
			cout << "Final Score: " << score << '\n';
			running = false;
			break;
		}
		if (SDL_GetTicks() - targetStartTime >= TARGET_LIFETIME) {
			target.x = rand() % SCREEN_WIDTH;
			target.y = rand() % SCREEN_HEIGHT;
			target.radius = 30 + rand() % 20;
			targetStartTime = SDL_GetTicks();
		}
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = false;
			}
			else if (event.type == SDL_MOUSEBUTTONDOWN) {
				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);

				if (isInside(mouseX, mouseY, target)) {
					score++;
					target.x = rand() % SCREEN_WIDTH;;
					target.y = rand() % SCREEN_HEIGHT;
					target.radius = 30 + rand() % 20;
					targetStartTime = SDL_GetTicks();
					cout << "Score: " << score << '\n';
				}
			}
		}
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);
		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
		for (int w = -target.radius; w <= target.radius; w++) {
			for (int h = -target.radius; h <= target.radius; h++) {
				if (w * w + h * h <= target.radius * target.radius) {
					SDL_RenderDrawPoint(renderer, target.x + w, target.y + h);
				}
			}
		}
		SDL_RenderPresent(renderer);
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
	if (!font) {
		cout << "FONT FAIL" << '\n';
		return -1;
	}
	else {
		SDL_Color textColor = { 255,255,255,255 };
		string scoreText = "Final Score: " + to_string(score);
		SDL_Surface* textSurface = TTF_RenderText_Solid(font, scoreText.c_str(), textColor);
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		SDL_Rect textRect = { SCREEN_WIDTH / 2 - 20, SCREEN_HEIGHT / 2 - 20, textSurface->w,textSurface->h};

		SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
		SDL_RenderPresent(renderer);

		SDL_Delay(10000);

		SDL_FreeSurface(textSurface);
		SDL_DestroyTexture(textTexture);
		TTF_CloseFont(font);
	}
	TTF_Quit(); 
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}