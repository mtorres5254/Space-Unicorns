#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Input.h"
#include "j1Textures.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Map.h"
#include "j1PathFinding.h"
#include "j1Player.h"
#include "j1SceneChange.h"
#include "j1Scene.h"
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

// Called before the first frame
bool j1Scene::Start()
{
	if(App->map->Load("mapa.tmx") == true)
	{
		int w, h;
		uchar* data = NULL;
		if(App->map->CreateWalkabilityMap(w, h, &data))
			App->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);

		p2List_item<ObjectLayer*>* ob_lay;
		for (ob_lay = App->map->data.obj_layers.start; ob_lay; ob_lay = ob_lay->next) {
			if (ob_lay->data->name == "Entities") {
				App->entity->LoadFromObjectLayer(ob_lay->data);
			}
		}
		App->render->camera.x = 0;
		App->render->camera.y = -900;
	
	}

	uint winx, winy;
	App->win->GetWindowSize(winx, winy);

	col_camera_up = App->col->AddCollider({ 0,0, (int)winx, (int)winy / 4 }, COLLIDER_CAM_UP, this);
	col_camera_down = App->col->AddCollider({ 0, ((int)winy / 3) * 2, (int)winx, (int)winy / 3 }, COLLIDER_CAM_DOWN, this);
	col_camera_left = App->col->AddCollider({ 0,0, (int)winx / 4, (int)winy }, COLLIDER_CAM_LEFT, this);
	col_camera_right = App->col->AddCollider({ ((int)winx / 3) * 2, 0, (int)winx / 3, (int)winy }, COLLIDER_CAM_RIGHT, this);

	initial_camera.x = App->render->camera.x;
	initial_camera.y = App->render->camera.y;

	return true;
}

// Called each loop iteration
bool j1Scene::PreUpdate(float dt)
{
	BROFILER_CATEGORY("Scene_PreUpdate", Profiler::Color::Azure)

		LOG("%i x %i", App->render->camera.x, App->render->camera.y);

	return true;
}

// Called each loop iteration
bool j1Scene::Update(float dt)
{
	BROFILER_CATEGORY("Scene_Update", Profiler::Color::Red)


	dt_scene = dt;
		
	if(App->input->GetKey(SDL_SCANCODE_L) == KEY_DOWN)
		App->LoadGame("save_game.xml");

	if(App->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
		App->SaveGame("save_game.xml");

	//  6: Make the camera movement independent of framerate
	if(App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
		App->render->camera.y += (CAMERA_SPEED * dt);

	if(App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT)
		App->render->camera.y -= (CAMERA_SPEED * dt);

	if(App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT)
		App->render->camera.x += (CAMERA_SPEED * dt);

	if(App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)
		App->render->camera.x -= (CAMERA_SPEED * dt);


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

	col_camera_up->SetPos(-1 * App->render->camera.x, -1 * App->render->camera.y);
	col_camera_down->SetPos(-1 * App->render->camera.x, -1 * App->render->camera.y + ((int)winy / 3) * 2);
	col_camera_left->SetPos(-1 * App->render->camera.x, -1 * App->render->camera.y);
	col_camera_right->SetPos(-1 * App->render->camera.x + ((int)winx / 3) * 2, -1 * App->render->camera.y);


	App->map->Draw();

	//Show Pathfinding result path
	const p2DynArray<iPoint>* path = App->pathfinding->GetLastPath();

	for(uint i = 0; i < path->Count(); ++i)
	{
		iPoint pos = App->map->MapToWorld(path->At(i)->x, path->At(i)->y);
		//App->render->Blit(debug_tex, pos.x, pos.y);
	}

	return true;
}

// Called each loop iteration
bool j1Scene::PostUpdate(float dt)
{
	bool ret = true;


	if(App->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;

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
