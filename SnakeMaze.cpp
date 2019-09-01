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
	void calculatePosition(){
		x += blockSize * xSpeed;
		y += blockSize * ySpeed;
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
		sAppName = "Snake Maze";
	}

	struct lightCoord { float x; float y; float fBlend; };

	int board[31][31] = { 0 };
	float gameSpeed = 0.20f;

	Snake snakeUser;
	Wall wall;
	Food fruit;

private:
	float last_update;
	bool title;
public:
	bool OnUserCreate() override
	{
		srand(time(NULL));

		title = true;
		last_update = 0.0f;
		snakeUser.trail.push_back({snakeUser.x, snakeUser.y});

		// Initialise walls
		wall.initWalls();
		for(int x = 0; x < ScreenWidth()/snakeUser.blockSize; x+= 2){
			for(int y = 0; y < ScreenHeight()/snakeUser.blockSize; y+= 2){
				int wallNo = rand() % 4;
				if (rand() % 5)
					wallNo = 0;

				unsigned int randBlock = wall.walls[wallNo];
				unsigned int bit_n = randBlock & 1;
				int x_scale = x ;
				int y_scale = y ;

				board[x_scale][y_scale] = bit_n;
				randBlock >>= 1;
				bit_n = randBlock & 1;
				board[x_scale][y_scale+1] = bit_n;
				randBlock >>= 1;
				bit_n = randBlock & 1;
				board[x_scale+1][y_scale] = bit_n;
				randBlock >>= 1;
				bit_n = randBlock & 1;
				board[x_scale+1][y_scale+1] = bit_n;
				randBlock >>= 1;
			}
		}

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

			DrawString(110, 50, "SNAKE", olc::GREEN, 2);
			DrawString(70, 80, "Press Enter to start", olc::GREEN, 1);
			DrawString(50, ScreenHeight() - 20, "Built for olcCodeJam 2019 \nusing olcPixelGameEngine", olc::GREEN, 1);
			if (ENTER.bHeld) {
				title = False;
				last_update = 0;
			} else if (QUIT.bHeld) {
				exit(0);
			}

		} else {
			if (UP.bHeld && snakeUser.ySpeed != 1.0f){
				snakeUser.xSpeed = 0.0f;
				snakeUser.ySpeed = -1.0f;
			}
			else if (DOWN.bHeld && snakeUser.ySpeed != -1.0f){
				snakeUser.xSpeed = 0.0f;
				snakeUser.ySpeed = 1.0f;
			} 
			else if (LEFT.bHeld && snakeUser.xSpeed != 1.0f){
				snakeUser.xSpeed = -1.0f;
				snakeUser.ySpeed = 0.0f;
			}
			else if (RIGHT.bHeld && snakeUser.xSpeed != -1.0f){
				snakeUser.xSpeed = 1.0f;
				snakeUser.ySpeed = 0.0f;
			}
			else if (RESET.bHeld){
				title = true;
			}
			else if (QUIT.bHeld){
				exit(0);
			}

			if (last_update > snakeUser.speed) {
				// std::cout << "X: " << snakeUser.x << " Y: " << snakeUser.y << " fEl: " << fElapsedTime << std::endl;

				// Calculations
				snakeUser.shift();
				snakeUser.calculatePosition();

				// Clear
				FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::VERY_DARK_MAGENTA);

				// Draw
				for (int i = 0; i < snakeUser.trail.size(); i++) {
					FillRect(snakeUser.trail[i].x, snakeUser.trail[i].y, snakeUser.blockSize, snakeUser.blockSize, olc::GREEN);
				}

				for (int x = 0; x < ScreenWidth() / snakeUser.blockSize + 1; x++) {
					for (int y = 0; y < ScreenHeight() / snakeUser.blockSize + 1; y++) {
						if (board[x][y] == 1) {
							FillRect(x*10,y*10, snakeUser.blockSize, snakeUser.blockSize, olc::BLUE);
						}
					}
				}

				FillRect(fruit.x, fruit.y, 10, 10, olc::RED);

				// Collision
				snakeUser.boundaryCollision(ScreenWidth(), ScreenHeight());

				if (board[int(snakeUser.x/snakeUser.blockSize)][int(snakeUser.y/snakeUser.blockSize)] == 1){
					std::cout << "HIT" << std::endl;
				}

				if (fruit.collision(snakeUser.x, snakeUser.y)){
					while (board[int(fruit.x/snakeUser.blockSize)][int(fruit.y/snakeUser.blockSize)] != 0)
						fruit.randomLoc();
					snakeUser.length++;
					snakeUser.score += 5;
				}

				last_update -= gameSpeed;
			}
		}
		DrawString(ScreenWidth() - 30, 5, std::to_string(snakeUser.score), olc::WHITE);
		last_update += fElapsedTime;
		return true;
	}
};

int main()
{
	Snake s;
	SnakeMaze demo;
	if (demo.Construct(300, 300, 3, 3))
		demo.Start();
	return 0;
}