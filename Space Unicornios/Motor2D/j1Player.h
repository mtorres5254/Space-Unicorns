#ifndef __ModulePlayer_H__
#define __ModulePlayer_H__

#include "p2Point.h"
#include "p2Defs.h"
#include "j1Module.h"

#define INPUTSOUTS 7

enum state {
	A_ZERO = 0,
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
	//add animations + sound
	SDL_Rect rectplayer;

	//position and limitators
};
#endif
