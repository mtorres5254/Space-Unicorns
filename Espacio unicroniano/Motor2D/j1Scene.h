#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct Collider;
struct j1Player;
struct j1FloorEnemy;
struct j1FlyEnemy;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate(float dt);

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	void OnCollision(Collider* c1, Collider* c2);

public:
	j1Player* player  = nullptr;
	p2List<j1FloorEnemy*> FloorEnemies;
	p2List<j1FlyEnemy*> FlyEnemies;

	iPoint initial_camera;
	
	int map = 1;

	iPoint origin;
	iPoint dest;

	bool point1selected;

	Collider* col_camera_up;
	Collider* col_camera_down;
	Collider* col_camera_left;
	Collider* col_camera_right;

private:
	SDL_Texture* debug_tex;
	float dt_scene;
};

#endif // __j1SCENE_H__