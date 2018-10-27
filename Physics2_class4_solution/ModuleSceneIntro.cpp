#include "Globals.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleSceneIntro.h"
#include "ModuleInput.h"
#include "ModuleTextures.h"
#include "ModuleAudio.h"
#include "ModulePhysics.h"
#include "ModulePlayer.h"
#include "ModuleFadeToBlack.h"
#include "ModuleUI.h"

ModuleSceneIntro::ModuleSceneIntro(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	circle = box = rick = NULL;
	sensed = false;
}

ModuleSceneIntro::~ModuleSceneIntro()
{}

// Load assets
bool ModuleSceneIntro::Start()
{
	LOG("Loading Intro assets");
	bool ret = true;

	//
	balls_left = 3;
	App->ui->score = 0;

	if (!App->audio->IsEnabled() && App->audio->isAudioDeviceOpened) {
		App->audio->Enable();
		App->audio->Init();
	}

	if (!App->textures->IsEnabled()) {
		App->textures->Enable();
		App->textures->Init();
	}

	if (!App->ui->IsEnabled()) {
		App->ui->Enable();
		App->ui->Start();
	}
	
	App->renderer->camera.x = App->renderer->camera.y = 0;

	//if (App->audio->isAudioDeviceOpened)
	//{
	//	/*hole_in_fx = App->audio->LoadFx("audio/sound_fx/hole_in.wav");*/
	//

	//	if (!App->audio->PlayMusic("audio/music/Nightmaren.ogg"))
	//		ret = false;
	//}

	pinball_spritesheet = App->textures->Load("pinball/Map big.png");

	if (pinball_spritesheet == nullptr)
	{
		LOG("Cannot load the animations spritesheet in SceneIntro");
		ret = false;
	}

	// ---- Setting up SDL_Rect attributes ----

	rect_bg.h = SCREEN_HEIGHT;
	rect_bg.w = SCREEN_WIDTH;
	rect_bg.x = 0;
	rect_bg.y = 0;

	// ------- Setting up wall chains -------
	setWalls();

	//// ----- Creating sensors for the ball -----
	//setSensors();

	App->player->Enable();

	// Spawning ball
	spawnBall();

	//

	/*circle = App->textures->Load("pinball/wheel.png"); */
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");


	//Creating score_blocks

	score_block1=App->physics->CreateCircle(375,160,20);
	score_block2 = App->physics->CreateCircle(285, 160, 20);
	score_block3 = App->physics->CreateCircle(195, 160, 20);
	score_block4 = App->physics->CreateCircle(462, 160, 20);
	score_block5 = App->physics->CreateCircle(205, 265, 20);
	score_block6 = App->physics->CreateCircle(238, 360, 20);
	score_block7 = App->physics->CreateCircle(430, 288, 20);

	circles.add(score_block1);
	circles.add(score_block2);
	circles.add(score_block3);
	circles.add(score_block4);
	circles.add(score_block5);
	circles.add(score_block6);
	circles.add(score_block7);
	//sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50, this);

	return ret;
}

// Load assets
bool ModuleSceneIntro::CleanUp()
{
	LOG("Unloading Intro scene");

	for (p2List_item<PhysBody*>* ball_item = balls.getFirst(); ball_item != NULL; ball_item = ball_item->next)
	{
		App->physics->world->DestroyBody(ball_item->data->body);
	}

	for (p2List_item<PhysBody*>* wall_item = pinball_walls.getFirst(); wall_item != NULL; wall_item = wall_item->next)
	{
		App->physics->world->DestroyBody(wall_item->data->body);
	}

	for (p2List_item<PhysBody*>* circlesitem = circles.getFirst(); circlesitem != NULL; circlesitem = circlesitem->next)
	{
		App->physics->world->DestroyBody(circlesitem->data->body);
	}

	if (triangle_L != NULL)
	{
		App->physics->world->DestroyBody(triangle_L->body);
		triangle_L = NULL;
	}

	if (triangle_R != NULL)
	{
		App->physics->world->DestroyBody(triangle_R->body);
		triangle_R = NULL;
	}

	App->player->CleanUp();
	App->player->Disable();

	App->ui->CleanUp();
	App->ui->Disable();

	App->audio->CleanUp();
	App->audio->Disable();

	App->textures->CleanUp();
	App->textures->Disable();

	balls.clear();
	pinball_walls.clear();

	return true;
}

// Update: draw background
update_status ModuleSceneIntro::Update()
{
	// Blitting background
	App->renderer->Blit(pinball_spritesheet, 0, 0, &rect_bg, 1.0f);

	// Blitting Bouncing triangles
	//App->renderer->Blit(pinball_spritesheet, 113, 621, &triangle_L_anim.GetCurrentFrame(), 1.0f);
	//App->renderer->Blit(pinball_spritesheet, 325, 621, &triangle_R_anim.GetCurrentFrame(), 1.0f);

	// ----- Ball creation -----
	if (App->input->GetKey(SDL_SCANCODE_1) == KEY_DOWN)
	{
		balls.add(App->physics->CreateBall(App->input->GetMouseX(), App->input->GetMouseY(), 14));
		balls.getLast()->data->listener = this;
	}

	// All draw functions ------------------------------------------------------
	p2List_item<PhysBody*>* c = circles.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
	/*	App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());*/
		c = c->next;
	}

	c = boxes.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(box, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	c = ricks.getFirst();

	while(c != NULL)
	{
		int x, y;
		c->data->GetPosition(x, y);
		App->renderer->Blit(rick, x, y, NULL, 1.0f, c->data->GetRotation());
		c = c->next;
	}

	return UPDATE_CONTINUE;
}

void ModuleSceneIntro::OnCollision(PhysBody* bodyA, PhysBody* bodyB)
{
	int x, y;

	App->audio->PlayFx(bonus_fx);

	for (p2List_item<PhysBody*>* bc = balls.getFirst(); bc != NULL; bc = bc->next)
	{
		if (bodyA == bc->data)
		{
			if (bodyB->physType == TRIANGLE)
			{
				App->ui->score += 400;
				/*App->audio->PlayFx(triangle_fx);*/
			}
		}

	}
}


void ModuleSceneIntro::setWalls() {

	// Here we create all chains of the scene

	// Bouncing triangles

	int points_triangle_R_sensor[8] =
	{
		449, 630,
		491, 573,
		496, 577,
		449, 631
	};

	triangle_R = App->physics->CreateChain(0, 0, points_triangle_R_sensor, 8, groupIndex::RIGID_PINBALL, 1.5f, TRIANGLE);

	int points_triangle_L_sensor[8] =
	{
		210, 586,
		246, 650,
		240, 655,
		209, 586
	};

	triangle_L = App->physics->CreateChain(0, 0, points_triangle_L_sensor, 8, groupIndex::RIGID_PINBALL, 1.5f, TRIANGLE);

	int points_triangle_L[12] =
	{
		201, 589,
		196, 630,
		237, 660,
		247, 652,
		207, 583,
		199, 596
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, points_triangle_L, 12, groupIndex::RIGID_PINBALL, 0.1f, NO_DEF_));

	int points_triangle_R[12] =
	{
	    502, 576,
	    493, 624,
		454, 639,
		448, 638,
		495, 572,
		502, 576
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, points_triangle_R, 12, groupIndex::RIGID_PINBALL, 0.1f, NO_DEF_));


	// Static walls

	int points_top_wall[50] = {
		200, 924,
		242, 834,
		89, 741,
		90, 453,
		108, 460,
		96, 443,
		131, 395,
		148, 402,
		134, 383,
		134, 97,
		186, 67,
		258, 42,
		323, 35,
		394, 42,
		450, 53,
		502, 78,
		551, 113,
		597, 161,
		652, 253,
		656, 903,
		607, 901,
		609, 427,
		604, 725,
		466, 806,
		490, 924
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, points_top_wall, 50, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));

	int Stick_right_big[6] =
	{
		571, 485,
		570, 686,
		425, 771
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, Stick_right_big, 6, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));

	int Stick_left_big[6] =
	{
		128, 487,
		128, 685,
		256, 779
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, Stick_left_big, 6, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));

	int Stick_right_small[6] =
	{
		537, 532,
		537, 636,
		455, 689
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, Stick_right_small, 6, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));

	int Stick_left_small[6] =
	{
		163, 540,
		163, 653,
		232, 695
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, Stick_left_small, 6, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));
	

	int top_right_block[24] = {
		549, 307,
		549, 229,
		528, 257,
		540, 225,
		511, 205,
		486, 230,
		503, 197,
		491, 191,
		491, 162,
		483, 193,
		435, 195,
		435, 163
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, top_right_block, 24, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));


	int Scoreblock_right[8] = {
		400, 164,
		401, 193,
		348, 193,
		348, 164
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, Scoreblock_right, 8, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));


	int Scoreblock_middle[8] = {
		312, 163,
		312, 192,
		258, 191,
		258, 163
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, Scoreblock_middle, 8, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));
	

	int Scoreblock_left[8] = {
		221, 164,
		221, 194,
		168, 192,
		166, 163
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, Scoreblock_left, 8, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));

}

void ModuleSceneIntro::spawnBall()
{
	balls.add(App->physics->CreateBall(630, 750, 14));
	balls.getLast()->data->listener = this;
}