#include "j1App.h"
#include "j1Entities.h"
#include "j1Player.h"
#include "j1FloorEnemy.h"
#include "FlyEnemy.h"
#include "j1Particles.h"
#include "j1Scene.h"
#include "j1Map.h"
#include "j1Collisions.h"

j1Entities::j1Entities() {
	name.create("entities");
}

bool j1Entities::Awake(pugi::xml_node& config) {
	bool ret = true;
	
	int cycles;

	cycles = config.attribute("cycles_in_second").as_int();

	update_ms_cycle = 1000.0f * (1 / (float)cycles);

	return ret;
}

bool j1Entities::Load(pugi::xml_node& load) {
	p2List_item<Entity*>* ent;
	for (ent = entities.start; ent; ent = ent->next) {
		App->col->DeleteColliderNow(ent->data->col);
	}
	DestroyAll();

	App->scene->player = (j1Player*)CreateEntity(Entity::EntityType::player, { 0,0 });
	App->scene->player->Load(load.child("Player"));

	pugi::xml_node node_floor;
	for (node_floor = load.child("Floor_Enemies").child("Floor_Enemy"); node_floor; node_floor = node_floor.next_sibling("Floor_Enemy")) {
		iPoint pos;
		pos.x = pos.y = 0;

		j1FloorEnemy* ret = (j1FloorEnemy*)CreateEntity(Entity::EntityType::floor_enemy, pos);
		ret->Load(node_floor);
		App->scene->FloorEnemies.add(ret);
	}

	return true;
}

bool j1Entities::Save(pugi::xml_node& save) const {
	save.append_child("Player");
	App->scene->player->Save(save.child("Player"));

	save.append_child("Floor_Enemies");
	p2List_item<j1FloorEnemy*>* floor;
	save.child("Floor_Enemies").append_child("Floor_Enemy");
	pugi::xml_node node_floor;
	node_floor = save.child("Floor_Enemies").child("Floor_Enemy");
	for (floor = App->scene->FloorEnemies.start; floor; floor = floor->next) {
		floor->data->Save(node_floor);
		if (floor->next != NULL) {
			save.child("Floor_Enemies").append_child("Floor_Enemy");
		}
		node_floor = node_floor.next_sibling("Floor_Enemy");
	}

	return true;
}

bool j1Entities::PreUpdate(float dt) {
	p2List_item<Entity*>* AuxEntity = entities.start;

	for (; AuxEntity != NULL; AuxEntity = AuxEntity->next) {
		AuxEntity->data->PreUpdate(dt);
	}

	return true;
}

bool j1Entities::Update(float dt) {
	/*
	accumulated_time += dt;

	if (accumulated_time >= update_ms_cycle) {
		do_logic = true;
	}*/

	UpdateEntities(dt, do_logic);
/*
	if (do_logic == true) {
		accumulated_time = 0.0f;
		do_logic = false;
	}*/

	return true;
}

void j1Entities::UpdateEntities(float dt, bool do_logic) {
	p2List_item<Entity*>* AuxEntity = entities.start;

	for (; AuxEntity != NULL; AuxEntity = AuxEntity->next) {
		AuxEntity->data->Update(dt);
	}
}

Entity* j1Entities::CreateEntity(Entity::EntityType type, iPoint pos, int dest_X, int dest_Y) {
	Entity* ret = nullptr;
	switch (type) {
	case Entity::EntityType::player:				ret = new j1Player(pos);			break;
	case Entity::EntityType::floor_enemy:			ret = new j1FloorEnemy(pos);		break;
	case Entity::EntityType::fly_enemy:				ret = new j1FlyEnemy(pos);			break;
	case Entity::EntityType::particle:				ret = new j1Particle(pos, dest_X, dest_Y); break;
	}

	if (ret != nullptr) {
		entities.add(ret);
	}

	return ret;
}

void j1Entities::ResetEntities() {
	p2List_item<Entity*>* AuxEntity = entities.start;

	for (; AuxEntity != NULL; AuxEntity = AuxEntity->next) {
		AuxEntity->data->Reset();
	}
}

void j1Entities::DestroyEntity(Entity* entity) {
	int entity_pos;
	p2List_item<Entity*>* AuxEntity;
	
	entity_pos = entities.find(entity);
	if (entity_pos != -1) {
		AuxEntity = entities.At(entity_pos);
		entities.del(AuxEntity);
	}
}

void j1Entities::DestroyAll() {
	entities.clear();
}

void j1Entities::OnCollision(Collider* c1, Collider* c2) {
	if (c1->type == COLLIDER_PLAYER) {
		App->scene->player->OnCollision( c1, c2);
	}

	if (c1->type == COLLIDER_SHOT) {
		p2List_item<j1Particle*>* particle;
		for (particle = App->scene->player->bullets.start; particle; particle = particle->next) {
			if (particle->data->col->rect.x == c1->rect.x && particle->data->col->rect.y == c1->rect.y && particle->data->col->rect.w == c1->rect.w && particle->data->col->rect.h == c1->rect.h) {
				particle->data->OnCollision(c1, c2);
			}
		}
		
	}

	if (c1->type == COLLIDER_ENEMY) {
		p2List_item<j1FloorEnemy*>* floor_enemy;
		for (floor_enemy = App->scene->FloorEnemies.start; floor_enemy; floor_enemy = floor_enemy->next) {
			if (floor_enemy->data->col->rect.x == c1->rect.x && floor_enemy->data->col->rect.y == c1->rect.y && floor_enemy->data->col->rect.w == c1->rect.w && floor_enemy->data->col->rect.h == c1->rect.h) {
				floor_enemy->data->OnCollision(c1, c2);
			}
		}

		p2List_item<j1FlyEnemy*>* fly_enemy;
		for (fly_enemy = App->scene->FlyEnemies.start; fly_enemy; fly_enemy = fly_enemy->next) {
			if (fly_enemy->data->col->rect.x == c1->rect.x && fly_enemy->data->col->rect.y == c1->rect.y && fly_enemy->data->col->rect.w == c1->rect.w && fly_enemy->data->col->rect.h == c1->rect.h) {
				fly_enemy->data->OnCollision(c1, c2);
			}
		}
	}
}

void j1Entities::LoadFromObjectLayer(ObjectLayer* layer) {
	p2List_item<ObjectData*>* obj;
	for (obj = layer->list.start; obj; obj = obj->next) {
		if (obj->data->name == "Player" && App->scene->player == nullptr) {
			iPoint pos;
			pos.x = obj->data->x;
			pos.y = obj->data->y;

			App->scene->player = (j1Player*) CreateEntity(Entity::EntityType::player, pos);
		}
		if (obj->data->name == "FloorEnemy") {
			iPoint pos;
			pos.x = obj->data->x;
			pos.y = obj->data->y;

			j1FloorEnemy* ret = (j1FloorEnemy*) CreateEntity(Entity::EntityType::floor_enemy, pos);
			App->scene->FloorEnemies.add(ret);
		}
		if (obj->data->name == "FlyEnemy") {
			iPoint pos;
			pos.x = obj->data->x;
			pos.y = obj->data->y;

			j1FlyEnemy* ret = (j1FlyEnemy*)CreateEntity(Entity::EntityType::fly_enemy, pos);
			App->scene->FlyEnemies.add(ret);
		}
	}
}