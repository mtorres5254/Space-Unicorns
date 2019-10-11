#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Scene.h"
//include SDL_timer.h

j1Player::j1Player() {

	//add animation pushbacks
	
	
	//IDLE
	//idle.PushBack({ 256 , 0 ,32 , 32 });

	//WALKING

	//walk.PushBack({ 0,0 , 32 , 32 });
	//walk.PushBack({ 32 , 0 , 32 , 32 });
	//walk.PushBack({ 64 , 0 ,	32, 32 });
	//walk.PushBack({128 , 0	32 , 32 });
	//walk.PushBack({ 256 , 0 ,32 , 32});
	//walk.PushBack({ 512 , 0 , 32 , 32});
	//add speed

	//JUMPING

	//HOOK


		
}

j1Player::~j1Player()
{}

bool j1Player::Start() {

	//load conditions
	//flip = false
	position.x = 20;
	position.y = 226;
	//Current_Animation = idle;// add animation module

	//load graphics
	normal_graphics = App->tex->Load("textures/walk.png");
	current_graphics = normal_graphics;
	//load sounds and collisions
}

bool j1Player::CleanUp() {
	//unload graphics
	App->tex->UnLoad(normal_graphics);
}

bool j1Player::PreUpdate() {
	//inputs
}

bool j1Player::Update()
{

}

bool j1Player::PostUpdate() {

}

//collisions

