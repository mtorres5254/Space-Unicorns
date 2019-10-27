#ifndef __ModulePlayer_H__
#define __ModulePlayer_H__

#include "p2Point.h"
#include "p2Defs.h"
#include "j1Module.h"
#include "SDL/include/SDL.h"
#include "j1Animations.h"

#define INPUTSOUTS 7
#define JUMP 80
#define SPEED 3
#define JUMP_SPEED 2.5
#define JUMP_Y_SPEED 5

enum state {
	A_NONE = 0,
	A_IDLE,
	A_WALK_FORWARD,
	A_WALK_BACKWARDS,
	A_JUMP_NEUTRAL,
	A_JUMP_FORWARD,
	A_JUMP_BACKWARDS,
	A_HOOK,
};

enum input {
	//add inputs
	IN_NONE = 0,
	IN_JUMP,
	IN_FALLING,
	IN_JUMP_LEFT,
	IN_JUMP_RIGHT,
	IN_LEFT,
	IN_RIGHT,
	IN_CROUCH,
	IN_SPECIAL,

};

enum inputout {
	//add inputsout
	OUT_NONE = 0,
	OUT_JUMP,

};

struct Collider;

class j1Player :public j1Module {
public:
	j1Player();
	~j1Player();


	bool Start();
	bool Awake(pugi::xml_node& conf);
	bool PreUpdate();
	bool Update(float dt);
	bool PostUpdate();
	bool CleanUp();

	input GetInput();
	input GetLeftRight();

	void OnCollision(Collider* c1, Collider* c2);
	void ChangeLevel1();
	void ChangeLevel2();
	//void RestartLevel1();
	//void RestartLevel2();
	//add colliders
public:
	
	iPoint position;


	SDL_Texture* graphics = nullptr;

	Animation* Current_Animation;
	Animation idle;
	Animation walking;
	Animation jumping;
	Animation fall;
	Animation crouching;
	Animation death;
	Animation special_anim;

	//------------------
	Collider* col;
	Collider* col_prova;
	
	SDL_Rect rectplayer;

	SDL_RendererFlip flip = SDL_FLIP_NONE;
	
	bool godmode = false;
	bool jump = false;
	bool left = false;
	bool right = false;
	bool crouch = false;
	bool special = false;
	bool died = false;

	bool has_jump = false;

	bool falling = true;

	int maxjump;

	unsigned int walkingsound;
	unsigned int jumpingsound;

	//--------------------
	state states;
	input inputs;
	
	//position and limitators
};
#endif
