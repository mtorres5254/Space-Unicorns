#include "j1FloorEnemy.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1App.h"

j1FloorEnemy::j1FloorEnemy(iPoint pos) : Entity(EntityType::floor_enemy) {
	//Load Sprite
	sprite = App->tex->Load("textures/alien-enemy.png");

	//Load Animations
	idle.PushBack({ 0,33,34,30 });
	idle.PushBack({ 43,34,39,29 });
	idle.PushBack({ 88,35,42,28 });
	idle.PushBack({ 139,34,39,29 });
	idle.speed = 5.0f;

	//Load position and save it
	initialPosition = position = pos;
}

j1FloorEnemy::~j1FloorEnemy(){}

void j1FloorEnemy::PreUpdate(float dt) {

}

void j1FloorEnemy::Update(float dt) {


	Draw();
}

void j1FloorEnemy::HandeInput() {

}

void j1FloorEnemy::Draw() {
	Current_animation = &idle;
	SDL_RendererFlip flip = SDL_FLIP_NONE;

	App->render->Blit(sprite, position.x, position.y, &Current_animation->GetCurrentFrame(), 1.0f, NULL, NULL, NULL, flip);
}

void j1FloorEnemy::OnCollision(Collider* c1, Collider *c2) {

}