#include <math.h>
#include "p2Log.h"
#include "j1App.h"
#include "j1SceneChange.h"
#include "j1Render.h"
#include "j1Scene.h"
#include "j1Window.h"
#include "j1Player.h"
#include "j1Audio.h"
#include "j1Map.h"

#include "SDL\include\SDL_render.h"
#include "SDL\include\SDL_timer.h"

j1MapChange::j1MapChange()
{
	//TODO: screen en funcio de config
	
}

j1MapChange::~j1MapChange()
{}

bool j1MapChange::Awake(pugi::xml_node&)
{
	uint width;
	uint height;
	App->win->GetWindowSize(width, height);
	uint scale = App->win->GetScale();
	screen = { 0, 0, (int)width * (int)scale, (int)height * (int)scale };
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
				if (App->scene->map1Loaded == true) {
					App->map->ReLoad(App->scene->map2.GetString());
					App->scene->map1Loaded = false;
				}
				if (App->scene->map2Loaded == true) {
					App->map->ReLoad(App->scene->map1.GetString());
					App->scene->map2Loaded = false;
				}
				
				App->player->Restart();
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
				current_step = fade_step::none;
				if (App->scene->map1Loaded == true) {
					App->scene->map2Loaded = true;
				}
				if (App->scene->map2Loaded == true) {
					App->scene->map1Loaded = true;
				}
			}

		}break;
	}
	
	SDL_SetRenderDrawColor(App->render->renderer, 0, 0, 0, (Uint8)(normalized * 255.0f));
	SDL_RenderFillRect(App->render->renderer, &screen);

	return true;
}

bool j1MapChange::ChangeMap(float time)
{
	bool ret = false;


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