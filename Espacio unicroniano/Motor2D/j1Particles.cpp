#include "j1Particles.h"
#include "j1Textures.h"
#include "j1Collisions.h"
#include "j1App.h"

#define SPEED 500

j1Particle::j1Particle(iPoint pos, int x, int y) : Entity(EntityType::particle) {
	sprite = App->tex->Load("textures/bullet.png");

	idle.PushBack({ 0,0,67,30 });
	idle.PushBack({ 82,0,68,30 });
	idle.speed = 7.0f;

	aux1 = y - pos.y;
	aux2 = x - pos.x;


	destination.x = x + 33;
	destination.y = y + 15;
	position = pos;

	col = App->col->AddCollider({ position.x,position.x,67,30 }, COLLIDER_SHOT, App->entity);

	lives = 1;
}

void j1Particle::Update(float dt) {
	if (position == destination) {
		lives = 0;
	}
	if (lives == 0) {
		App->entity->DestroyEntity(this);
		App->col->DeleteCollider(col);
	}

	if (aux2 > 0) {
		position.x += SPEED * dt;
		if (aux1 > 0) {
			float s = (float)aux1 / aux2;
			position.y += s * SPEED * dt;
		}
		else if (aux1 < 0) {
			int aux1_bis;
			aux1_bis = aux1 * -1;
			float s = (float)aux1_bis / aux2;
			position.y -= s * SPEED * dt;
		}
	}
	else if (aux2 < 0) {
		position.x -= SPEED * dt;
		if (aux1 > 0) {
			float s = (float)aux1 / aux2;
			position.y -= s * SPEED * dt;
		}
		else if (aux1 < 0) {
			int aux1_bis;
			aux1_bis = aux1 * -1;
			float s = (float)aux1_bis / aux2;
			position.y += s * SPEED * dt;
		}
	}
	
	col->SetPos(position.x, position.y);
	Draw();
}

void j1Particle::Draw() {
	float angle;
	if (aux2 > 0) {
		if (aux1 > 0) {
			float s = (float)aux1 / aux2;
			angle = s * 33;
		}
		else if (aux1 < 0) {
			int aux1_bis;
			aux1_bis = aux1 * -1;
			float s = (float)aux1_bis / aux2;
			angle = s * -33;
		}
	}
	else if (aux2 < 0) {
		if (aux1 > 0) {
			float s = (float)aux1 / aux2;
			angle = (s * 33) + 180;
		}
		else if (aux1 < 0) {
			int aux1_bis;
			aux1_bis = aux1 * -1;
			float s = (float)aux1_bis / aux2;
			angle = (s * -33) + 180;
		}
	}

	App->render->Blit(sprite, position.x, position.y, &idle.GetCurrentFrame(),1.0f,angle);
}

void j1Particle::OnCollision(Collider* c1, Collider*c2) {
	if (c2->type == COLLIDER_FLOOR || c2->type == COLLIDER_WALL || c2->type == COLLIDER_ENEMY) {
		lives = 0;
	}
} 