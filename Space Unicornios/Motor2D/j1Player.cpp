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
	walking.speed = 6.0f; //no son sprites corrent per tant camina tot puesto com la policia nacional

	//JUMPING
	/*jumping.PushBack({8, 101, 18, 28});
	jumping.PushBack({30 , 101, 16 , 28});
	jumping.PushBack({ 49 , 101 , 19, 28 });
		jumping.speed = 1.0f;*/

	//HOOK

	//DEATH
	death.PushBack({32, 0 , 32 , 32});
	death.PushBack({32, 32, 32, 32});
	death.PushBack({32, 64, 32 , 32});
	death.PushBack({32, 128 , 32 , 32});
	death.PushBack({32, 256, 32 , 32 });
	death.PushBack({32 , 512, 32 , 32 });
	death.speed = 0.3f;


		
}

j1Player::~j1Player()
{}

bool j1Player::Start() {

	//load conditions
	//flip = false
	position.x = 50;//check positions 
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
	if (whileair = false && ducking = false) {

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
			
			position.y += player_speed;
			jumping = true;
		if ((App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) && !not_forward && !jumping) {
			position.x += player_speed;
			Current_Animation.GetCurrentFrame() = walking.GetCurrentFrame();
			flip = SDL_FLIP_NONE;
		}
		if ((App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) && !not_backwards &&!jumping) {
			position.x -= player_speed;
			Current_Animation.GetCurrentFrame() = walking.GetCurrentFrame();
			flip = SDL_FLIP_HORIZONTAL;
		}
		if ((App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT && !jumping) {
			Current_Animation-GetCurrentFrame() = duck.GetCurrentFrame();
		}
	
	}
	//Fix Jumping//Add gravity
	//if (jumping = true) {} Add gravity. Allow player to move while its in the air

	// Add gravity make movement while player is in air/falling

	//flip

	//special move


	//input fails
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

