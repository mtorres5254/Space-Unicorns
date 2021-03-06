#ifndef __J1ENTITIES_H__
#define __J1ENTITIES_H__

#include "j1Module.h"
#include "p2Point.h"
#include "p2List.h"
#include "j1Render.h"

struct Collider;
struct ObjectLayer;

class Entity
{
public:
	enum class EntityType {
		player = 0,
		floor_enemy,
		fly_enemy,
		coin,
		particle,
		unknown
	};

	iPoint position;
	EntityType type;
	SDL_Texture* sprite;
	int lives;

	Collider* col;

	//-------------------
	Entity::Entity(EntityType type) : type(type)
	{}

	virtual void PreUpdate(float dt) {}
	virtual void Update(float dt) {}
	virtual void Draw() {}
	virtual void HandeInput() {}
	virtual void Reset() {}
	virtual void Load(pugi::xml_node&){}
	virtual void Save(pugi::xml_node&) const {}
	virtual void OnCollision(Collider* c1, Collider* c2) {}
};

class j1Entities : public j1Module 
{
public:
	j1Entities();
	~j1Entities() {}

	bool PreUpdate(float dt);
	bool Update(float dt);
	bool Awake(pugi::xml_node& config);
	void ResetEntities();
	void OnCollision(Collider* c1, Collider* c2);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void LoadFromObjectLayer(ObjectLayer* layer);
	Entity* CreateEntity(Entity::EntityType type, iPoint pos, int dest_X = NULL, int dest_Y = NULL);
	void DestroyEntity(Entity* entity);
	void DestroyAll();

	bool pause = false;

private:

	void UpdateEntities(float dt, bool do_logic);

	p2List<Entity*> entities;

	float accumulated_time;
	float update_ms_cycle;

	

	bool do_logic = false;
};

#endif
