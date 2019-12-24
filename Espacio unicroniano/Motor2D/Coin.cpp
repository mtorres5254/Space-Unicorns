#include "j1Render.h"
#include "j1Textures.h"
#include "j1Player.h"
#include "j1Scene.h"
#include "j1Collisions.h"
#include "j1App.h"
#include"Coin.h"





Coin::Coin(iPoint pos) : Entity(EntityType::coin) {
	//Load Sprite
	sprite = App->tex->Load("textures/coins.png");

	idle.PushBack({ 18 , 0 , 465 , 464 });
	idle.speed = 0.1f;

	//Load Animations
	//idle.PushBack({ });
	
	

	

	col = App->col->AddCollider({ position.x,position.y,32,32 }, COLLIDER_COIN, App->entity);

	//Load position and save it
	CoinPosition = position = pos;

	lives = 1;
}

Coin::~Coin() {}

void Coin::PreUpdate(float dt) {
	private_dt = dt;
}


void Coin::Reset() {
	position.x = CoinPosition.x;
	position.y = CoinPosition.y - 5;
	
	
	if (col != nullptr) {
		App->col->DeleteColliderNow(col);
	}
	col = App->col->AddCollider({ position.x,position.y,32,32 }, COLLIDER_COIN, App->entity);
	taken = false;
}

void Coin::Update(float dt) {
	BROFILER_CATEGORY("FloorEnemy_Update", Profiler::Color::FloralWhite)

		if (taken == false) {
			if (lives == 0) {
				taken = true;
				App->col->DeleteCollider(col);
			}

			//HandeInput();

		
			

			

			Draw();
			col->SetPos(position.x, position.y);
			falling = true;
			
		}
		else if (taken == true) {
		
			App->render->Blit(sprite, position.x, position.y, &Current_animation->GetCurrentFrame(), 1.0f, NULL, NULL, NULL, flip);
			coinCnt += 1;
		}
}




void Coin::Draw() {

	Current_animation = &idle;
	
	App->render->Blit(sprite, position.x, position.y, &Current_animation->GetCurrentFrame(), 1.0f, NULL, NULL, NULL, flip);
}

void Coin::OnCollision(Collider* c1, Collider *c2) {
	if (c2->type == COLLIDER_PLAYER) {
		if (lives != 0) {
			lives -= 1;
			
		}



	}
}