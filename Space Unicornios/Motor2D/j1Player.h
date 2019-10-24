#ifndef __ModulePlayer_H__
#define __ModulePlayer_H__

#include "p2Point.h"
#include "p2Defs.h"
#include "j1Module.h"
#include "j1Animations.h"

#define INPUTSOUTS 7

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

	void OnCollision(Collider* c1, Collider* c2);
	//add colliders
public:
	
	iPoint position;
	//texture , animations and sounds
	SDL_Texture* graphics = nullptr;

	Animation* Current_Animation;
	Animation idle;
	Animation walking;
	Animation jumping;
	Animation crouch;
	Animation death;
	Animation special;

	//------------------
	Collider* col;
	Collider* col_prova;
	
	SDL_Rect rectplayer;
	SDL_Rect rectoli;

	int gravity = 2;
	int max_gravity = 20;
	int player_speed = 5;
	int jumping_speed = 25;
	int mult;
	int max_jumpheight = 40;

	SDL_RendererFlip flip = SDL_FLIP_NONE;
	
	bool godmode = false;
	bool jumping_bool = false;
	bool falling = false;
	bool ducking = false;
	bool not_forward = false;
	bool not_backwards = false;
	bool death_bool = false;

	//--------------------
	state states;
	input inputs;
	
	//position and limitators
};
#endif
