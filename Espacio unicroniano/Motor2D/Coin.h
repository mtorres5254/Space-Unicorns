#ifndef __COIN_H__
#define __COIN_H__

#include "j1Entities.h"
#include "j1Animations.h"

class Coin : public Entity {
public:
	Coin(iPoint pos);
	~Coin();

	void PreUpdate(float dt);
	void Update(float dt);
	void Draw();
	//void HandeInput();

	void Reset();
	//void Load(pugi::xml_node&);
	//void Save(pugi::xml_node&) const;
	void OnCollision(Collider* c1, Collider* c2);

private:
	iPoint CoinPosition;
	Animation idle;
	Animation takenanim;
	Animation* Current_animation;
	bool taken = false;
	//bool falling = true;
	float private_dt;
	SDL_RendererFlip flip = SDL_FLIP_NONE;
	Collider* col;
	int coinCnt = 0;
	


};

#endif
