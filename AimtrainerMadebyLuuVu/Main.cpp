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

int targetSize = 30;
int targetLifetime = 500;
int timeplay = 15000;
bool draggingSize = false, draggingLifetime = false, draggingDuration = false;
int highScore = 0;

double mapValue(int value, int inMin, int inMax, int outMin, int outMax) {
	return outMin + (double)(value - inMin) / (inMax - inMin) * (outMax - outMin);
}

void drawSlider(SDL_Renderer* renderer, int x, int y, int width, int height, double value, double minVal, double maxVal) {
	SDL_SetRenderDrawColor(renderer, 100, 100, 100, 255);
	SDL_Rect bar = { x,y,width,height };
	SDL_RenderFillRect(renderer, &bar);

	double normalizedValue = (value - minVal) / (maxVal - minVal);
	int sliderX = x + static_cast<int>(normalizedValue * (width-10));
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_Rect slider = { sliderX,y-5,10,height +10 };
	SDL_RenderFillRect(renderer, &slider);
}
void renderText(SDL_Renderer* renderer, TTF_Font* font, const string& text, int x, int y) {
	SDL_Color textColor = { 255,255,255,255 };
	SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
	SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
	SDL_Rect textRect = { x,y,textSurface->w,textSurface->h };
	SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
	SDL_FreeSurface(textSurface);
	SDL_DestroyTexture(textTexture);
}
void drawTarget(SDL_Renderer* renderer, Circle target) {
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	for (int w = 0; w < target.radius * 2; w++) {
		for (int h = 0; h < target.radius * 2; h++) {
			int dx = target.radius - w;
			int dy = target.radius - h;
			if ((dx * dx + dy * dy) <= (target.radius * target.radius)) {
				SDL_RenderDrawPoint(renderer, target.x + dx, target.y + dy);
			}
		}
	}
}
int main(int argc, char* argv[]) {
	SDL_Init(SDL_INIT_VIDEO);
	TTF_Init();

	SDL_Window* window = SDL_CreateWindow("Aim Trainer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	TTF_Font* font = TTF_OpenFont("arial.ttf", 24);
	if (!font) {
		cout << "FONT FAIL" << '\n';
		return -1;
	}

	srand(time(0));
	bool running = true;

	while (running) {
		bool inMenu = true;
		SDL_Event event;
		while (inMenu) {
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
			renderText(renderer, font, "Aim Trainer", SCREEN_WIDTH / 2 - 50, 50);
			renderText(renderer, font, "Play", SCREEN_WIDTH / 2 - 18, 100 );
			renderText(renderer, font, "Quit", SCREEN_WIDTH / 2 - 18, 150);
			renderText(renderer, font, "High Score: " + to_string(highScore), SCREEN_WIDTH / 2 - 55, 200);
			renderText(renderer, font, "Target Size: " + to_string(targetSize), 100, 400);
			drawSlider(renderer, 350, 410, 250, 10, targetSize, 10, 100);
			renderText(renderer, font, "Target Lifetime: " + to_string(targetLifetime) + "ms", 100, 450);
			drawSlider(renderer, 350, 460, 250, 10, targetLifetime, 500, 5000);
			renderText(renderer, font, "Time to Play: " + to_string(timeplay / 1000) + "s", 100, 500);
			drawSlider(renderer, 350, 510, 250, 10, timeplay/1000, 10, 60);

			SDL_RenderPresent(renderer);


			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					inMenu = false;
					running = false;
				}
				else if (event.type == SDL_MOUSEBUTTONDOWN) {
					int mouseX, mouseY;
					SDL_GetMouseState(&mouseX, &mouseY);
					if (mouseX >= SCREEN_WIDTH / 2 - 18 && mouseX <= SCREEN_WIDTH / 2 + 18) {
						if (mouseY >= 100 && mouseY <= 120) {
							inMenu = false;
						}
						else if (mouseY >= 150 && mouseY <= 220) {
							inMenu = false;
							running = false;
						}
					}
					else if (mouseX >= 350 && mouseX <= 600) {
						if (mouseY >= 400 && mouseY <= 430) {
							draggingSize = true;
						}
						else if (mouseY >= 450 && mouseY <=480) {
							draggingLifetime = true;
						}
						else if (mouseY >= 500 && mouseY <= 530) {
							draggingDuration = true;
						}
					}
				}
				else if (event.type == SDL_MOUSEBUTTONUP) {
						draggingSize = false;
						draggingLifetime = false;
						draggingDuration = false;
				}
				else if (event.type == SDL_MOUSEMOTION && (event.motion.state & SDL_BUTTON_LMASK)) {
					int mouseX = event.motion.x;
					if(mouseX >= 350 && mouseX <= 600){
						if (draggingSize) {
							targetSize = static_cast<int>(mapValue(mouseX, 350, 600, 10, 100));
						}
						if (draggingLifetime) {
							targetLifetime = static_cast<int>(mapValue(mouseX, 350, 600, 500, 5000));
						}
						if (draggingDuration) {
							timeplay = static_cast<int>(mapValue(mouseX, 350, 600, 10, 60)) * 1000;
						}
					}
				}
			}
		}
		if (!running) break;

		Circle target = { rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT,targetSize };
		int score = 0;
		Uint32 startTime = SDL_GetTicks();
		Uint32 targetStartTime = SDL_GetTicks();

		bool gameRunning = true;
		while (gameRunning) {
			Uint32 elapsedTime = SDL_GetTicks() - startTime;
			if (elapsedTime >= timeplay) {
				gameRunning = false;
				continue;
			}
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_QUIT) {
					gameRunning = false;
					running = false;
				}
				else if (event.type == SDL_MOUSEBUTTONDOWN) {
					int mouseX, mouseY;
					SDL_GetMouseState(&mouseX, &mouseY);
					if (isInside(mouseX, mouseY, target)) {
						score++;
						target.x = rand() % SCREEN_WIDTH;
						target.y = rand() % SCREEN_HEIGHT;
						targetStartTime = SDL_GetTicks();
					}
				}
			}
			if (SDL_GetTicks() - targetStartTime >= targetLifetime) {
				target.x = rand() % SCREEN_WIDTH;
				target.y = rand() % SCREEN_HEIGHT;
				targetStartTime = SDL_GetTicks();
			}
			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderClear(renderer);
			drawTarget(renderer, target);
			renderText(renderer, font, "Time:" + to_string((timeplay - elapsedTime) / 1000), 10, 10);
			renderText(renderer, font, "Score:" + to_string(score), 10, 40);
			SDL_RenderPresent(renderer);
		}
		if (score > highScore) {
			highScore = score;
		}
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(font);
	TTF_Quit();
	SDL_Quit();
	return 0;
}
