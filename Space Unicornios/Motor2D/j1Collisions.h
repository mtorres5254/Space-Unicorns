#ifndef __j1COLLISIONS_H__
#define _j1COLLISIONS_H_

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

#define MAX_COLLIDERS 100

//------------------------------------------------------------------------
enum COLLIDER_TYPE
{
	COLLIDER_NONE = -1,
	COLLIDER_PLAYER,
	COLLIDER_ENEMY,
	COLLIDER_FLOOR,

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

	void SetPos(int x, int y) {
		rect.x = x;
		rect.y = y;
	}

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

	bool PreUpdate();

	bool Update();

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	Collider* AddCollider(SDL_Rect rectC, COLLIDER_TYPE typeC, j1Module* callbackC = nullptr);
	void DebugDraw();
	void DeleteCollider(Collider*);

private:

	bool LoadColliders();

public:

	MapData data;

private:

	Collider* colliders[MAX_COLLIDERS];
	bool matrix[COLLIDER_MAX][COLLIDER_MAX];
	bool debug = false;

#endif // __j1COLLISIONS_H__