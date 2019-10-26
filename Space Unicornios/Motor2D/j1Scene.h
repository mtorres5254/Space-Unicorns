#ifndef __j1SCENE_H__
#define __j1SCENE_H__

#include "j1Module.h"

struct SDL_Texture;

class j1Scene : public j1Module
{
public:

	j1Scene();

	// Destructor
	virtual ~j1Scene();

	// Called before render is available
	bool Awake(pugi::xml_node& config);

	// Called before the first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate();

	// Called each loop iteration
	bool Update(float dt);

	// Called before all Updates
	bool PostUpdate();

	// Called before quitting
	bool CleanUp();
	void ResetLevel(bool);
public: 

	Collider* End;
	int Endx;
	int Endy;

	bool colliderEnd = false;
	bool ColliderEnd1 = false;
	bool ColliderEnd2 = false;
	bool ChangeLevel = false;
 
	int initialposx;
		int initialposy;

private:
};

#endif // __j1SCENE_H__