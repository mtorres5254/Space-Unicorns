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

	

	idle.PushBack({ 19 , 0 , 31 , 31 });
	takenanim.PushBack({ 61 , 0 , 31 , 31 });
	
	idle.speed = 1.0f;
	
	takenanim.PushBack({ 61 , 0 , 31 , 31 });
	takenanim.PushBack({ 19 , 0 , 31 , 31 });
	takenanim.PushBack({ 61 , 0 , 31 , 31 });
	takenanim.speed = 1.0f;


	
	
	

	

	col = App->col->AddCollider({ position.x,position.y,31,31 }, COLLIDER_COIN, App->entity);

	
	CoinPosition = position = pos;

	lives = 1;
}

Coin::~Coin() {
	this->col->to_delete = true;
	App->tex->UnLoad(this->sprite);
	taken = true;
}

void Coin::PreUpdate(float dt) {
	private_dt = dt;
}


void Coin::Reset() {
	
	
	position.x = CoinPosition.x;
	position.y = CoinPosition.y - 5;
	lives = 1;
	//App->entity->DestroyEntity(this);
	App->col->DeleteColliderNow(col);

	if (col != nullptr) {
		App->col->DeleteColliderNow(col);
	}
	
	col = App->col->AddCollider({ position.x,position.y,22,22 }, COLLIDER_COIN, App->entity);
	taken = false;
}

void Coin::Update(float dt) {
	BROFILER_CATEGORY("Coins_Update", Profiler::Color::DarkBlue)
		if (taken == false) {
			if (lives == 0) {
				taken = true;
				App->col->DeleteColliderNow(col);
			}
			Draw();
			col->SetPos(position.x, position.y);
		}
		else if (taken = true) {
			Current_animation = &takenanim;
			App->render->Blit(sprite, position.x, position.y, &Current_animation->GetCurrentFrame(), 1.0f, NULL, NULL, NULL, flip);



		}
}




void Coin::Draw() {

	Current_animation = &idle;
	
	App->render->Blit(sprite, position.x, position.y, &Current_animation->GetCurrentFrame(), 1.0f, NULL, NULL, NULL, flip);
}

void Coin::OnCollision(Collider* c1, Collider *c2) {
	
		if (c1->type == COLLIDER_PLAYER) {
			if (lives = 1) {
				lives = 0;
			}

			coinCnt += 1;
			
			Coin::~Coin();
			

		}



	
}
