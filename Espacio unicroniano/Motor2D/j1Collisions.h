#ifndef __j1COLLISIONS_H__
#define _j1COLLISIONS_H_

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Map.h"
#include "j1Module.h"

#define MAX_COLLIDERS 200

//------------------------------------------------------------------------
enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_PLAYER,
	COLLIDER_ENEMY,
	COLLIDER_FLOOR,
	COLLIDER_WALL,
	COLLIDER_DEAD,
	COLLIDER_END,
	COLLIDER_CAM_UP,
	COLLIDER_CAM_DOWN,
	COLLIDER_CAM_LEFT,
	COLLIDER_CAM_RIGHT,
	COLLIDER_MAX
};
//------------------------------------------------------------------------
struct Collider 
{
	SDL_Rect rect;
	bool to_delete = false;
	COLLIDER_TYPE type;
	j1Module* callback = nullptr;

	Collider(SDL_Rect rectC, COLLIDER_TYPE typeC, j1Module* callbackC = nullptr) {
		rect = rectC;
		type = typeC;
		callback = callbackC;
	}

	void SetPos(int, int);
		
	
	

	bool CheckCollision(const SDL_Rect& r) const;
};

//------------------------------------------------------------------------
class j1Collisions : public j1Module
{
public:

	j1Collisions();

	// Destructor
	virtual ~j1Collisions();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	bool Start();

	bool PreUpdate(float dt);

	bool Update(float dt);

	// Called before quitting
	bool CleanUp();

	bool LoadFromObjectLayer(ObjectLayer* layer);

	Collider* AddCollider(SDL_Rect rectC, COLLIDER_TYPE typeC, j1Module* callbackC = nullptr);
	void DebugDraw();
	void DeleteCollider(Collider*);
	void DeleteAll();

private:
	//bool LoadColliders();
	p2SString			folder;
	pugi::xml_document collisions_data;
	Collider* colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	bool debug = false;
};

#endif // __j1COLLISIONS_H__