#include "j1FloorEnemy.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Player.h"
#include "j1Scene.h"
#include "j1Collisions.h"
#include "j1Pathfinding.h"
#include "j1App.h"

#define SPEED 100


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
	death.speed = 8.0f;
	death.loop = false;

	hit.PushBack({83,5,31,27});
	hit.PushBack({ 82,37,33,27 });
	hit.speed = 4.0f;
	hit.loop = false;

	col = App->col->AddCollider({ position.x,position.y,32,32 }, COLLIDER_ENEMY, App->entity);

	//Load position and save it
	initialPosition = position = pos;

	lives = 3;
}

j1FloorEnemy::~j1FloorEnemy(){}

void j1FloorEnemy::PreUpdate(float dt) {
	private_dt = dt;
}

void j1FloorEnemy::Load(pugi::xml_node& load) {
	lives = load.attribute("lives").as_int();

	position.x = load.child("position").attribute("x").as_int();
	position.y = load.child("position").attribute("y").as_int() - 300;

	initialPosition.x = load.child("initial_position").attribute("x").as_int();
	initialPosition.y = load.child("initial_position").attribute("y").as_int();

	vel.x = vel.y = 0;
}

void j1FloorEnemy::Save(pugi::xml_node& save) const {
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
	save.child("initial_position").attribute("x").set_value(initialPosition.x);
	save.child("initial_position").attribute("y").set_value(initialPosition.y);
}

void j1FloorEnemy::Reset() {
	position.x = initialPosition.x;
	position.y = initialPosition.y - 5;
	vel.x = 0;
	vel.y = 0;
	lives = 5;
	if (col != nullptr) {
		App->col->DeleteColliderNow(col);
	}
	col = App->col->AddCollider({ position.x,position.y,32,32 }, COLLIDER_ENEMY, App->entity);
	dead = false;
}

void j1FloorEnemy::Update(float dt) {
	BROFILER_CATEGORY("FloorEnemy_Update", Profiler::Color::FloralWhite)

	if (dead == false) {
		if (lives == 0) {
			dead = true;
			App->col->DeleteCollider(col);
		}

		HandeInput();

		switch (CurrentState)
		{
		case j1FloorEnemy::NONE:
			//nothing
			break;
		case j1FloorEnemy::MOVE:
			position.x += vel.x * dt;
			break;
		default:
			break;
		}

		position.y += vel.y * dt;

		if (position.x > App->scene->player->position.x) {
			flip = SDL_FLIP_HORIZONTAL;
		}
		else if (position.x < App->scene->player->position.x) {
			flip = SDL_FLIP_NONE;
		}

		Draw();
		col->SetPos(position.x, position.y);
		falling = true;
	}
	else if (dead == true) {
		Current_animation = &death;
		App->render->Blit(sprite, position.x, position.y, &Current_animation->GetCurrentFrame(), 1.0f, NULL, NULL, NULL, flip);
	}
}

bool j1FloorEnemy::ChasePlayer(iPoint player) {
	if (player.x + 16 > position.x - 500 && player.x + 16 < position.x + 500) {
		if (player.y > position.y - 200 && player.y < position.y + 300) {
			return true;
		}
	}

	return false;
}

void j1FloorEnemy::HandeInput() {
	vel.x = 0;

	CurrentState = NONE;

	if (ChasePlayer(App->scene->player->position) == true) {
		App->pathfinding->CreatePath(App->map->WorldToMap(position.x, position.y), App->map->WorldToMap(App->scene->player->position.x, App->scene->player->position.y + App->map->data.tile_height + App->map->data.tile_height / 2), false);

		const p2DynArray<Path>* path = App->pathfinding->GetLastPath();

		const Path* path_dir = path->At(1);
		if (path_dir != nullptr) {

			switch (path_dir->dir)
			{
			case DIR_UP:
				//vel.y = SPEED;
				//CurrentState = MOVE;
				break;
			case DIR_DOWN:
				//vel.y = -SPEED;
				//CurrentState = MOVE;
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

	if (falling == true) {
		vel.y += 10;
	}
}

void j1FloorEnemy::Draw() {

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

void j1FloorEnemy::OnCollision(Collider* c1, Collider *c2) {
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
	}
	if (c2->type == COLLIDER_FLOOR) {
		if (vel.y > 0) {
			vel.y = 0;
		}
		falling = false;
	}
}