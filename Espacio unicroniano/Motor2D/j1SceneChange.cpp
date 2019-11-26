#include <math.h>
#include "p2Log.h"
#include "j1App.h"
#include "j1SceneChange.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Audio.h"
#include "j1Collisions.h"
#include "j1Map.h"

#include "SDL\include\SDL_render.h"
#include "SDL\include\SDL_timer.h"

j1MapChange::j1MapChange()
{
	//TODO: screen en funcio de config
	name.create("scenes");
}

j1MapChange::~j1MapChange()
{}

bool j1MapChange::Awake(pugi::xml_node& node)
{
	uint width;
	uint height;
	App->win->GetWindowSize(width, height);
	uint scale = App->win->GetScale();
	screen = { 0, 0, (int)width * (int)scale, (int)height * (int)scale };

	//charge diferents maps
	pugi::xml_node scene;
	for (scene = node.child("scene"); scene; scene = scene.next_sibling("scene")) {
		if (map1.Length() == 0) {
			map1 = scene.attribute("name").as_string();
		}
		else {
			map2 = scene.attribute("name").as_string();
		}
	}


	return true;
}

bool j1MapChange::Start()
{
	LOG("Preparing Fade Screen");
	SDL_SetRenderDrawBlendMode(App->render->renderer, SDL_BLENDMODE_BLEND);
	return true;
}

bool j1MapChange::Update(float dt)
{
	BROFILER_CATEGORY("SceneChange_Update", Profiler::Color::DarkSlateBlue )

	if (current_step == fade_step::none)
	{
		return true;
	}

	uint now = SDL_GetTicks() - start_time;
	float normalized = 1.0f < ((float)now / (float)total_time) ? 1.0f : ((float)now / (float)total_time);

		
	switch (current_step)
	{
			case fade_step::fade_to_black:
		{
			if (now >= total_time)
			{
				//map change logic
				App->map->CleanUp();
				App->col->DeleteAll();
				App->entity->DestroyAll();

				if (map_to_change == 1) {
					App->map->Load(map1.GetString());
				}
				else if (map_to_change == 2) {
					App->map->Load(map2.GetString());
				}

				//pom pom
				total_time += total_time;
				start_time = SDL_GetTicks();
				fading = false;
				current_step = fade_step::fade_from_black;
			}
		}break;

		case fade_step::fade_from_black:
		{
			normalized = 1.0f - normalized;

			if (now >= total_time)
			{
				//reload things
				p2List_item<ObjectLayer*>* obj;
				for (obj = App->map->data.obj_layers.start; obj; obj = obj->next) {
					App->col->LoadFromObjectLayer(obj->data);
				}
				uint winx, winy;
				App->win->GetWindowSize(winx, winy);

				App->scene->col_camera_up = App->col->AddCollider({ 0,0, (int)winx, (int)winy / 4 }, COLLIDER_CAM_UP, App->scene);
				App->scene->col_camera_down = App->col->AddCollider({ 0, ((int)winy / 3) * 2, (int)winx, (int)winy / 3 }, COLLIDER_CAM_DOWN, App->scene);
				App->scene->col_camera_left = App->col->AddCollider({ 0,0, (int)winx / 4, (int)winy }, COLLIDER_CAM_LEFT, App->scene);
				App->scene->col_camera_right = App->col->AddCollider({ ((int)winx / 3) * 2, 0, (int)winx / 3, (int)winy }, COLLIDER_CAM_RIGHT, App->scene );

				current_step = fade_step::none;
			}

		}break;
	}
	
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

bool j1MapChange::ChangeMap(float time, int map_id)
{
	bool ret = false;

	if (map_id != NULL) {
		map_to_change = map_id;
	}
	else {
		map_to_change = 2;
	}

	if (current_step == fade_step::none)
	{
		current_step = fade_step::fade_to_black;
		start_time = SDL_GetTicks();
		total_time = (Uint32)(time*0.5f*1000.0f);
		fading = true;
		ret = true;
	}

	return ret;
}

bool j1MapChange::IsChanging() const
{
	return current_step != fade_step::none;
}