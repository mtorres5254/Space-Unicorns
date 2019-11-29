#ifndef __J1FLYENEMT_H__
#define __J1FLYENEMY_H__

#include "j1Entities.h"
#include "j1App.h"
#include "j1Animations.h"


class j1FlyEnemy : public Entity {
public:
	j1FlyEnemy(iPoint pos);
	~j1FlyEnemy();

	void Update(float dt);
	void Draw();
	void HandeInput();
	bool ChasePlayer(iPoint);
	void Reset();
	void Load(pugi::xml_node&);
	void Save(pugi::xml_node&) const;
	void OnCollision(Collider* c1, Collider* c2);

	Animation* Current_animation;
	Animation idle;
	Animation hit;
	Animation death;

	SDL_RendererFlip flip = SDL_FLIP_NONE;

	bool hitted = false;
	bool dead = false;
	bool falling = false;

	iPoint initial_position;
	iPoint vel;

	int maxLives;
private:
	float private_dt;
};

#endif
