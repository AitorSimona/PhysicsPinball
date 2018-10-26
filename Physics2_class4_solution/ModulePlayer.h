#pragma once
#include "Module.h"
#include "Globals.h"
#include "p2Point.h"

class PhysBody;
class SDL_Texture;

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

	SDL_Rect rect_rFlipper;
	SDL_Rect rect_lFlipper;

	SDL_Texture * flippers_tex;

	uint flipper_hit_fx;

};