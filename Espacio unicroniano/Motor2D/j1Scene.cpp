#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Fonts.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Player.h"
#include "j1SceneChange.h"
#include "j1Scene.h"
#include "j1Gui.h"
#include "j1Collisions.h"

#define CAMERA_SPEED 250

j1Scene::j1Scene() : j1Module()
{
	name.create("scene");
}

// Destructor
j1Scene::~j1Scene()
{}

// Called before render is available
bool j1Scene::Awake()
{
	LOG("Loading Scene");
	bool ret = true;

	return ret;
}

bool j1Scene::Load(pugi::xml_node& load) {

	initial_camera.x = load.child("initial_camera").attribute("x").as_int();
	initial_camera.y = load.child("initial_camera").attribute("y").as_int();


	uint winx, winy;
	App->win->GetWindowSize(winx, winy);
	if (col_camera_up == nullptr && col_camera_down == nullptr && col_camera_left == nullptr && col_camera_right == nullptr) {
		App->scene->col_camera_up = App->col->AddCollider({ 0,0, (int)winx, (int)winy / 4 }, COLLIDER_CAM_UP, App->scene);
		App->scene->col_camera_down = App->col->AddCollider({ 0, ((int)winy / 3) * 2, (int)winx, (int)winy / 3 }, COLLIDER_CAM_DOWN, App->scene);
		App->scene->col_camera_left = App->col->AddCollider({ 0,0, (int)winx / 4, (int)winy }, COLLIDER_CAM_LEFT, App->scene);
		App->scene->col_camera_right = App->col->AddCollider({ ((int)winx / 3) * 2, 0, (int)winx / 3, (int)winy }, COLLIDER_CAM_RIGHT, App->scene);
	}

	return true;
}

bool j1Scene::Save(pugi::xml_node& save) const {
	save.append_child("initial_camera");

	save.child("initial_camera").append_attribute("x");
	save.child("initial_camera").append_attribute("y");
	save.child("initial_camera").attribute("x").set_value(initial_camera.x);
	save.child("initial_camera").attribute("y").set_value(initial_camera.y);

	return true;
}

// Called before the first frame
bool j1Scene::Start()
{
	debug_tex = App->tex->Load("maps/debug.png");

	//App->audio->PlayMusic("audio/music/Brain_Damage.ogg");
	iPoint pos;
	pos.x = pos.y = 50;

	//App->gui->CreateUI_Button("HOLA", pos, { 353, 346, 169, 39 }, { 353,346,169,39 }, { 353, 390, 169, 39 }, Events::ButtonEvents::PATATA, nullptr, false);

	StartMenu();

	initial_camera.x = App->render->camera.x;
	initial_camera.y = App->render->camera.y;

	return true;
}

bool j1Scene::StartMenu() {
	App->gui->DeleteAllUI();

	//background;

	//play button
	iPoint play;
	play.x = 100;
	play.y = 300;
	App->gui->CreateUI_Button("Play", play, { 353, 346, 169, 39 }, { 353,390,169,39 }, { 353,346,169,39 }, Events::ButtonEvents::NEWGAME);

	//continue button
	iPoint cont;
	cont.x = 100;
	cont.y = 350;
	if (App->IsSaveDataExist("save_game.xml") == false) {
		App->gui->CreateUI_Button("Continue", cont, { 353, 302, 169, 39 }, { 353,302,169,39 }, { 353,302,169,39 }, Events::ButtonEvents::NONE);
	}
	else {
		App->gui->CreateUI_Button("Continue", cont, { 353, 346, 169, 39 }, { 353,390,169,39 }, { 353,346,169,39 }, Events::ButtonEvents::CONTINUE);
	}

	//settings button
	iPoint setting;
	setting.x = 100;
	setting.y = 400;
	App->gui->CreateUI_Button("Settings", setting, { 353, 346, 169, 39 }, { 353,390,169,39 }, { 353,346,169,39 }, Events::ButtonEvents::SETTINGS);

	//Exit button
	iPoint exit;
	exit.x = 100;
	exit.y = 450;
	App->gui->CreateUI_Button("Exit", exit, { 353, 346, 169, 39 }, { 353,390,169,39 }, { 353,346,169,39 }, Events::ButtonEvents::EXIT);


	return true;
}

void j1Scene::SettingsMenu() {
	//window parent
	uint width, height;
	App->win->GetWindowSize(width, height);

	iPoint menu;
	menu.x = width / 2 - (312 / 2);
	menu.y = height / 2 - (287 / 2);

	UI * window = App->gui->CreateUI_Window("Settings", menu, { 31,75,312,287 });

	//closing button
	iPoint close;
	close.x = menu.x + 258;
	close.y = menu.y;

	App->gui->CreateUI_Button("X", close, { 228,686,54,54 }, { 292,686,54,54 }, { 161,685,54,54 }, Events::ButtonEvents::CLOSEWINDOW, window);

	//back to menu button
	iPoint back;
	back.x = menu.x + (312 / 2) - (169 / 2);
	back.y = menu.y + 250;

	App->gui->CreateUI_Button("Back to menu", back, { 353, 346, 169, 39 }, { 353,390,169,39 }, { 353,346,169,39 }, Events::ButtonEvents::BACKTOMENU, window);

	//sliders
	iPoint music;
	music.x = menu.x + 312 / 3;
	music.y = menu.y + 35;

	App->gui->CreateUI_ScrollBar(music, { 637,736,16,203 }, { 591,786,33,33 }, 20, Events::ScrollEvents::MUSIC, window);

	iPoint fx;
	fx.x = menu.x + (312 / 3) * 2;
	fx.y = menu.y + 35;

	App->gui->CreateUI_ScrollBar(fx, { 637,736,16,203 }, { 591,786,33,33 }, 20, Events::ScrollEvents::FX, window);

	//Text
	iPoint musicTxt;
	musicTxt.x = music.x - 50;
	musicTxt.y = music.y + 50;

	App->gui->CreateUI_Text("Music:", musicTxt, "Music:", "Music:", window);

	iPoint fxTxt;
	fxTxt.x = fx.x - 50;
	fxTxt.y = fx.y + 50;

	App->gui->CreateUI_Text("Effects:", fxTxt, "Effects:", "Effects:", window);
}

// Called each loop iteration
bool j1Scene::PreUpdate(float dt)
{
	BROFILER_CATEGORY("Scene_PreUpdate", Profiler::Color::Azure)

	static iPoint origin;
	static iPoint origin2;
	static bool origin_selected = false;
	static bool origin_selected2 = false;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_DOWN)
	{
		if (origin_selected == true)
		{
			App->pathfinding->CreatePath(origin, p, false);
			origin_selected = false;
		}
		else
		{
			origin = p;
			origin_selected = true;
		}
	}

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene_Update", Profiler::Color::Red)


	dt_scene = dt;
	

	//  6: Make the camera movement independent of framerate
	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += (CAMERA_SPEED * dt);

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= (CAMERA_SPEED * dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += (CAMERA_SPEED * dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= (CAMERA_SPEED * dt);



	if (App->input->GetKey(SDL_SCANCODE_RETURN) == KEY_DOWN) {
		App->entity->pause = !App->entity->pause;
	}
	if (App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN) {
		App->entity->pause = true;
		SettingsMenu();
	}


	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) { //Change first level
		App->scene_change->ChangeMap(2.0f, 2);
	}
	if (App->input->GetKey(SDL_SCANCODE_F2) == KEY_DOWN) { //Change second level
		App->scene_change->ChangeMap(2.0f, 1);
	}
	if (App->input->GetKey(SDL_SCANCODE_F3) == KEY_DOWN) { //Reset all the level
		App->render->camera.x = initial_camera.x;
		App->render->camera.y = initial_camera.y;

		App->entity->ResetEntities();
	}
	if (App->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN) {
		App->SaveGame("save_game.xml");
	}
	if (App->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN) {
		App->LoadGame("save_game.xml");
	}
	if (App->input->GetKey(SDL_SCANCODE_F11) == KEY_DOWN) { //Enable/Disable FPS cap to 30
		if (App->max_framerate == 0) {
			App->ChangeFrameCap(30);
		}
		else if (App->max_framerate == 30) {
			App->ChangeFrameCap(0);
		}
	}
	


	uint winx, winy;
	App->win->GetWindowSize(winx, winy);

	if (col_camera_up != nullptr && col_camera_down != nullptr && col_camera_left != nullptr && col_camera_right != nullptr) {
		col_camera_up->SetPos(-1 * App->render->camera.x, -1 * App->render->camera.y);
		col_camera_down->SetPos(-1 * App->render->camera.x, -1 * App->render->camera.y + ((int)winy / 3) * 2);
		col_camera_left->SetPos(-1 * App->render->camera.x, -1 * App->render->camera.y);
		col_camera_right->SetPos(-1 * App->render->camera.x + ((int)winx / 3) * 2, -1 * App->render->camera.y);
	}
	

	App->map->Draw();

	//Show Pathfinding result path
	int x, y;

	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p = App->map->WorldToMap(p.x, p.y);
	p = App->map->MapToWorld(p.x, p.y);

	//App->render->Blit(debug_tex, p.x, p.y);

	/*const p2DynArray<Path>* path = App->pathfinding->GetLastPath();

	for (uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->PosX, path->At(i)->PosY);
		App->render->Blit(debug_tex, pos.x, pos.y);
	}*/

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate(float dt)
{
	bool ret = true;

	return ret;
}

// Called before quitting
bool j1Scene::CleanUp()
{
	LOG("Freeing scene");

	return true;
}

void j1Scene::OnCollision(Collider*c1, Collider* c2) {
	if (c1->type == COLLIDER_CAM_UP) {
		App->render->camera.y += (0.8 * CAMERA_SPEED * dt_scene);
	}
	if (c1->type == COLLIDER_CAM_DOWN) {
		App->render->camera.y -= (CAMERA_SPEED * dt_scene);
	}
	if (c1->type == COLLIDER_CAM_LEFT) {
		App->render->camera.x += (CAMERA_SPEED * dt_scene);
	}
	if (c1->type == COLLIDER_CAM_RIGHT) {
		App->render->camera.x -= (0.8 * CAMERA_SPEED * dt_scene);
	}
}
