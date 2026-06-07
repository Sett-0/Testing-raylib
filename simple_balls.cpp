#include <ctime>
#include <vector>
#include "raylib.h"
#include "raymath.h"

#define GRAVITY 1
#define DRAG 0.9f
#define WIDTH  800
#define HEIGHT 800
#define GRIDSIZE_X 4
#define GRIDSIZE_Y 4

struct BallInitData {
	float R;
	Vector2 coord;
	Vector2 speed;
	Color color;
	Rectangle collider;
};

class Ball {
public:
	float R;
	Vector2 coord;
	Vector2 speed;
	Color color; 

	Ball(float r, Vector2 cd, Vector2 sp, Color clr) : R(r), coord(cd), speed(sp), color(clr) {
		collider = { cd.x - R, cd.y - R, cd.x + R, cd.y + R };
		ball = BallInitData { r, cd, sp, clr };
	}
	
	Ball() = default;
	
	void resetBall() {
		this->R     = ball.R;
		this->coord = ball.coord;
		this->speed = ball.speed;
		this->color = ball.color;
	}
	
	void shiftColor() {
		unsigned char shiftVal = 15;
		this->color.r += shiftVal;
		this->color.g += shiftVal;
		this->color.b += shiftVal;
	}

private:
	Rectangle collider;
	BallInitData ball;
	
};

Ball makeRandomBall() {
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
	Color color = Color { r, g, b, 255 };
	return Ball(R, coord, speed, color);
}

std::vector<Ball> makeRandomBalls(const int N) {
	std::vector<Ball> balls(N);
	for (size_t i = 0; i < balls.size(); i++) {
		balls[i] = makeRandomBall();
	}
	return balls;
}

void clearGrid(std::vector<Ball*> (&grid)[GRIDSIZE_X][GRIDSIZE_Y]) {
	for (size_t x = 0; x < GRIDSIZE_X; x++)
		for (size_t y = 0; y < GRIDSIZE_Y; y++)
			grid[x][y].clear();
}

void populateGrid(std::vector<Ball*> (&grid)[GRIDSIZE_X][GRIDSIZE_Y], std::vector<Ball> &balls) {
	for (Ball &ball : balls) {
		int cellX = ball.coord.x / (WIDTH  / GRIDSIZE_X);
		int cellY = ball.coord.y / (HEIGHT / GRIDSIZE_Y);
		grid[cellX][cellY].push_back(&ball);
		
		// Checking if the ball touches neighboring cells
		int lCellX = (ball.coord.x - ball.R) / (WIDTH  / GRIDSIZE_X);
		int rCellX = (ball.coord.x + ball.R) / (WIDTH  / GRIDSIZE_X);
		int tCellY = (ball.coord.y - ball.R) / (HEIGHT / GRIDSIZE_Y);
		int bCellY = (ball.coord.y + ball.R) / (HEIGHT / GRIDSIZE_Y);
		// Top-left
		if (lCellX > 0 && tCellY > 0 && lCellX < cellX && tCellY < cellY) { grid[lCellX][tCellY].push_back(&ball); }
		// Top
		if (tCellY > 0 && tCellY < cellY) { grid[cellX][tCellY].push_back(&ball); }
		// Top-right
		if (rCellX < GRIDSIZE_X && tCellY > 0 && rCellX > cellX && tCellY < cellY) { grid[rCellX][tCellY].push_back(&ball); }
		// Right
		if (rCellX < GRIDSIZE_X && rCellX > cellX) { grid[rCellX][cellY].push_back(&ball); }
		// Bottom-right
		if (rCellX < GRIDSIZE_X && bCellY < GRIDSIZE_Y && rCellX > cellX && bCellY > cellY) { grid[rCellX][bCellY].push_back(&ball); }
		// Bottom
		if (rCellX < GRIDSIZE_X && bCellY > cellY) { grid[cellX][bCellY].push_back(&ball); }
		// Bottom-left
		if (lCellX > 0 && bCellY < GRIDSIZE_Y && lCellX < cellX && bCellY > cellY) { grid[lCellX][bCellY].push_back(&ball); }
		// Left
		if (lCellX > 0 && lCellX < cellX) { grid[lCellX][cellY].push_back(&ball); }
	}
}

void onCollision(float &coord, float &speed) {
	speed  = -speed;
	coord +=  speed;
	speed  =  speed * DRAG;
}

void onCollision(Vector2 &coord, Vector2 &speed) {
	speed = Vector2Negate(speed);
	coord = Vector2Add(coord, speed);
	speed = Vector2Scale(speed, DRAG);
}

void resolveCollisions(std::vector<Ball*> (&grid)[GRIDSIZE_X][GRIDSIZE_Y]) {
	// Window bounds
	Vector2 lBound[] = { { 0,     0      }, { 0,     HEIGHT } };
	Vector2 rBound[] = { { WIDTH, 0      }, { WIDTH, HEIGHT } };
	Vector2 tBound[] = { { 0,     0      }, { WIDTH, 0      } };
	Vector2 bBound[] = { { 0,     HEIGHT }, { WIDTH, HEIGHT } };			
	
	for (size_t x = 0; x < GRIDSIZE_X; x++) {
		for (size_t y = 0; y < GRIDSIZE_Y; y++) {
			auto &bucket = grid[x][y];
			for (size_t i = 0; i < bucket.size(); i++) {
				// Collision type: Ball : lBound
				if (x == 0) {
					if (CheckCollisionCircleLine(bucket[i]->coord, bucket[i]->R, lBound[0], lBound[1])) {
						onCollision(bucket[i]->coord.x, bucket[i]->speed.x);
					}
				}
				
				// Collision type: Ball : rBound
				if (x == GRIDSIZE_X-1) {
					if (CheckCollisionCircleLine(bucket[i]->coord, bucket[i]->R, rBound[0], rBound[1])) {
						onCollision(bucket[i]->coord.x, bucket[i]->speed.x);
					}
				}
				
				// Collision type: Ball : tBound
				if (y == 0) {
					if (CheckCollisionCircleLine(bucket[i]->coord, bucket[i]->R, tBound[0], tBound[1])) {
						onCollision(bucket[i]->coord.y, bucket[i]->speed.y);
					}
				}
				
				// Collision type: Ball : bBound
				if (y == GRIDSIZE_Y-1) {
					if (CheckCollisionCircleLine(bucket[i]->coord, bucket[i]->R, bBound[0], bBound[1])) {
						onCollision(bucket[i]->coord.y, bucket[i]->speed.y);
					}
				}
				
				if (bucket.size() > 1) {
					for (size_t j = i + 1; j < bucket.size(); j++) {
						// Collision type: Ball : Ball
						if (bucket[i] == bucket[j]) continue;
						if (CheckCollisionCircles(bucket[i]->coord, bucket[i]->R, bucket[j]->coord, bucket[j]->R)) {
							onCollision(bucket[i]->coord, bucket[i]->speed);
							onCollision(bucket[j]->coord, bucket[j]->speed);
						}
					}
				}
			}
		}
	}
}

int main() {
	InitWindow(WIDTH, HEIGHT, "raylib Balling");
	SetTargetFPS(60);
	SetRandomSeed(std::time(0));
	std::vector<Ball*> grid[GRIDSIZE_X][GRIDSIZE_Y];
	
	//auto balls = makeRandomBalls(10);
	Ball ball1 = Ball(100, { WIDTH/2 - 300, HEIGHT/2 }, { 0, 0 }, RED);
	Ball ball2 = Ball(100, { WIDTH/2 + 300, 0        }, { 0, 0 }, BLUE);
	std::vector<Ball> balls = { ball1, ball2 };
	
	while (!WindowShouldClose()) {
		BeginDrawing();
		ClearBackground(RAYWHITE);
		
		// Drawing the collison grid
		for (size_t x = 1; x < GRIDSIZE_X; x++) {
			for (size_t y = 1; y < GRIDSIZE_Y; y++) {
				Vector2 startPosRow = { 0,     HEIGHT / GRIDSIZE_Y * (float)y };
				Vector2 endPosRow   = { WIDTH, HEIGHT / GRIDSIZE_Y * (float)y };
				Vector2 startPosCol = { WIDTH / GRIDSIZE_X * (float)x, 0      };
				Vector2 endPosCol   = { WIDTH / GRIDSIZE_X * (float)x, HEIGHT };
				DrawLineDashed(startPosRow, endPosRow, 5, 5, LIGHTGRAY);
				DrawLineDashed(startPosCol, endPosCol, 5, 5, LIGHTGRAY);
			}
		}
		
		for (Ball &ball : balls) {
			// Reset balls option
			if (IsKeyPressed(KEY_R)) {
				for (Ball &ball : balls) {
					ball.resetBall();
				}
				continue;
			}
			
			// Updating ball
			ball.speed.y += GRAVITY;
			ball.coord = Vector2Add(ball.coord, ball.speed);
			
			// Collision handling
			clearGrid(grid);
			populateGrid(grid, balls);
			resolveCollisions(grid);
			
			// Drawing ball
			DrawCircleV(ball.coord, ball.R, ball.color);
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
