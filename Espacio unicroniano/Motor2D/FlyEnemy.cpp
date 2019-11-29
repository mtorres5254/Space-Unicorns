#include "FlyEnemy.h"
#include "j1Collisions.h"
#include "j1Textures.h"
#include "j1Pathfinding.h"
#include "j1Scene.h"
#include "j1Player.h"

#define SPEED 100


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

void j1FlyEnemy::Load(pugi::xml_node& load) {
	lives = load.attribute("lives").as_int();

	position.x = load.child("position").attribute("x").as_int();
	position.y = load.child("position").attribute("y").as_int() - 300;

	initial_position.x = load.child("initial_position").attribute("x").as_int();
	initial_position.y = load.child("initial_position").attribute("y").as_int();

	vel.x = vel.y = 0;
}

void j1FlyEnemy::Save(pugi::xml_node& save) const {
	save.append_attribute("lives");
	save.attribute("lives").set_value(lives);

	save.append_child("position");
	save.child("position").append_attribute("x");
	save.child("position").append_attribute("y");
	save.child("position").attribute("x").set_value(position.x);
	save.child("position").attribute("y").set_value(position.y);

	save.append_child("initial_position");
	save.child("initial_position").append_attribute("x");
	save.child("initial_position").append_attribute("y");
	save.child("initial_position").attribute("x").set_value(initial_position.x);
	save.child("initial_position").attribute("y").set_value(initial_position.y);
}

void j1FlyEnemy::Reset() {
	position = initial_position;
	vel.x = 0;
	vel.y = 0;
	lives = maxLives;
	if (col != nullptr) {
		App->col->DeleteColliderNow(col);
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

		switch (CurrentState)
		{
		case j1FlyEnemy::NONE:
			//nothing
			break;
		case j1FlyEnemy::MOVE:
			position.x += vel.x * dt;
			position.y += vel.y * dt;
			break;
		default:
			break;
		}

		

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
	CurrentState = NONE;

	if (ChasePlayer(App->scene->player->position) == true) {
		App->pathfinding->CreatePath(App->map->WorldToMap(position.x, position.y), App->map->WorldToMap(App->scene->player->position.x, App->scene->player->position.y), true);

		const p2DynArray<Path>* path = App->pathfinding->GetLastPath();

		const Path* path_dir = path->At(1);
		if (path_dir != nullptr) {

			switch (path_dir->dir)
			{
			case DIR_UP:
				vel.y = SPEED;
				CurrentState = MOVE;
				break;
			case DIR_DOWN:
				vel.y = -SPEED;
				CurrentState = MOVE;
				break;
			case DIR_LEFT:
				vel.x = -SPEED;
				CurrentState = MOVE;
				break;
			case DIR_RIGHT:
				vel.x = SPEED;
				CurrentState = MOVE;
				break;
			}
		}
	}
}

bool j1FlyEnemy::ChasePlayer(iPoint player) {
	if (player.x + 16 > position.x - 500 && player.x + 16 < position.x + 500) {
		if (player.y > position.y - 200 && player.y < position.y + 300) {
			return true;
		}
	}

	return false;
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