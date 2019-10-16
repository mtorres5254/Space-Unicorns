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


};

enum inputout {
	//add inputsout
};

class j1Player :public j1Module {
public:
	j1Player();
	~j1Player();


	bool Start();
	bool Awake();
	bool PreUpdate();
	bool Update();
	bool PostUpdate();
	bool CleanUp();
	//add colliders
public:
	
	iPoint position;
	//texture , animations and sounds
	SDL_Texture* current_graphics = nullptr;
	SDL_Texture* normal_graphics = nullptr;

	Animation Current_Animation;
	Animation idle;
	Animation walking;
	Animation jumping;
	Animation duck;
	Animation death;
	Animation special;


	//add more animations + sound
	
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
	bool jumping = false;
	bool falling = false;
	bool ducking = false;
	bool not_forward = false;
	bool not_backwards = false;
	bool death = false;
	bool godmode = false;
	state actual;
	
	//position and limitators
};
#endif
