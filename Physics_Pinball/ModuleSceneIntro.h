#pragma once
#include "Module.h"
#include "p2List.h"
#include "p2Point.h"
#include "Globals.h"

class PhysBody;

class ModuleSceneIntro : public Module
{
public:
	ModuleSceneIntro(Application* app, bool start_enabled = true);
	~ModuleSceneIntro();

	bool Start();
	update_status Update();
	bool CleanUp();
	void OnCollision(PhysBody* bodyA, PhysBody* bodyB);

	//Main functionals calls
	void spawnBall();
	void spawnBall2();
	void setWalls();

public:
	p2List<PhysBody*> circles;
	p2List<PhysBody*> boxes;
	p2List<PhysBody*> ricks;

	PhysBody* sensor;
	bool sensed;
	bool ball_lost;

	//Circle sensor oncollision checker
	bool spawnBall_Oncollision;
	int prev_ballCount;

	SDL_Texture* circle;
	SDL_Texture* box;
	SDL_Texture* rick;
	uint bonus_fx;


	//Creating score blocks

	PhysBody* score_block1;
	PhysBody* score_block2;
	PhysBody* score_block3;
	PhysBody* score_block4;
	PhysBody* score_block5;
	PhysBody* score_block6;
	PhysBody* score_block7;

	//Win_Fail_Conditions
	uint balls_left;

	//Textures and SDL_Rects
	SDL_Texture* pinball_spritesheet;
	SDL_Texture* plunger_sprite;
	SDL_Texture* background2;
	SDL_Texture* flippers_and_ball;
	SDL_Texture* gear_texture;
	SDL_Rect rect_bg;
	SDL_Rect rect_plunger;
	SDL_Rect rect_bg2;
	SDL_Rect ballsprite;
	SDL_Rect gearsprite;

	//PhysBody variables
	p2List<PhysBody*> balls;
	p2List<PhysBody*> pinball_walls;
	p2List<PhysBody*> sensors;
	PhysBody* triangle_L;
	PhysBody* triangle_R;


	//FX
	uint ballcreate;
	uint flipper;
	uint gameover;
	uint plunger;
	uint score_block_hit;
	uint ball_lost_fx;
	uint triangle_sound;

	bool music_playing;
	bool load_fxonce;

	uint highest_score;
};
