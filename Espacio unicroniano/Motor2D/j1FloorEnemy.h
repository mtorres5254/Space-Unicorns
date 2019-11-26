#ifndef __J1FLOORENEMY_H__
#define __J1FLOORENEMY_H__

#include "j1Entities.h"
#include "j1Animations.h"

class j1FloorEnemy : public Entity {
public:
	j1FloorEnemy(iPoint pos);
	~j1FloorEnemy();

	void PreUpdate(float dt);
	void Update(float dt);
	void Draw();
	void HandeInput();
	void OnCollision(Collider* c1, Collider* c2);

private:
	iPoint initialPosition;

	bool dead = false;

	SDL_RendererFlip flip = SDL_FLIP_NONE;

	//---------------------------------
	Animation* Current_animation;
	Animation idle;
	Animation death;

};

#endif
