#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"
#include "ModuleTextures.h"
#include "ModulePhysics.h"

class PhysBody;


class ModulePlayer : public Module
{
public:
	ModulePlayer(Application* app, bool start_enabled = true);
	virtual ~ModulePlayer();

	bool Start();
	update_status Update();
	bool CleanUp();

public:

	PhysBody* left_flipper;
	PhysBody* right_flipper;

	PhysBody* left_flipper2;
	PhysBody* right_flipper2;
	PhysBody* plunge;
	PhysBody* gear;

	SDL_Rect rect_rFlipper;
	SDL_Rect rect_lFlipper;

	SDL_Rect rect_rFlipper2;
	SDL_Rect rect_lFlipper2;

	SDL_Texture * flippers_tex;

	uint flipper_hit_fx;
	uint plunge_fx;

	b2Vec2 plungespritepos;
};