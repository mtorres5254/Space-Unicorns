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
	void Reset();
	void OnCollision(Collider* c1, Collider* c2);

private:
	iPoint initialPosition;
	iPoint vel;

	bool dead = false;
	bool hitted = false;
	bool falling = true;

	SDL_RendererFlip flip = SDL_FLIP_NONE;

	float private_dt;
	//---------------------------------
	Animation* Current_animation;
	Animation idle;
	Animation hit;
	Animation death;

};

#endif
