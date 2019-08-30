#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Snake
{
public:
	float x = 0.0f;
	float y = 0.0f;
	float xSpeed = 1.0f;
	float ySpeed = 0.0f;
	float blockSize = 10;
	float speed = 0.25f;
	int length = 2;
	struct coord {float x; float y;};
	std::vector<coord> trail;
	int score = 0;

public:
	void boundaryCollision(int32_t ScreenWidth, int32_t ScreenHeight)
	{
		if (x < 0){
			x = ScreenWidth - blockSize;
		} else if (x > ScreenWidth - blockSize){
			x = 0;
		} else if (y < 0) {
			y = ScreenHeight - blockSize;
		} else if (y > ScreenHeight - blockSize){
			y = 0;
		}

		for (int i = 0; i < trail.size(); i++){
			if (trail[i].x == x && trail[i].y == y){
				length = 2;
				score = 0;
			}
		} 
	}
	void shift(){
		coord new_c = {x, y};
		trail.push_back(new_c);
		if (trail.size() > length){
			trail.erase(trail.begin(), trail.begin() + (trail.size() - length));
		}
	}
};

class Wall
{
public:
	int walls[4] = { 0 };

public:
	void initWalls(){
		walls[0] = 0x0;
		walls[1] = 0x5;
		walls[2] = 0xD;
		walls[3] = 0xC;
	}
};

class Food
{
public:
	float x = 20.0f;
	float y = 20.0f;

public:
	void randomLoc() {
		x = rand() % 20 * 10;
		y = rand() % 20 * 10;
		std::cout << x << " " << y << std::endl;
	}

	bool collision(float snakeX, float snakeY) {
		if (x == snakeX && y == snakeY){
			randomLoc();
			return true;
		}
		return false;

	}
};

class SnakeMaze : public olc::PixelGameEngine
{
public:
	SnakeMaze()
	{
		sAppName = "Snake";
	}
	struct lightCoord { float x; float y; float fBlend; };
	Snake s;
private:
	float last_update;
	bool title;
public:
	bool OnUserCreate() override
	{
		title = true;
		last_update = 0.0f;
		s.trail.push_back({s.x, s.y});
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		olc::HWButton UP = GetKey(olc::Key::W);
		olc::HWButton DOWN = GetKey(olc::Key::S);
		olc::HWButton LEFT = GetKey(olc::Key::A);
		olc::HWButton RIGHT = GetKey(olc::Key::D);
		olc::HWButton QUIT = GetKey(olc::Key::Q);
		olc::HWButton RESET = GetKey(olc::Key::R);
		olc::HWButton ENTER = GetKey(olc::Key::ENTER);

		if (title){

			FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

			DrawString(60, 50, "SNAKE", olc::GREEN, 2);
			DrawString(20, 80, "Press Enter to start", olc::GREEN, 1);
			if (ENTER.bHeld) {
				title = False;
				last_update = 0;
			} else if (QUIT.bHeld) {
				exit(0);
			}

		} else {
			if (UP.bHeld && s.ySpeed != 1.0f){
				s.xSpeed = 0.0f;
				s.ySpeed = -1.0f;
			}
			else if (DOWN.bHeld && s.ySpeed != -1.0f){
				s.xSpeed = 0.0f;
				s.ySpeed = 1.0f;
			} 
			else if (LEFT.bHeld && s.xSpeed != 1.0f){
				s.xSpeed = -1.0f;
				s.ySpeed = 0.0f;
			}
			else if (RIGHT.bHeld && s.xSpeed != -1.0f){
				s.xSpeed = 1.0f;
				s.ySpeed = 0.0f;
			}
			else if (RESET.bHeld){
				title = true;
			}
			else if (QUIT.bHeld){
				exit(0);
			}

			if (last_update > s.speed) {
				s.x += s.blockSize * s.xSpeed;
				s.y += s.blockSize * s.ySpeed;

				// Clear
				FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::VERY_DARK_MAGENTA);

				// Draw
				for (int i = 0; i < s.trail.size(); i++) {
					FillRect(s.trail[i].x, s.trail[i].y, s.blockSize, s.blockSize, olc::GREEN);
				}

				// Collision
				s.boundaryCollision(ScreenWidth(), ScreenHeight());
				// std::cout << "X: " << s.x << " Y: " << s.y << " fEl: " << fElapsedTime << std::endl;

				s.shift();

				last_update -= s.speed;
			}
		}
		last_update += fElapsedTime;
		return true;
	}
};

int main()
{
	Snake s;
	SnakeMaze demo;
	if (demo.Construct(200, 200, 3, 3))
		demo.Start();
	return 0;
}