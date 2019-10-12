#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Scene.h"
#include "j1Collisions.h"
#include "j1Map.h"
#include "j1Audio.h"
#include "SDL/include/SDL_timer.h"
//include SDL_timer.h

j1Player::j1Player() {

	//add animation pushbacks
	
	
	//IDLE
	idle.PushBack({ 256 , 0 ,32 , 32 });
	idle.speed = 0.5f;

	//WALKING

	walking.PushBack({ 0,0 , 32 , 32 });
	walking.PushBack({ 32 , 0 , 32 , 32 });
	walking.PushBack({ 64 , 0 ,	32, 32 });
	walking.PushBack({128 , 0 ,	32 , 32 });
	walking.PushBack({ 256 , 0 ,32 , 32});
	walking.PushBack({ 512 , 0 , 32 , 32});
	walking.speed = 1.0f;

	//JUMPING

	//HOOK


		
}

j1Player::~j1Player()
{}

bool j1Player::Start() {

	//load conditions
	//flip = false
	position.x = 20;
	position.y = 200;
	Current_Animation = idle;

	//load graphics
	normal_graphics = App->tex->Load("textures/walk.png");
	current_graphics = normal_graphics;
	//load sounds and collisions
	return true;
}

bool j1Player::CleanUp() {
	//unload graphics
	App->tex->UnLoad(normal_graphics);
	App->tex->UnLoad(current_graphics);
	return true;
}

bool j1Player::PreUpdate() {
	//inputs
	Current_Animation.GetCurrentFrame() = idle.GetCurrentFrame();
	/*godModechetao
	if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT) {
		position.y -= player_speed;
		
	}
	if (App->input->GetKey(SDL_SCANCODE_DOWN) == KEY_REPEAT) {
		position.y += player_speed;
	}
	if (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) {
		position.x += player_speed;
		flip = SDL_FLIP_NONE;
	}
	if (App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) {
		position.x -= player_speed;
		flip = SDL_FLIP_HORIZONTAL;
	}
	*/
	if (whileair = false) {
		if (App->input->GetKey(SDL_SCANCODE_UP) == KEY_REPEAT)
			
			position.y += player_speed;
			jumping = true;
		if ((App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT) && !not_forward) {
			position.x += player_speed;
			Current_Animation.GetCurrentFrame() = walking.GetCurrentFrame();
			flip = SDL_FLIP_NONE;
		}
		if ((App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) && !not_backwards) {
			position.x -= player_speed;
			Current_Animation.GetCurrentFrame() = walking.GetCurrentFrame();
			flip = SDL_FLIP_HORIZONTAL;
		}

	
	}
	//make movement while player is in air


	//input not works
	if ((App->input->GetKey(SDL_SCANCODE_LEFT) == KEY_REPEAT) && (App->input->GetKey(SDL_SCANCODE_RIGHT) == KEY_REPEAT)) {
		Current_Animation.GetCurrentFrame() = idle.GetCurrentFrame();
	}
	
}



bool j1Player::Update() { return true; }


bool j1Player::PostUpdate() {
	App->render->Blit(current_graphics, position.x, position.y, &(Current_Animation.GetCurrentFrame()), 1.5f, 0, 0, 0, flip);

	return true;
}

//collisions

