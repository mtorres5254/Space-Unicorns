#include "FlyEnemy.h"
#include "j1Collisions.h"
#include "j1Textures.h"
#include "j1Scene.h"
#include "j1Player.h"


j1FlyEnemy::j1FlyEnemy(iPoint pos) : Entity(EntityType::fly_enemy) {
	//load graphics
	sprite = App->tex->Load("textures/flying_enemy.png");

	//Load animations
	idle.PushBack({ 4, 7 , 31 , 26 });
	idle.PushBack({ 90 , 8 , 28, 27 });
	idle.PushBack({ 90 , 8 , 28 , 28 });
	idle.PushBack({ 131 , 6 , 30 , 28 });
	idle.PushBack({ 6 , 49 , 27 , 28 });
	idle.PushBack({ 48 , 49 , 27 , 28 });
	idle.PushBack({ 89 , 49 , 31 ,27 });
	idle.PushBack({ 133 , 49 , 28 , 27 });
	idle.PushBack({ 49 , 90 , 28 , 29 });
	idle.speed = 5.0f;

	hit.PushBack({6,134,29,26});
	hit.PushBack({47,133,28,28});
	hit.PushBack({89,133,29,27});
	hit.PushBack({131,133,29,26});
	hit.speed = 10.0f;
	hit.loop = false;

	death.PushBack({4,175,28,27});
	death.PushBack({46,175,28,27 });
	death.PushBack({90,175,28,27 });
	death.speed = 3.0f;
	death.loop = false;

	//save positions
	initial_position = position = pos;

	//save lives
	lives = maxLives = 8;

	//add collider
	col = App->col->AddCollider({ position.x, position.y, 28,28 }, COLLIDER_ENEMY, App->entity);
}

j1FlyEnemy::~j1FlyEnemy(){}

void j1FlyEnemy::Reset() {
	position = initial_position;
	vel.x = 0;
	vel.y = 0;
	lives = maxLives;
	if (col != nullptr) {
		App->col->DeleteCollider(col);
	}
	dead = false;
	falling = false;
	col = App->col->AddCollider({ position.x,position.y,32,32 }, COLLIDER_ENEMY, App->entity);
}

void j1FlyEnemy::Update(float dt) {
	BROFILER_CATEGORY("FlyEnemy_Update", Profiler::Color::Magenta)
	if (dead == false) {
		private_dt = dt;
		if (lives == 0) {
			dead = true;
			falling = true;
		}

		HandeInput();

		position.x += vel.x * dt;
		position.y += vel.y * dt;

		if (position.x > App->scene->player->position.x) {
			flip = SDL_FLIP_HORIZONTAL;
		}
		else if (position.x < App->scene->player->position.x) {
			flip = SDL_FLIP_NONE;
		}

		Draw();
		col->SetPos(position.x, position.y);
	}
	else if (dead == true) {
		if (falling == true) {
			vel.y = 100;
			position.y += vel.y * dt;
			if (col != nullptr) {
				col->SetPos(position.x, position.y);
			}
		}
		Current_animation = &death;
		App->render->Blit(sprite, position.x, position.y, &Current_animation->GetCurrentFrame(), 1.0f, NULL, NULL, NULL, flip);
	}
}

void j1FlyEnemy::Draw() {
	Current_animation = &idle;
	if (hitted == true) {
		Current_animation = &hit;
		if (hit.Finished() == true) {
			hitted = false;
			hit.Reset();
		}
	}

	App->render->Blit(sprite, position.x, position.y, &Current_animation->GetCurrentFrame(), 1.0f, NULL, NULL, NULL, flip);
}

void j1FlyEnemy::HandeInput() {
	vel.x = vel.y = 0;
}

void j1FlyEnemy::OnCollision(Collider* c1, Collider * c2) {
	if (c2->type == COLLIDER_SHOT) {
		if (lives != 0) {
			lives -= 1;
		}
		hitted = true;
		if (c2->rect.x + c2->rect.w < c1->rect.x + c1->rect.w / 2) {
			vel.x = 200;
			position.x += vel.x * private_dt;
		}
		else if (c2->rect.x > c1->rect.x + c1->rect.w / 2) {
			vel.x = -200;
			position.x += vel.x * private_dt;
		}

		if (c2->rect.y > c1->rect.y + c1->rect.h / 2) {
			vel.y = -200;
			position.y += vel.y * private_dt;
		}
		else if (c2->rect.y + c2->rect.h < c1->rect.y + c1->rect.h / 2) {
			vel.y = 200;
			position.y += vel.y * private_dt;
		}
	}

	if (c2->type == COLLIDER_FLOOR) {
		falling = false;
		vel.x = vel.y = 0;
		App->col->DeleteCollider(col);
	}
}