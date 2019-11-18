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
	walking.speed = 0.12f;

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
	//dead;
	death.PushBack({});
	death.speed = 0.1f;

	//load conditions
	Current_Animation = &idle;

	//load graphics
	sprite = App->tex->Load("textures/player.png");
	if (sprite == NULL) {
		LOG("Error loading graphics");
	}
	//load sounds and collisions
	jumpingsound = App->audio->LoadFx("audio/fx/jump.wav");

	bool godmode = false;
	bool jump = false;
	bool left = false;
	bool right = false;
	bool crouch = false;
	bool special = false;
	bool died = false;

	bool has_jump = false;
	bool has_col = true;
	bool falling = true;

	col = App->col->AddCollider({ position.x, position.y, 25, 75 }, COLLIDER_PLAYER, App->entity);
}

j1Player::~j1Player()
{
	//unload graphics
	App->tex->UnLoad(sprite);
	App->audio->StopFx();
	App->col->DeleteCollider(col);
}

void j1Player::Update(float dt) { 
	HandeInput();

	position.x += (vel.x * dt);
	position.y += (vel.y * dt);

	col->SetPos(position.x, position.y);

	Draw();
}

void j1Player::Draw() {
	Current_Animation = &idle;

	App->render->Blit(sprite, position.x, position.y, &Current_Animation->GetCurrentFrame(), 1.0f, NULL, NULL, NULL, SDL_FLIP_NONE);
}

void j1Player::HandeInput() {
	//reset velocity x
	vel.x = 0;

	if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT) {
		vel.x = SPEED;
	}
	if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT) {
		vel.x = (-1)*SPEED;
	}

	if (App->input->GetKey(SDL_SCANCODE_W) == KEY_DOWN) {
		vel.y = -500;
	}
	vel.y += (float)9.8f;
}

void j1Player::OnCollision(Collider* c1, Collider* c2) {
	if (c2->type == COLLIDER_FLOOR) {
		if (vel.y > 0) {
			vel.y = 0;
		}
		position.y = c2->rect.y - c1->rect.h + 1;
	}
}
