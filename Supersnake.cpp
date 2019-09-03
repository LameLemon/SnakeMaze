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
	int direction = 3;
	int lives = 3;

public:
	bool reset(){
		if (lives > 1) {
			lives--;
			x = y = 0.0f;
			xSpeed = 1.0f;
			ySpeed = 0.0f;		
			return true;
		} 
		lives = 3;
		length = 2;
		score = 0;
		x = y = 0.0f;
		xSpeed = 1.0f;
		ySpeed = 0.0f;
		direction = 3;
		trail.push_back({x, y});
		return false;
	}
	bool boundaryCollision(int32_t ScreenWidth, int32_t ScreenHeight)
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
				return false;
			}
		}
		return true; 
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
		if (xSpeed > 0)
			direction = 3;
		else if (xSpeed < 0)
			direction = 2;
		else if (ySpeed > 0)
			direction = 1;
		else
			direction = 0;
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

class Bullet
{
public:
	float x = -10.0f;
	float y = -10.0f;
	int direction = 0; // up 0, down 1, left 2, right 3
	float coolDown = 0.0f;
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
		// std::cout << x << " " << y << std::endl;
	}

	bool collision(float snakeX, float snakeY) {
		if (x == snakeX && y == snakeY)
			return true;
		return false;

	}
};

class Supersnake : public olc::PixelGameEngine
{
public:
	Supersnake()
	{
		sAppName = "Supersnake";
	}

	struct lightCoord { float x; float y; float fBlend; };

	int board[31][31] = { 0 };
	float gameSpeed = 0.15f;

	Snake snakeUser;
	Wall wall;
	Food fruit;
	Bullet bullet;

private:
	float last_update;
	bool title;
	int level = 0;
	int wall_chance = 8;
public:

	void reset(){
		
		last_update = 0.0f;
		if(!snakeUser.reset()){
			level = 0;
			newLevel();	
		}
	}

	void newLevel(){
		srand(level + 1);
		std::cout << "Level " << level + 1 << std::endl;
		last_update = 0.0f;

		snakeUser.x = snakeUser.y = 0.0f;
		snakeUser.direction = 3;
		snakeUser.xSpeed = 1.0f;
		snakeUser.ySpeed = 0.0f;
		snakeUser.trail.push_back({snakeUser.x, snakeUser.y});

		fruit.randomLoc();
		while (board[int(fruit.x/snakeUser.blockSize)][int(fruit.y/snakeUser.blockSize)] != 0)
			fruit.randomLoc();
		
		// Initialise walls
		for(int x = 0; x < ScreenWidth()/snakeUser.blockSize; x+= 2){
			for(int y = 0; y < ScreenHeight()/snakeUser.blockSize; y+= 2){
				int wallNo;
				if (y == 0)
					wallNo = 0;
				else {
					wallNo = rand() % 4;
					if (rand() % wall_chance)
						wallNo = 0;
				}
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

	}
	bool OnUserCreate() override
	{
		title = true;
		wall.initWalls();
		newLevel();
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		olc::HWButton UP = GetKey(olc::Key::W);
		olc::HWButton DOWN = GetKey(olc::Key::S);
		olc::HWButton LEFT = GetKey(olc::Key::A);
		olc::HWButton RIGHT = GetKey(olc::Key::D);
		olc::HWButton FIRE = GetKey(olc::Key::SPACE);
		olc::HWButton QUIT = GetKey(olc::Key::Q);
		olc::HWButton PAUSE = GetKey(olc::Key::P);
		olc::HWButton ENTER = GetKey(olc::Key::ENTER);

		if (title){

			FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

			DrawString(110, 50, "SNAKE", olc::GREEN, 2);
			DrawString(70, 80, "Press Enter to start", olc::GREEN, 1);
			DrawString(50, ScreenHeight() - 20, "Built for olcCodeJam 2019 \nusing olcPixelGameEngine", olc::GREEN, 1);
			if (ENTER.bHeld) {
				title = false;
				last_update = 0;
			} else if (QUIT.bHeld) {
				exit(0);
			}

		} else {
			if (UP.bHeld && snakeUser.direction != 1){
				snakeUser.xSpeed = 0.0f;
				snakeUser.ySpeed = -1.0f;
			}
			else if (DOWN.bHeld && snakeUser.direction != 0){
				snakeUser.xSpeed = 0.0f;
				snakeUser.ySpeed = 1.0f;
			} 
			else if (LEFT.bHeld && snakeUser.direction != 3){
				snakeUser.xSpeed = -1.0f;
				snakeUser.ySpeed = 0.0f;
			}
			else if (RIGHT.bHeld && snakeUser.direction != 2){
				snakeUser.xSpeed = 1.0f;
				snakeUser.ySpeed = 0.0f;
			}
			else if (PAUSE.bHeld){
				title = true;
			}
			else if (QUIT.bHeld){
				exit(0);
			}

			if (FIRE.bHeld && (bullet.y <= 0 || bullet.y >= ScreenHeight()) && (bullet.coolDown > 2.0f)){
				bullet.x = snakeUser.x;
				bullet.y = snakeUser.y;
				if (snakeUser.xSpeed > 0)
					bullet.direction = 3;
				else if (snakeUser.xSpeed < 0)
					bullet.direction = 2;
				else if (snakeUser.ySpeed > 0)
					bullet.direction = 1;
				else
					bullet.direction = 0;
				bullet.coolDown = 0.0f;
			}

			if (last_update > gameSpeed) {
				// std::cout << "X: " << snakeUser.x << " Y: " << snakeUser.y << " fEl: " << fElapsedTime << std::endl;

				// Calculations
				lightCoord lightCoordStruct [13] = { {snakeUser.x-20, snakeUser.y, 0.5f}, 
									{snakeUser.x-10, snakeUser.y-10, 0.5f}, {snakeUser.x-10, snakeUser.y, 1.0f}, {snakeUser.x-10, snakeUser.y+10, 0.5f}, 
									{snakeUser.x, snakeUser.y-20, 0.5f}, {snakeUser.x, snakeUser.y-10, 1.0f}, {snakeUser.x, snakeUser.y, 1.0f}, {snakeUser.x, snakeUser.y+10, 1.0f}, {snakeUser.x, snakeUser.y+20, 0.5f}, 
									{snakeUser.x+10, snakeUser.y-10, 0.5f}, {snakeUser.x+10, snakeUser.y, 1.0f}, {snakeUser.x+10, snakeUser.y+10, 0.5f},
									{snakeUser.x+20, snakeUser.y, 0.5f} };

				snakeUser.shift();
				snakeUser.calculatePosition();

				if (bullet.x <= ScreenWidth() && bullet.x >= 0 && bullet.y <= ScreenHeight() && bullet.y >= 0){
					if (bullet.direction == 0)
						bullet.y += snakeUser.blockSize * -2.0f;
					else if (bullet.direction == 1)
						bullet.y += snakeUser.blockSize * 2.0f;
					else if (bullet.direction == 2)
						bullet.x += snakeUser.blockSize * -2.0f;
					else if (bullet.direction == 3)
						bullet.x += snakeUser.blockSize * 2.0f;
				} else {
					bullet.x = -10.0f;
					bullet.y = -10.0f;
				}

				// Clear
				FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::VERY_DARK_MAGENTA);

				// Draw
				for (int i = 0; i < snakeUser.trail.size(); i++) {
					FillRect(snakeUser.trail[i].x, snakeUser.trail[i].y, snakeUser.blockSize, snakeUser.blockSize, olc::GREEN);
				}

				if (bullet.y != -10.0f) {
					FillRect(bullet.x, bullet.y, snakeUser.blockSize, snakeUser.blockSize, olc::RED);
					FillRect(bullet.x + 2, bullet.y + 2, 6, 6, olc::YELLOW);
				}
				
				for (int x = 0; x < ScreenWidth() / snakeUser.blockSize; x++) {
					for (int y = 0; y < ScreenHeight() / snakeUser.blockSize; y++) {
						if (board[x][y] == 1) {
							FillRect(x*10,y*10, snakeUser.blockSize, snakeUser.blockSize, olc::BLUE);
						}
					}
				}

				FillRect(fruit.x, fruit.y, 10, 10, olc::RED);

				int current_light_coord = 0;				
				SetPixelMode(olc::Pixel::ALPHA);
				for(int x = -20; x < ScreenWidth()+20; x += snakeUser.blockSize){
					for (int y = -20; y < ScreenHeight()+20; y += snakeUser.blockSize){
						if (x == lightCoordStruct[current_light_coord].x && y == lightCoordStruct[current_light_coord].y){
							if(lightCoordStruct[current_light_coord].fBlend == 1){
								current_light_coord++;
							} else {
								SetPixelBlend(lightCoordStruct[current_light_coord].fBlend);
								FillRect(lightCoordStruct[current_light_coord].x, lightCoordStruct[current_light_coord].y, snakeUser.blockSize, snakeUser.blockSize, olc::BLACK);
								current_light_coord++;
							}
						} else {
							SetPixelBlend(0.80f);
							FillRect(x, y, snakeUser.blockSize, snakeUser.blockSize, olc::BLACK);
						}
					}
				}
				SetPixelMode(olc::Pixel::NORMAL);
				SetPixelBlend(1.0f);

				// Collision
				float nextX, nextY, prevX, prevY;
				if (bullet.direction == 0){
					nextX = prevX = bullet.x;
					nextY = bullet.y - 10.0f;
					prevY = bullet.y + 10.0f;
				} else if (bullet.direction == 1){
					nextX = prevX = bullet.x;
					nextY = bullet.y + 10.0f;
					prevY = bullet.y - 10.0f; 
				} else if (bullet.direction == 2){
					nextX = bullet.x - 10.0f;
					nextY = prevY= bullet.y;
					prevY = bullet.x + 10.0f;
				} else if (bullet.direction == 3){
					nextX = bullet.x + 10.0f;
					nextY = prevY= bullet.y;
					prevX = bullet.x - 10.0f;
				}
				if (board[int(prevX/snakeUser.blockSize)][int(prevY/snakeUser.blockSize)] == 1){
					board[int(prevX/snakeUser.blockSize)][int(prevY/snakeUser.blockSize)] = 0;
					FillRect(prevX, prevY, snakeUser.blockSize, snakeUser.blockSize, olc::MAGENTA);
					bullet.x = bullet.y = -10.0f;
				} else if (board[int(bullet.x/snakeUser.blockSize)][int(bullet.y/snakeUser.blockSize)] == 1) {
					board[int(bullet.x/snakeUser.blockSize)][int(bullet.y/snakeUser.blockSize)] = 0;
					FillRect(bullet.x, bullet.y, snakeUser.blockSize, snakeUser.blockSize, olc::MAGENTA);
					bullet.x = bullet.y = -10.0f;
				}

				if (!(snakeUser.boundaryCollision(ScreenWidth(), ScreenHeight()))){
					reset();
					//dead
				}

				if (board[int(snakeUser.x/snakeUser.blockSize)][int(snakeUser.y/snakeUser.blockSize)] == 1){
					reset();
					//dead
				}

				if (fruit.collision(snakeUser.x, snakeUser.y)){
					fruit.randomLoc();
					while (board[int(fruit.x/snakeUser.blockSize)][int(fruit.y/snakeUser.blockSize)] != 0)
						fruit.randomLoc();
					snakeUser.length++;
					snakeUser.score += 5;

					float level_fl = snakeUser.score / 20;
					if (floor(level_fl) == level_fl && level != level_fl){
						level = level_fl;
						newLevel();
					}
				}
				last_update -= gameSpeed;
			}
		}
		DrawString(5, 5, "Score " + std::to_string(snakeUser.score), olc::WHITE);
		DrawString(115, 5, "Level " + std::to_string(level + 1), olc::WHITE);
		DrawString(ScreenWidth()- 60, 5, "Lives " + std::to_string(snakeUser.lives), olc::WHITE);

		bullet.coolDown += fElapsedTime;
		last_update += fElapsedTime;
		return true;
	}
};

int main()
{
	Snake s;
	Supersnake demo;
	if (demo.Construct(300, 300, 3, 3))
		demo.Start();
	return 0;
}