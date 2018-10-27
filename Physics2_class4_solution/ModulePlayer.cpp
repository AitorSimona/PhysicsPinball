#include "Globals.h"
#include "Application.h"
#include "ModulePlayer.h"
#include "ModuleTextures.h"
#include "ModulePhysics.h"
#include "ModuleRender.h"
#include "ModuleInput.h"
#include "ModuleAudio.h"
#include "ModuleSceneIntro.h"

ModulePlayer::ModulePlayer(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

ModulePlayer::~ModulePlayer()
{}

// Load assets
bool ModulePlayer::Start()
{
	LOG("Loading player");

	right_flipper = App->physics->CreateRightFlipper(352,675,1,16);
	left_flipper = App->physics->CreateLeftFlipper(250,720,1,14);

	right_flipper2 = App->physics->CreateRightFlipper(378, 615,2,12);
	left_flipper2 = App->physics->CreateLeftFlipper(228, 660,2,12);
	plunge = App->physics->CreatePlunge();

	flippers_tex = App->textures->Load("pinball/Sprites.png");

	if (App->audio->isAudioDeviceOpened) {
		flipper_hit_fx = App->audio->LoadFx("audio/sound_fx/flipper_hit.wav");
		plunge_fx = App->audio->LoadFx("audio/sound_fx/fire_ball.wav");
	}

	rect_rFlipper.h = 22;
	rect_rFlipper.w = 81;
	rect_rFlipper.x = 0;
	rect_rFlipper.y = 85;

	rect_rFlipper2.h = 18;
	rect_rFlipper2.w = 64;
	rect_rFlipper2.x = 17;
	rect_rFlipper2.y = 65;

	rect_lFlipper.h = 22;
	rect_lFlipper.w = 81;
	rect_lFlipper.x = 0;
	rect_lFlipper.y = 42;

	rect_lFlipper2.h = 18;
	rect_lFlipper2.w = 64;
	rect_lFlipper2.x = 0;
	rect_lFlipper2.y = 23;

	return true;
}

// Unload assets
bool ModulePlayer::CleanUp()
{
	LOG("Unloading player");

	if (left_flipper != NULL)
	{
		App->physics->world->DestroyBody(left_flipper->bodyB);
		App->physics->world->DestroyBody(left_flipper->body);
		left_flipper = NULL;
	}

	if (right_flipper != NULL)
	{
		App->physics->world->DestroyBody(right_flipper->bodyB);
		App->physics->world->DestroyBody(right_flipper->body);
		right_flipper = NULL;
	}

	if (left_flipper2 != NULL)
	{
		App->physics->world->DestroyBody(left_flipper2->bodyB);
		App->physics->world->DestroyBody(left_flipper2->body);
		left_flipper2 = NULL;
	}

	if (right_flipper2 != NULL)
	{
		App->physics->world->DestroyBody(right_flipper2->bodyB);
		App->physics->world->DestroyBody(right_flipper2->body);
		right_flipper2 = NULL;
	}

	if (plunge != NULL)
	{
		App->physics->world->DestroyBody(plunge->body);
		App->physics->world->DestroyBody(plunge->bodyB);
		plunge = NULL;
	}


	return true;
}

// Update: draw background
update_status ModulePlayer::Update()
{

	// ----- Blitting flippers -----

	b2Vec2 anchorRVec = right_flipper->joint->GetAnchorB();
	App->renderer->Blit(flippers_tex, 350, 760, &rect_rFlipper, 1.0f, right_flipper->GetRotation(), anchorRVec.x + 70, anchorRVec.y-12);

	b2Vec2 anchorLVec = left_flipper->joint->GetAnchorB();
	App->renderer->Blit(flippers_tex, 250, 765, &rect_lFlipper, 1.0f, left_flipper->GetRotation(), anchorLVec.x, anchorLVec.y -4);

	b2Vec2 anchorRVec2 = right_flipper2->joint->GetAnchorB();
	App->renderer->Blit(flippers_tex, 390, 680, &rect_rFlipper2, 1.0f, right_flipper2->GetRotation(), anchorRVec2.x + 60, anchorRVec2.y - 8);

	b2Vec2 anchorLVec2 = left_flipper2->joint->GetAnchorB();
	App->renderer->Blit(flippers_tex, 225, 685, &rect_lFlipper2, 1.0f, left_flipper2->GetRotation(), anchorLVec2.x, anchorLVec2.y - 4);

	// ----- Flippers and plunge audio control -----

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_DOWN)
		App->audio->PlayFx(flipper_hit_fx);

	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_DOWN)
		App->audio->PlayFx(flipper_hit_fx);


	// ----- Flippers torque -----

	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
	{
		left_flipper->body->ApplyTorque(-65.0f, true);
		left_flipper2->body->ApplyTorque(-65.0f, true);
	}
	else
	{
		left_flipper->body->ApplyTorque(10.0f, true);
		left_flipper2->body->ApplyTorque(10.0f, true);
	}


	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
	{
		right_flipper->body->ApplyTorque(65.0f, true);
		right_flipper2->body->ApplyTorque(65.0f, true);
	}
	else
	{
		right_flipper->body->ApplyTorque(-10.0f, true);
		right_flipper2->body->ApplyTorque(-10.0f, true);
	}

	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		plunge->body->ApplyForceToCenter(b2Vec2(0, 250), true);

	return UPDATE_CONTINUE;
}



