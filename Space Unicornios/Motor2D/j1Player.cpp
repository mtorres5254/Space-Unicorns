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
	idle.PushBack({2 , 2 , 37 , 74});
	idle.PushBack({42 , 4 , 34 , 72});
	idle.PushBack({82 , 4 , 33 , 72});
	idle.PushBack({123 , 6 , 29 , 70});
	idle.PushBack({162 , 2 , 37 , 74});
	idle.speed = 0.05f;

	//WALKING
	walking.PushBack({ 13,87,39,64 });
	walking.PushBack({ 74,84,41,67 });
	walking.PushBack({ 125,82,48,67 });
	walking.PushBack({ 186,84,59,61 });
	walking.PushBack({ 253,86,46,65 });
	walking.PushBack({ 324,87,29,64 });
	walking.PushBack({ 381,84,34,67 });
	walking.PushBack({ 435,82,47,67 });
	walking.PushBack({ 495,84,55,61 });
	walking.PushBack({ 560,86,41,65 });
	walking.speed = 0.12f; //no son sprites corrent per tant camina tot puesto com la policia nacional

	//CROUCH
	crouching.PushBack({ 208, 32, 43, 44 });
	crouching.loop = false;

	//JUMPING
	jumping.PushBack({ 7,154,51,71 });
	jumping.PushBack({ 60,169,53,39 });
	jumping.PushBack({ 117,175,52,30 });
	jumping.speed = 0.1f;
	jumping.loop = false;

	//fall
	fall.PushBack({ 440,165,53,39 });
	fall.PushBack({ 182,156,30,52 });
	fall.PushBack({ 234,156,52,29 });
	fall.PushBack({ 307,156,30,52 });
	fall.PushBack({ 361,165,53,39 });
	fall.speed = 0.1f;

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
	graphics = App->tex->Load("textures/player.png");
	if (graphics == NULL) {
		return false;
	}
	//load sounds and collisions
	jumpingsound = App->audio->LoadFx("audio/fx/jump.wav");
	col = App->col->AddCollider({ position.x, position.y, 37, 80 }, COLLIDER_PLAYER, this);
	return true;
}

bool j1Player::CleanUp() {
	//unload graphics
	App->tex->UnLoad(graphics);
	return true;
}

bool j1Player::PreUpdate() {
	flip = SDL_FLIP_NONE;
	if (position.x < 0) {
		position.x = 0;
	}
	return true;
}



bool j1Player::Update(float dt) { 
	if (inputs != IN_JUMP && inputs != IN_JUMP_LEFT && inputs != IN_JUMP_RIGHT) {
		maxjump = 0;
		inputs = GetInput();
	}

	input inputtmp = GetLeftRight();

	switch (inputs)
	{
	case IN_NONE:
		Current_Animation = &idle;
		break;
	case IN_JUMP:
		if (jumping.Finished() == true) {
			Current_Animation = &fall;
		}
		else {
			Current_Animation = &jumping;
			App->audio->PlayFx(jumpingsound, 0);
		}
		//-----------
		if (maxjump != JUMP) {
			if (maxjump >= 0 && maxjump < JUMP / 2) {
				position.y = position.y - JUMP_Y_SPEED;
				maxjump++;
			}
			else if (maxjump >= JUMP / 2 && maxjump < JUMP - (JUMP / 3)) {
				position.y = position.y - JUMP_Y_SPEED / 2;
				maxjump++;
			}
			else {
				position.y = position.y - JUMP_Y_SPEED / 4;
				maxjump++;
			}
		}
		else {
			inputs = IN_FALLING;
			jumping.Reset();
		}
		//------------
		if (inputtmp == IN_LEFT) {
			position.x = position.x - JUMP_SPEED;
		}
		if (inputtmp == IN_RIGHT) {
			position.x = position.x + JUMP_SPEED;
		}
		
		break;
	case IN_FALLING:
		Current_Animation = &fall;
		position.y = position.y + (int)(3 * dt);

		if (inputtmp == IN_LEFT) {
			position.x = position.x - JUMP_SPEED;
		}
		if (inputtmp == IN_RIGHT) {
			position.x = position.x + JUMP_SPEED;
		}

		break;
	case IN_JUMP_LEFT:
		if (jumping.Finished() == true) {
			Current_Animation = &fall;
		}
		else {
			Current_Animation = &jumping;
		}
		//-----------
		if (maxjump != JUMP) {
			if (maxjump >= 0 && maxjump < JUMP / 2) {
				position.y = position.y - JUMP_Y_SPEED;
				maxjump++;
			}
			else if (maxjump >= JUMP / 2 && maxjump < JUMP - (JUMP / 3)) {
				position.y = position.y - JUMP_Y_SPEED / 2;
				maxjump++;
			}
			else {
				position.y = position.y - JUMP_Y_SPEED / 4;
				maxjump++;
			}
		}
		else {
			inputs = IN_FALLING;
			jumping.Reset();
		}
		//------------
		if (inputtmp == IN_LEFT) {
			position.x = position.x - JUMP_SPEED;
		}
		if (inputtmp == IN_RIGHT) {
			position.x = position.x + JUMP_SPEED;
		}
		break;
	case IN_JUMP_RIGHT:
		if (jumping.Finished() == true) {
			Current_Animation = &fall;
		}
		else {
			Current_Animation = &jumping;
			
		}
		//-----------
		if (maxjump != JUMP) {
			if (maxjump >= 0 && maxjump < JUMP / 2) {
				position.y = position.y - JUMP_Y_SPEED;
				maxjump++;
			}
			else if (maxjump >= JUMP / 2 && maxjump < JUMP - (JUMP / 3)) {
				position.y = position.y - JUMP_Y_SPEED / 2;
				maxjump++;
			}
			else {
				position.y = position.y - JUMP_Y_SPEED / 4;
				maxjump++;
			}
		}
		else {
			inputs = IN_FALLING;
			jumping.Reset();
		}
		//------------
		if (inputtmp == IN_LEFT) {
			position.x = position.x - JUMP_SPEED;
		}
		if (inputtmp == IN_RIGHT) {
			position.x = position.x + JUMP_SPEED;
		}
		break;
	case IN_LEFT:
		Current_Animation = &walking;
		flip = SDL_FLIP_HORIZONTAL;
		//-----------
		position.x = position.x - SPEED;
		break;
	case IN_RIGHT:
		Current_Animation = &walking;
		//-----------
		position.x = position.x + SPEED;
		break;
	case IN_CROUCH:
		Current_Animation = &crouching;

		break;
	case IN_SPECIAL:
		Current_Animation = &special_anim;

		break;
	}

	//////DEATH/////
	//if (dead = true) 
	//reset scene
	

	col->SetPos(position.x, position.y);
	App->render->Blit(graphics, position.x, position.y, &(Current_Animation->GetCurrentFrame()), 1.0f, 0, 0, 0, flip);
	return true; 
}


bool j1Player::PostUpdate() {
	if (inputs != IN_JUMP && inputs != IN_JUMP_LEFT && inputs != IN_JUMP_RIGHT) {
		falling = true;
	}
	if (inputs == IN_JUMP && inputs == IN_JUMP_LEFT && inputs == IN_JUMP_RIGHT) {
		falling = false;
	}
	return true;
}

input j1Player::GetInput() {

	input in = IN_NONE;
	bool jump = false;
	bool left = false;
	bool right = false;
	bool crouch = false;
	bool special = false;

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
	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
		jump = true;
	}
	else {
		in = IN_NONE;
	}

	if (jump == true) {
		in = IN_JUMP;
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

	if (jump == true && left == true) {
		in = IN_JUMP_LEFT;
	}
	if (jump == true && right == true) {
		in = IN_JUMP_RIGHT;
	}
	
	if (falling == true) {
		in = IN_FALLING;
	}

	return in;
}

input j1Player::GetLeftRight() {
	input in = IN_NONE;

	bool left2 = false;
	bool right2 = false;

	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		left2 = true;
	}
	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_DOWN || App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		right2 = true;
	}

	
	if (right2 == true) {
		in = IN_RIGHT;
	}
	if (left2 == true) {
		in = IN_LEFT;
	}
	if (left2 == true && right2 == true) {
		in = IN_NONE;
	}

	return in;
}

void j1Player::OnCollision(Collider* c1, Collider* c2) {

	/*if (c1->type == COLLIDER_PLAYER) {
		if (c2->type == COLLIDER_FLOOR) {
			falling = false;
		}
		if (c2->type == COLLIDER_END) {
			App->player->ChangeLevel();
		}
		if (c2->type == COLLIDER_DEAD) {
			died = true;
		}
	}
}
	*/
	//collisions for walls, death and end





	if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_FLOOR) {
		falling = false;



	}
	/*if (c1->type == COLLIDER_PLAYER && c2->type == COLLIDER_END) {
		App->player->ChangeLevel();


	}*/
}


void j1Player::ChangeLevel()
{
	App->map->CleanUp();
	App->map->Load("mapa2.tmx");
	App->scene->ColliderEnd1 = true;
	App->scene->ChangeScene = true;
	//change position of the player to restart
	position.x = App->scene->initialposx;
	position.y = App->scene->initialposy;
}