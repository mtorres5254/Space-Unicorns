#include "j1App.h"
#include "j1Entities.h"
#include "j1Player.h"
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

bool j1Entities::Update(float dt) {
	accumulated_time += dt;

	if (accumulated_time >= update_ms_cycle) {
		do_logic = true;
	}

	UpdateEntities(dt, do_logic);

	if (do_logic == true) {
		accumulated_time = 0.0f;
		do_logic = false;
	}

	return true;
}

void j1Entities::UpdateEntities(float dt, bool do_logic) {
	p2List_item<Entity*>* AuxEntity = entities.start;

	for (; AuxEntity != NULL; AuxEntity = AuxEntity->next) {
		AuxEntity->data->Update(dt);
	}
}

Entity* j1Entities::CreateEntity(Entity::EntityType type, iPoint pos) {
	Entity* ret = nullptr;
	switch (type) {
	case Entity::EntityType::player:				ret = new j1Player(pos);			break;
		case Entity::EntityType::floor_enemy:	/*ret = new floor eney()*/ break;
		case Entity::EntityType::fly_enemy:		/*ret = new fly enemy()*/ break;
	}

	if (ret != nullptr) {
		entities.add(ret);
	}

	return ret;
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

void j1Entities::OnCollision(Collider* c1, Collider* c2) {
	if (c1->type == COLLIDER_PLAYER) {
		
	}
}