#include "Includes.h"
#define BLOCK 20
#define XBLOCKS 40
#define YBLOCKS 30

typedef enum{SNAKE, WALL, FOOD, AIR}BlockType;

typedef struct{
	uint x, y;
}Food;
Food *food;

typedef struct Snake{
	uint x, y;
	struct Snake* next;
}Snake;
Snake *head;

BlockType whatsAt(uint x, uint y)
{
	if(x==0 || y==0)
		return WALL;
	if(x>=XBLOCKS-1 || y>=YBLOCKS-1)
		return WALL;
	if(x==food->x && y==food->y)
		return FOOD;
	Snake *current = head;
	while(current != NULL){
		if(x==current->x && y==current->y)
			return SNAKE;
		current = current->next;
	}
	return AIR;
}

bool inSnake(uint x, uint y)
{
	Snake *current = head;
	while(current != NULL){
		if(x == current->x && y == current->y)
			return true;
		current = current->next;
	}
	return false;
}

void drawBlock(uint x, uint y, Color c)
{
	setColor(c);
	fillRect(x*BLOCK, y*BLOCK, BLOCK, BLOCK);
	setColor(GREY);
	drawRect(x*BLOCK, y*BLOCK, BLOCK, BLOCK);
}

void drawWalls()
{
	for(uint y = 0; y < YBLOCKS; y++){
		drawBlock(0,y,WHITE);
		drawBlock(XBLOCKS-1,y,WHITE);
	}
	for(uint x = 1; x < XBLOCKS-1; x++){
		drawBlock(x,0,WHITE);
		drawBlock(x,YBLOCKS-1,WHITE);
	}
}

void drawSnake()
{
	Snake *current = head;
	while(current != NULL){
		drawBlock(current->x, current->y, GREEN);
		current = current->next;
	}
}

void drawFood()
{
	drawBlock(food->x, food->y, RED);
}

void drawAll()
{
	clear();
	drawWalls();
	drawSnake();
	drawFood();
	draw();
}

void placeFood()
{
	uint x, y;
	do{
		x=(rand()%(XBLOCKS-1))+1;
		y=(rand()%(YBLOCKS-1))+1;
	}while(whatsAt(x, y) != AIR);
	food->x = x;
	food->y = y;
}

void init()
{
	gfx_init(BLOCK*XBLOCKS, BLOCK*YBLOCKS);
	food = malloc(sizeof(Food));
	head = malloc(sizeof(Snake));
	head->x = (rand()%(XBLOCKS-1))+1;
	head->y = (rand()%(YBLOCKS-1))+1;
	placeFood();
	printf("Head x:%u y:%u\n", head->x, head->y);
	printf("Food x:%u y:%u\n", food->x, food->y);
}

event waitForMove()
{
	printf("Waiting for first movement\n");
	event moveDir = NOTHING;
	while(moveDir==NOTHING){
		moveDir = events();
	}
	printf("Starting\n");
	return moveDir;
}

void tailToHead()
{
	Snake *current = head;
	if(current->next == NULL)
		return;
	while(current->next->next != NULL){
		current = current->next;
	}
	Snake *newTail = current;
	Snake *oldHead = head;
	head = current->next;
	current->next = NULL;
	head->next = oldHead;
}

void growSnake()
{
	Snake *current = head;
	while(current->next != NULL){
		current = current->next;
	}
	current->next = malloc(sizeof(Snake));
	current->next->x = current-> x;
	current->next->y = current-> y;
}

void loose()
{
	while(1){
		events();
	}
}

void moveSnake(event dir)
{
	printf("Moving snake\n");
	uint x = head->x;
	uint y = head->y;
	switch (dir) {
		case MOVE_D:
			y++;
			break;
		case MOVE_U:
			y--;
			break;
		case MOVE_R:
			x++;
			break;
		case MOVE_L:
			x--;
			break;
		default:
		case NOTHING:
			return;
			break;
	}

	switch (whatsAt(x,y)) {
		case SNAKE:
		case WALL:
			loose();
			break;
		case FOOD:
			growSnake();
			placeFood();
		case AIR:
			tailToHead();
			head->x = x;
			head->y = y;
		default:
			return;
			break;
	}
}

int main(int argc, char const *argv[])
{
	init();
	drawAll();
	event moveDir = waitForMove();
	event lastMove = NOTHING;
	drawAll();
	resetTime();
	while(1){
		if(elapsedTime()>250){
			moveSnake(moveDir);
			lastMove = moveDir;
			drawAll();
			resetTime();
		}
		event newDir = events();
		if(newDir != NOTHING){
			switch (newDir) {
				case MOVE_D:
					if(lastMove != MOVE_U)
						moveDir = newDir;
					break;
				case MOVE_U:
					if(lastMove != MOVE_D)
						moveDir = newDir;
					break;
				case MOVE_L:
					if(lastMove != MOVE_R)
						moveDir = newDir;
					break;
				case MOVE_R:
					if(lastMove != MOVE_L)
						moveDir = newDir;
					break;
				default:
					moveDir = newDir;
					break;
			}
		}
	}
	return 0;
}
