#ifndef __J1PARTICLES_H__
#define __J1PARTICLES_H__

#include "j1Entities.h"
#include "j1Animations.h"

class j1Particle : public Entity {
public:
	j1Particle(iPoint pos, int x, int y);
	~j1Particle() {}

	void Update(float dt);
	void Draw();
	void OnCollision(Collider* c1, Collider* c2);

private:
	iPoint destination;

	int aux1;
	int aux2;

	Animation idle;

};

#endif
