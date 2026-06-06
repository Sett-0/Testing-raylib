#include <ctime>
#include "raylib.h"
#include "raymath.h"


#define GRAVITY 1
#define DRAG 0.9f
#define WIDTH  600
#define HEIGHT 800
#define N 10

struct BallData {
	float R;
	Vector2 coord;
	Vector2 speed;
	Color color;
};

struct Ball {
	float R;
	Vector2 coord;
	Vector2 speed;
	Color color; 
	BallData _ball;
	
	Ball(float r, Vector2 &cd, Vector2 &sp, Color &clr) : R(r), coord(cd), speed(sp), color(clr) {
		_ball = BallData { r, cd, sp, clr };
	}
	
	Ball() = default;
	
	void resetBall() {
		this->R     = _ball.R;
		this->coord = _ball.coord;
		this->speed = _ball.speed;
		this->color = _ball.color;
	}
	
	void shiftColor() {
		unsigned char shiftVal = 15;
		this->color.r += shiftVal;
		this->color.g += shiftVal;
		this->color.b += shiftVal;
	}
};

int main() {
	InitWindow(WIDTH, HEIGHT, "raylib Balling");
	SetTargetFPS(60);
	SetRandomSeed(std::time(0));
	Ball balls[N]; 
	Color colors[N];
	for (size_t i = 0; i < N; i++) {
		float R  = GetRandomValue(10, 100);
		float x  = GetRandomValue(R, WIDTH - R);
		float y  = GetRandomValue(0, HEIGHT/2);
		float dx = GetRandomValue(0, 20);
		float dy = GetRandomValue(-200, 200) / 200.0f;
		unsigned char r = GetRandomValue(0, 255);
		unsigned char g = GetRandomValue(0, 255);
		unsigned char b = GetRandomValue(0, 255);
		Vector2 coord = { x,  y };
		Vector2 speed = { dx, dy };
		colors[i] = Color { r, g, b, 255 };
		balls[i]  = Ball(R, coord, speed, colors[i]);
	}

	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		for (Ball &ball : balls) {
			if (IsKeyPressed(KEY_R)) {
				for (Ball &ball : balls) {
					ball.resetBall();
				}
				continue;
			}
			ball.coord = Vector2Add(ball.coord, ball.speed);
			DrawCircleV(ball.coord, ball.R, ball.color);
			if (ball.coord.y + ball.R + ball.speed.y * DRAG < HEIGHT) {
				ball.speed.y += GRAVITY;
			}
			else if (ball.coord.y + ball.R >= HEIGHT && FloatEquals(ball.speed.y, 0))
			{
				ball.speed.y = 0;
				ball.coord.y = HEIGHT - ball.R;
			}
			else {
				ball.speed.y = -ball.speed.y * DRAG;
				//ball.shiftColor();
			}
			if (ball.coord.x - ball.R + ball.speed.x * DRAG < 0 || ball.coord.x + ball.R + ball.speed.x * DRAG > WIDTH)
				ball.speed.x = -ball.speed.x * DRAG;
			if (HEIGHT - ball.coord.y < ball.R) {
				ball.speed.x *= DRAG;
				//ball.shiftColor();
			}
		}
		
		DrawText(TextFormat("x:  %f", balls[0].coord.x), 20, 20,  30, BLACK);
		DrawText(TextFormat("y:  %f", balls[0].coord.y), 20, 50,  30, BLACK);
		DrawText(TextFormat("dx: %f", balls[0].speed.x), 20, 80,  30, BLACK);
		DrawText(TextFormat("dy: %f", balls[0].speed.y), 20, 110, 30, BLACK);
		DrawText(TextFormat("Color: %d %d %d", balls[0].color.r, balls[0].color.g, balls[0].color.b), 20, 140, 30, BLACK);
		EndDrawing();
	}
	CloseWindow();
	return 0;
}
