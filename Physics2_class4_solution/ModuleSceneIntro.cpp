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

	//pinball_spritesheet = App->textures->Load("pinball/pinball_sonic_spritesheet.png");

	//if (pinball_spritesheet == nullptr)
	//{
	//	LOG("Cannot load the animations spritesheet in SceneIntro");
	//	ret = false;
	//}

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

	circle = App->textures->Load("pinball/wheel.png"); 
	box = App->textures->Load("pinball/crate.png");
	rick = App->textures->Load("pinball/rick_head.png");
	bonus_fx = App->audio->LoadFx("pinball/bonus.wav");

	sensor = App->physics->CreateRectangleSensor(SCREEN_WIDTH / 2, SCREEN_HEIGHT, SCREEN_WIDTH, 50, this);

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
		App->renderer->Blit(circle, x, y, NULL, 1.0f, c->data->GetRotation());
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

	/*for (p2List_item<PhysBody*>* bc = balls.getFirst(); bc != NULL; bc = bc->next)
	{
		if (bodyA == bc->data)
		{
			if (bodyB->physType == TRIANGLE)
			{
				App->ui->score += 400;
				App->audio->PlayFx(triangle_fx);
			}

			
			if (bodyB->physType == EXIT_CANON)
			{
				ball_created = false;
				balls.getLast()->data->listener = this;
				for (p2List_item<PhysBody*>* p_w = pinball_walls.getFirst(); p_w != NULL; p_w = p_w->next)
				{
					b2Fixture* fixture = p_w->data->body->GetFixtureList();

					while (fixture != NULL)
					{
						b2Filter newFilter;
						newFilter.groupIndex = groupIndex::RIGID_PINBALL;
						fixture->SetFilterData(newFilter);
						fixture = fixture->GetNext();
					}
				}

				b2Fixture* r_flipper_fixture = App->player->right_flipper->body->GetFixtureList();

				while (r_flipper_fixture != NULL)
				{
					b2Filter newFilter;
					newFilter.groupIndex = groupIndex::RIGID_PINBALL;
					r_flipper_fixture->SetFilterData(newFilter);
					r_flipper_fixture = r_flipper_fixture->GetNext();
				}

				b2Fixture* l_flipper_fixture = App->player->left_flipper->body->GetFixtureList();

				while (l_flipper_fixture != NULL)
				{
					b2Filter newFilter;
					newFilter.groupIndex = groupIndex::RIGID_PINBALL;
					l_flipper_fixture->SetFilterData(newFilter);
					l_flipper_fixture = l_flipper_fixture->GetNext();
				}
				break;
			}

		}
	}*/
}


void ModuleSceneIntro::setWalls() {

	// Here we create all chains of the scene

	// Bouncing triangles

	int points_triangle_R_sensor[8] =
	{
		359, 629,
		331, 689,
		340, 688,
		362, 640
	};

	triangle_R = App->physics->CreateChain(0, 0, points_triangle_R_sensor, 8, groupIndex::RIGID_PINBALL, 1.5f, TRIANGLE);

	int points_triangle_L_sensor[8] =
	{
		121, 628,
		148, 687,
		140, 684,
		119, 639
	};

	triangle_L = App->physics->CreateChain(0, 0, points_triangle_L_sensor, 8, groupIndex::RIGID_PINBALL, 1.5f, TRIANGLE);

	int points_triangle_L[14] =
	{
		114, 679,
		114, 624,
		119, 624,
		150, 690,
		150, 696,
		144, 696,
		116, 681
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, points_triangle_L, 14, groupIndex::RIGID_PINBALL, 0.1f, NO_DEF_));

	int points_triangle_R[14] =
	{
		366, 624,
		367, 678,
		365, 681,
		336, 696,
		330, 696,
		330, 690,
		362, 624
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, points_triangle_R, 14, groupIndex::RIGID_PINBALL, 0.1f, NO_DEF_));


	// Static walls

	int points_top_wall[160] =
	{
		431, 396,
		427, 403,
		423, 407,
		418, 407,
		421, 401,
		427, 391,
		434, 385,
		440, 374,
		462, 307,
		465, 291,
		470, 267,
		483, 227,
		492, 189,
		497, 165,
		497, 133,
		488, 107,
		475, 88,
		461, 75,
		440, 66,
		419, 58,
		385, 58,
		360, 67,
		337, 82,
		320, 104,
		314, 121,
		310, 128,
		310, 142,
		308, 143,
		308, 182,
		314, 198,
		321, 220,
		329, 237,
		334, 240,
		341, 248,
		341, 260,
		332, 260,
		328, 255,
		328, 246,
		316, 236,
		298, 236,
		291, 244,
		284, 244,
		280, 240,
		281, 234,
		282, 228,
		284, 218,
		285, 211,
		286, 196,
		286, 180,
		285, 164,
		285, 148,
		278, 126,
		267, 116,
		259, 109,
		252, 104,
		244, 99,
		237, 96,
		216, 85,
		175, 77,
		155, 77,
		125, 82,
		95, 90,
		74, 99,
		53, 118,
		37, 140,
		31, 151,
		28, 159,
		26, 167,
		24, 177,
		23, 196,
		23, 234,
		29, 268,
		38, 294,
		54, 327,
		60, 352,
		59, 366,
		74, 411,
		72, 411,
		69, 406,
		65, 398
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, points_top_wall, 160, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));

	int points_bottom_wall[108] =
	{
		24, 415,
		32, 432,
		44, 454,
		48, 467,
		44, 472,
		36, 480,
		36, 559,
		38, 565,
		43, 568,
		47, 572,
		47, 580,
		33, 595,
		31, 600,
		29, 614,
		27, 632,
		25, 659,
		24, 665,
		24, 832,
		34, 835,
		54, 835,
		64, 832,
		64, 774,
		71, 775,
		184, 832,
		184, 962,
		296, 962,
		296, 832,
		408, 776,
		416, 774,
		416, 834,
		427, 840,
		446, 840,
		456, 834,
		456, 668,
		453, 638,
		451, 617,
		447, 595,
		433, 579,
		433, 574,
		457, 517,
		457, 511,
		449, 504,
		472, 484,
		472, 825,
		477, 840,
		499, 840,
		504, 825,
		504, 433,
		500, 425,
		494, 420,
		481, 418,
		469, 424,
		469, 420,
		475, 411
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, points_bottom_wall, 108, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));

	int points_right_L[28] =
	{
		310, 758,
		310, 774,
		320, 774,
		407, 730,
		416, 722,
		416, 643,
		413, 636,
		408, 636,
		408, 691,
		403, 704,
		394, 715,
		367, 731,
		327, 750,
		310, 758
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, points_right_L, 28, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));

	int points_left_L[28] =
	{
		170, 758,
		169, 774,
		160, 774,
		108, 748,
		72, 730,
		63, 721,
		64, 642,
		66, 636,
		72, 636,
		72, 691,
		80, 708,
		89, 718,
		97, 722,
		170, 758
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, points_left_L, 28, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));

	int points_top_U[46]
	{
		415, 127,
		422, 170,
		420, 202,
		434, 197,
		449, 183,
		455, 163,
		455, 145,
		450, 132,
		443, 122,
		434, 113,
		418, 107,
		403, 106,
		390, 109,
		379, 115,
		369, 123,
		360, 135,
		356, 151,
		356, 167,
		363, 179,
		369, 188,
		373, 192,
		382, 166,
		380, 135
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, points_top_U, 46, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));

	int points_left_U[30] =
	{
		131, 286,
		115, 299,
		108, 305,
		102, 300,
		105, 292,
		108, 283,
		110, 244,
		114, 232,
		125, 225,
		140, 226,
		154, 234,
		167, 247,
		183, 263,
		176, 271,
		160, 259
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, points_left_U, 30, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));



	/*pinball_walls.add(App->physics->CreateChain(0, 0, left_U_top_points, 36, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));

	int right_U_top_points[38] =
	{
		182, 127,
		203, 131,
		219, 137,
		233, 152,
		239, 166,
		242, 184,
		243, 191,
		246, 192,
		250, 189,
		250, 174,
		244, 158,
		237, 144,
		226, 133,
		216, 126,
		204, 122,
		192, 119,
		181, 118,
		176, 120,
		176, 124
	};

	pinball_walls.add(App->physics->CreateChain(0, 0, right_U_top_points, 38, groupIndex::RIGID_PINBALL, 0.01f, NO_DEF_));*/

}

void ModuleSceneIntro::spawnBall()
{
	balls.add(App->physics->CreateBall(488, 800, 14));
	balls.getLast()->data->listener = this;
}