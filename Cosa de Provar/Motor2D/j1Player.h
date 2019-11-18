#ifndef __ModulePlayer_H__
#define __ModulePlayer_H__

#include "p2Point.h"
#include "p2Defs.h"
#include "j1Module.h"
#include "SDL/include/SDL.h"
#include "j1Animations.h"
#include "j1Entities.h"

#define INPUTSOUTS 7
#define SPEED 300

enum state {
	A_NONE = 0,
	A_IDLE,
	A_WALK_FORWARD,
	A_WALK_BACKWARDS,
	A_JUMP_NEUTRAL,
	A_FALLING,
	A_CROUCH,
	A_SPECIAL,
	A_DEAD,
};

struct Collider;

class j1Player :public Entity{
public:
	j1Player(iPoint pos);
	~j1Player();


	void Update(float dt);
	void Draw();
	void HandeInput();
	void OnCollision(Collider* c1, Collider* c2);

private:

	iPoint vel;

	Animation* Current_Animation;
	Animation idle;
	Animation walking;
	Animation jumping;
	Animation fall;
	Animation crouching;
	Animation death;
	Animation special_anim;

	//------------------	

	SDL_RendererFlip flip = SDL_FLIP_NONE;
	
	bool godmode = false;
	bool died = false;

	bool has_jump = false;
	bool has_col = true;
	bool falling = true;

	int maxjump;

	unsigned int walkingsound;
	unsigned int jumpingsound;

	//--------------------
	state states;
	
	//position and limitators
};
#endif
