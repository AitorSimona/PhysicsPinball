#include "Globals.h"
#include "Application.h"
#include "ModuleWindow.h"
#include "ModuleUI.h"
#include "p2SString.h"
#include "ModuleSceneIntro.h"


ModuleUI::ModuleUI(Application* app, bool start_enabled) : Module(app, start_enabled)
{
	score = 0;
}

ModuleUI::~ModuleUI()
{}

// Load assets
bool ModuleUI::Start()
{
	LOG("Loading ModuleUI assets");
	bool ret = true;
	

	return ret;
}

// Update: draw background
update_status ModuleUI::Update()
{

	update_status status = UPDATE_CONTINUE;
	
	p2SString title("PINBALL 2	              SCORE: %i           HIGHEST: %i          BALLS LEFT: %i", App->ui->score,App->scene_intro->highest_score ,App->scene_intro->balls_left);

	App->window->SetTitle(title.GetString());

	return status;
}

bool ModuleUI::CleanUp()
{
	LOG("Unloading ModuleUI");

	bool ret = true;

	return ret;
}

