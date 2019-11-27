#ifndef  _MODULE_SCENECHANGE_
#define _MODULE_SCENECHANGE_

#include "j1Module.h"
#include "j1Timer.h"
#include "SDL\include\SDL_rect.h"


class j1MapChange : public j1Module
{
//-------------Functions-----------------
public:

	j1MapChange();

	//Destructor
	virtual ~j1MapChange();

	//Called before render is avalible
	bool Awake(pugi::xml_node&);

	//Called before the first frame
	bool Start();

	//Called each loop iteration
	bool Update(float dt);

	//Called to set up the map change and the time the game will fade
	bool ChangeMap(float time, int map_id = NULL);

	bool IsChanging() const;

private:
	
//-------------Variables-----------------
public:

	bool fading = false;
	bool reset = false;
	j1Timer reset_timer;

private:
	enum fade_step
	{
		none,
		fade_to_black,
		fade_from_black
	} current_step = fade_step::none;

	p2SString map1;
	p2SString map2;

	int map_to_change = 0;
	
	uint start_time = 0;
	uint total_time = 0;
	SDL_Rect screen;


};
#endif // ! _MODULE_SCENECHANGE_
