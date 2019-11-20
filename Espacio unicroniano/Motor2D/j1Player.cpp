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

j1Player::j1Player(iPoint pos) : Entity(EntityType::player) {
	
	//IDLE
	idle.PushBack({ 2,2,37,74 });
	idle.PushBack({ 42,4,34,72 });
	idle.PushBack({ 82 , 4 , 33 , 72});
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
	walking.speed = 0.24f;

	//CROUCH
	crouching.PushBack({ 208, 32, 43, 44 });
	crouching.loop = false;

	//JUMPING
	jumping.PushBack({ 7,154,51,71 });
	jumping.PushBack({ 60,169,53,39 });
	jumping.PushBack({ 117,175,52,30 });
	jumping.speed = 0.2f;
	jumping.loop = false;

	//fall
	fall.PushBack({ 440,165,53,39 });
	fall.PushBack({ 182,156,30,52 });
	fall.PushBack({ 234,156,52,29 });
	fall.PushBack({ 307,156,30,52 });
	fall.PushBack({ 361,165,53,39 });
	fall.speed = 0.24f;

	//dead;
	death.PushBack({2,286,46,45});
	death.PushBack({ 60,281,37,63 });
	death.speed = 0.05f;
	death.loop = false;

	//load conditions
	Current_Animation = &idle;

	//load graphics
	sprite = App->tex->Load("textures/player.png");
	if (sprite == NULL) {
		LOG("Error loading graphics");
	}
	weapon_pointer = App->tex->Load("textures/mira.png");
	if (weapon_pointer == NULL) {
		LOG("Error loading graphics");
	}

	//load sounds and collisions
	jumpingsound = App->audio->LoadFx("audio/fx/jump.wav");

	//set player info 
	initial_position.x = position.x = pos.x;
	initial_position.y = position.y = pos.y;
	lives = 1;

	//loading collider
	col = App->col->AddCollider({ position.x, position.y, 40, 80 }, COLLIDER_PLAYER, App->entity);
}

j1Player::~j1Player()
{
	//unload graphics
	App->tex->UnLoad(sprite);
	App->audio->StopFx();
	App->col->DeleteCollider(col);
}

void j1Player::PreUpdate(float dt) {
	BROFILER_CATEGORY("Player_PreUpdate", Profiler::Color::LightSkyBlue)

	falling = true;
}

void j1Player::Update(float dt) { 
	BROFILER_CATEGORY("Player_Update", Profiler::Color::DarkOrange )

	if (lives == 0) {
		states = A_DEAD;
		vel.x = vel.y = 0;
		//dead sound & effects 

		death_timer.Start();
	}

	if (states != A_DEAD) {
		HandeInput();

		position.x += vel.x * dt;
		position.y += vel.y * dt;

		col->SetPos(position.x, position.y);
	}
	else {
		if ((int)death_timer.ReadSec() == 2) {
			lives = 1;
			position.x = initial_position.x;
			position.y = initial_position.y;
			col->SetPos(position.x, position.y);

			App->render->camera.x = App->render->camera.y = 0;

			states = A_IDLE;
		}
	}

	Draw();
}

void j1Player::Draw() {
	Current_Animation = &idle;

	flip = SDL_FLIP_NONE;

	switch (states)
	{
	case A_IDLE:
		Current_Animation = &idle;
		break;
	case A_WALK_FORWARD:
		Current_Animation = &walking;
		break;
	case A_WALK_BACKWARDS:
		Current_Animation = &walking;
		flip = SDL_FLIP_HORIZONTAL;
		break;
	case A_JUMP_NEUTRAL:
		if (jumping.Finished() == false) {
			Current_Animation = &jumping;
		}
		else {
			states = A_FALLING;
		}

		if (vel.x < 0) {
			flip = SDL_FLIP_HORIZONTAL;
		}
		break;
	case A_CROUCH:
		Current_Animation = &crouching;
		break;
	case A_FALLING:
		Current_Animation = &fall;
		break;
	case A_DEAD:
		Current_Animation = &death;
		break;
	}


	App->render->Blit(sprite, position.x, position.y, &Current_Animation->GetCurrentFrame(), 1.0f, NULL, NULL, NULL, flip);

	DrawPointer();
}

void j1Player::DrawPointer() {
	int x;
	int y;

	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);

	App->render->Blit(weapon_pointer, p.x - 16, p.y - 16);
}

void j1Player::HandeInput() {
	//reset velocity x
	vel.x = 0;

	if (states == A_JUMP_NEUTRAL || states == A_FALLING ) {
		states = A_JUMP_NEUTRAL;
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			vel.x = (SPEED * 0.6);
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
				vel.x = (-1)*(SPEED * 0.6);
		}
		if (has_jump == false) {
			if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
				vel.y = -250;
				has_jump = true;
			}
		}
	}
	else {
		states = A_IDLE;

		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
			vel.x = SPEED;
			states = A_WALK_FORWARD;
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
			vel.x = (-1)*SPEED;
			states = A_WALK_BACKWARDS;
		}

		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
			vel.y = -300;
			states = A_JUMP_NEUTRAL;
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT) {
			states = A_CROUCH;
		}
	}

	if (falling == true) {
		states = A_FALLING;
	}

	if (vel.y < maxFallVel) {
		vel.y += (float)(10);
	}
}

void j1Player::OnCollision(Collider* c1, Collider* c2) {
	if (c2->type == COLLIDER_FLOOR) {
		//reset jumping conditions
		states = A_IDLE;
		jumping.Reset();
		has_jump = false;
		falling = false;

		LOG("%i", vel.y);

		if (vel.y > 0) {
			vel.y = 0;
		}
		if (c1->rect.y + c1->rect.h / 2 < c2->rect.y) {
			if (c2->rect.y > c1->rect.y) {
				if (c1->rect.y > c2->rect.y + c2->rect.h - 3) {
					if (c1->rect.x > c2->rect.x + c2->rect.w && c1->rect.x + c1->rect.w / 2 < c2->rect.x) {
						position.x = c1->rect.x - c2->rect.w;
					}
					if (c1->rect.x + c1->rect.w < c2->rect.x && c1->rect.x + c1->rect.w / 2 > c2->rect.x) {
						position.x = c1->rect.x + c1->rect.w;
					}
				}
			}
		}
	}

	if (c2->type == COLLIDER_WALL) {

		if (c1->rect.y > c2->rect.y + c2->rect.h - 10) {
			if (vel.y < 0) {
				vel.y = 0;
			}
		}
		else
		{
			if (c1->rect.x + c1->rect.w < c2->rect.x + c2->rect.w / 2) {
				position.x = c2->rect.x - c1->rect.w;
			}
			if (c1->rect.x > c2->rect.x + c2->rect.w / 2) {
				position.x = c2->rect.x + c2->rect.w;
			}
		}
	}

	if (c2->type == COLLIDER_DEAD) {
		lives -= 1;
		//add effects and other things
	}
}
