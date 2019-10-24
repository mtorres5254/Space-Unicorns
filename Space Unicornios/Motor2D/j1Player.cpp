#include "j1App.h"
#include "j1Textures.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Collisions.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Audio.h"
#include "p2Log.h"
#include "SDL/include/SDL_timer.h"
//include SDL_timer.h

j1Player::j1Player() {

	//add animation pushbacks
	
	
	//IDLE
	idle.PushBack({ 0 , 0 ,76 , 76 });
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
	jumping.PushBack({8, 101, 18, 28});
	jumping.PushBack({30 , 101, 16 , 28});
	jumping.PushBack({ 49 , 101 , 19, 28 });
	jumping.speed = 1.0f;

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

bool j1Player::Awake(pugi::xml_node& conf) {
	return true;
}

bool j1Player::Start() {

	//load conditions
	//flip = false
	position.x = 0;//check positions 
	position.y = 0;
	App->render->camera.x = 0;
	App->render->camera.y = 0;
	Current_Animation = &idle;

	//load graphics
	graphics = App->tex->Load("textures/Spritesheet.png");
	if (graphics == NULL) {
		return false;
	}
	//load sounds and collisions
	col = App->col->AddCollider({ 0, 0, 76, 76 }, COLLIDER_PLAYER, this);
	col_prova = App->col->AddCollider({ 0, 100, 150, 25 }, COLLIDER_FLOOR, this);
	return true;
}

bool j1Player::CleanUp() {
	//unload graphics
	App->tex->UnLoad(graphics);
	return true;
}

bool j1Player::PreUpdate() {
	falling = true;
	flip = SDL_FLIP_NONE;
	return true;
}



bool j1Player::Update(float dt) { 
	inputs = GetInput();

	input inputtmp = GetLeftRight();

	switch (inputs)
	{
	case IN_NONE:
		Current_Animation = &idle;
		break;
	case IN_JUMP:
		Current_Animation = &jumping;
		//-----------
		break;
	case IN_FALLING:
		position.y = position.y + (int)(2 * dt);
		

		if (inputtmp == IN_LEFT) {
			position.x = position.x - 1;
		}
		if (inputtmp == IN_RIGHT) {
			position.x = position.x + 1;
		}

		break;
	case IN_JUMP_LEFT:


		break;
	case IN_JUMP_RIGHT:


		break;
	case IN_LEFT:
		Current_Animation = &walking;
		flip = SDL_FLIP_HORIZONTAL;
		//-----------
		position.x = position.x - 1 * (int)dt;
		break;
	case IN_RIGHT:
		Current_Animation = &walking;
		//-----------
		position.x = position.x + 1 * (int)dt;
		break;
	case IN_CROUCH:
		Current_Animation = &crouch;

		break;
	}

	col->SetPos(position.x, position.y);
	App->render->Blit(graphics, position.x, position.y, &(Current_Animation->GetCurrentFrame()), 1.0f, 0, 0, 0, flip);
	return true; 
}


bool j1Player::PostUpdate() {
	return true;
}

input j1Player::GetInput() {

	input in = IN_NONE;
	bool jump = false;
	bool left = false;
	bool right = false;
	bool crouch = false;
	bool special = false;

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
		jump = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_S) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
		crouch = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT){
		left = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		right = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_SPACE) == KEY_DOWN) {
		special = true;
	}
	else {
		in = IN_NONE;
	}


	if (jump == true && left == true) {
		in = IN_JUMP_LEFT;
	}
	if (jump == true && right == true) {
		in = IN_JUMP_RIGHT;
	}

	if (left == true) {
		in = IN_LEFT;
	}
	if (right == true) {
		in = IN_RIGHT;
	}
	if(left == true && right == true) {
		in = IN_NONE;
	}

	if (special == true) {
		in = IN_SPECIAL;
	}

	if (crouch == true) {
		in = IN_CROUCH;
	}
	
	if (falling == true) {
		in = IN_FALLING;
	}

	return in;
}

input j1Player::GetLeftRight() {
	input in = IN_NONE;

	bool left = false;
	bool right = false;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		left = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		right = true;
	}

	
	if (right == true) {
		in = IN_RIGHT;
	}
	if (left == true) {
		in = IN_LEFT;
	}
	if (left == true && right == true) {
		in = IN_NONE;
	}

	return in;
}

void j1Player::OnCollision(Collider* c1, Collider* c2) {
	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_FLOOR) {
		falling = false;
	}
}

