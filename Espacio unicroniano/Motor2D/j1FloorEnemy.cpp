#include "j1FloorEnemy.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Player.h"
#include "j1Scene.h"
#include "j1Collisions.h"
#include "j1App.h"

j1FloorEnemy::j1FloorEnemy(iPoint pos) : Entity(EntityType::floor_enemy) {
	//Load Sprite
	sprite = App->tex->Load("textures/ground_enemy.png");

	//Load Animations
	idle.PushBack({ 141 , 5 , 31 , 27 });
	idle.PushBack({ 208 , 5 , 31 , 27 });
	idle.PushBack({ 143 , 37 , 33 , 27 });
	idle.PushBack({ 210 , 37 , 28 , 27 });
	idle.PushBack({ 78 , 70 , 35 , 26 });
	idle.PushBack({ 148 , 68 , 23 , 28 });
	idle.PushBack({ 211 , 68 , 25 , 28 });
	idle.speed = 5.0f;

	death.PushBack({ 15 , 4 , 32 , 28 });
	death.PushBack({ 10 , 34 , 41 , 30 });
	death.PushBack({ 9 , 68 , 49, 29 });
	death.PushBack({ 5 , 97 , 51 , 31 });
	death.PushBack({ 73,97,52,31 });
	death.speed = 3.0f;
	death.loop = false;

	col = App->col->AddCollider({ position.x,position.y,32,32 }, COLLIDER_ENEMY, App->entity);

	//Load position and save it
	initialPosition = position = pos;

	lives = 5;
}

j1FloorEnemy::~j1FloorEnemy(){}

void j1FloorEnemy::PreUpdate(float dt) {
	private_dt = dt;
}

void j1FloorEnemy::Reset() {
	position.x = initialPosition.x;
	position.y = initialPosition.y - 5;
	lives = 5;
	if (col != nullptr) {
		App->col->DeleteCollider(col);
	}
	col = App->col->AddCollider({ position.x,position.y,32,32 }, COLLIDER_ENEMY, App->entity);
	dead = false;
}

void j1FloorEnemy::Update(float dt) {
	if (dead == false) {
		if (lives == 0) {
			dead = true;
			App->col->DeleteCollider(col);
		}

		HandeInput();

		if (position.x > App->scene->player->position.x) {
			flip = SDL_FLIP_HORIZONTAL;
		}
		else if (position.x < App->scene->player->position.x) {
			flip = SDL_FLIP_NONE;
		}

		position.x += vel.x * dt;
		position.y += vel.y * dt;

		Draw();
		col->SetPos(position.x, position.y);
		falling = true;
	}
	else if (dead == true) {
		Current_animation = &death;
		App->render->Blit(sprite, position.x, position.y, &Current_animation->GetCurrentFrame(), 1.0f, NULL, NULL, NULL, flip);
	}
}

void j1FloorEnemy::HandeInput() {
	vel.x = 0;

	if (falling == true) {
		vel.y += 10;
	}
}

void j1FloorEnemy::Draw() {
	Current_animation = &idle;

	App->render->Blit(sprite, position.x, position.y, &Current_animation->GetCurrentFrame(), 1.0f, NULL, NULL, NULL, flip);
}

void j1FloorEnemy::OnCollision(Collider* c1, Collider *c2) {
	if (c2->type == COLLIDER_SHOT) {
		lives -= 1;
		if (c2->rect.x + c2->rect.w < c1->rect.x + c1->rect.w / 2) {
			vel.x = 200;
			position.x += vel.x * private_dt;
		}
		else if (c2->rect.x > c1->rect.x + c1->rect.w / 2) {
			vel.x = -200;
			position.x += vel.x * private_dt;
		}
	}
	if (c2->type == COLLIDER_FLOOR) {
		if (vel.y > 0) {
			vel.y = 0;
		}
		falling = false;
	}
}