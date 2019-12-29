#ifndef __j1GUI_H__
#define __j1GUI_H__

#include "j1Module.h"
#include "p2DynArray.h"

#define CURSOR_WIDTH 2

class Events {
public:
	enum class ButtonEvents {
		NONE = 0,
		NEWGAME,
		CONTINUE,
		SETTINGS,
		CREDITS,
		EXIT,
		BACKTOMENU,
		CLOSEWINDOW,
	};

	enum class ScrollEvents {
		NONE = 0,
		MUSIC,
		FX
	};

	ButtonEvents buttonEvent;
	ScrollEvents scrollevent;
	int volume;

	Events(Events::ButtonEvents eventbutton) {
		buttonEvent = eventbutton;
	}

	Events(Events::ScrollEvents eventscroll, int vol) {
		scrollevent = eventscroll;
		volume = vol;
	}

};

// TODO 1: Create your structure of classes
class UI {
public:
	enum class UiType {
		IMG = 0,
		TXT,
		BUTTON,
		WINDOW,
		INPUT,
		SCROLLBAR,
		UKNOWN
	};

	enum class MouseEvent {
		NONE = 0,
		CLICK,
		HOVER,
	};

	bool movable = false;
	bool interectable = true;

	UI* parent = nullptr;

	iPoint position;
	UiType type;
	MouseEvent mouse = MouseEvent::NONE;
	MouseEvent past_mouse = MouseEvent::NONE;
protected:
	SDL_Rect focus_Rect;
public:
	//--------------------------------------
	UI::UI(UiType type) : type(type)
	{}

	virtual void Draw() {}
	virtual void Update() {}

	bool CheckOnClcick(iPoint mouse_pos);
	bool CheckOnHover(iPoint mouse_pos);

	void DebugDraw();

	virtual bool DoMouseEvent() { return false; }

	UI* GetMovementFocus(iPoint mouse_pos);
	bool UnderMouse(iPoint mouse_pos);

	SDL_Rect GetScreenRect();
	SDL_Rect GetLocalRect();
	iPoint GetScreenPos();
	iPoint GetLocalPos();
	void SetLocalPos(iPoint pos);

};

//----------------------------------------------------------------

class UI_Img : public UI {
public:
	SDL_Rect sprite;
	SDL_Rect normalSprite;
	SDL_Rect ClickSprite;
	SDL_Rect HoverSprite;

	//--------------------------
	UI_Img(SDL_Rect rect, iPoint pos, bool movable = false);
	UI_Img(SDL_Rect rect, iPoint pos, SDL_Rect Click, SDL_Rect Hover, UI* paren = nullptr, bool movable = false);
	
	void Draw();
	void Update();

	bool DoMouseEvent();
};

class UI_Txt : public UI {
public:
	p2SString text;
	p2SString normalText;
	p2SString ClickText;
	p2SString HoverText;

	//--------------------------
	UI_Txt(const char* txt, iPoint pos, bool movable = false);
	UI_Txt(const char* txt, iPoint pos, const char* hovertxt, const char* clicktxt, UI* paren = nullptr, bool movable = false);
	
	void Draw();
	void Update();

	bool DoMouseEvent();
};

class UI_Button : public UI{
public:
	UI_Img* image;
	UI_Txt* text;
	Events* Feedback;

	//-----------------------------
	UI_Button(iPoint pos, const char* txt, SDL_Rect rect, SDL_Rect clickrect, SDL_Rect hoverrect, Events::ButtonEvents feedback, UI* paren = nullptr, bool move = false);

	void Update();

	bool DoMouseEvent();

};

class UI_Window : public UI {
public:
	UI_Img* image;
	UI_Txt* text;

	//------------------------------
	UI_Window(iPoint pos, const char* txt, SDL_Rect rect, UI* paren = nullptr, bool move = false);

	void Update();
};

class UI_Input : public UI {
public: 
	UI_Img* image;
	UI_Txt* text;
	int text_pos = 0;

	//------------------------------
	UI_Input(iPoint pos, const char* txt, SDL_Rect rect, UI* paren = nullptr, bool move = false);

	void Update();

	bool DoMouseEvent();
};

class UI_ScrollBar : public UI {
public:
	UI_Img* image_bar;
	UI_Img* image_pointer;
	Events* Feedback;

	int bar_pointer;

	//--------------------------------
	UI_ScrollBar(iPoint pos, SDL_Rect barRect, SDL_Rect pointerRect, int initial_point, Events::ScrollEvents feedback, UI* paren = nullptr, bool move = false);

	void Update();

	bool DoMouseEvent();
};


//----------------------------------------------------------------


class j1Gui : public j1Module
{
public:

	j1Gui();

	// Destructor
	virtual ~j1Gui();

	// Called when before render is available
	bool Awake(pugi::xml_node&);

	// Call before first frame
	bool Start();

	// Called before all Updates
	bool PreUpdate(float dt);

	bool Update(float dt);

	// Called after all Updates
	bool PostUpdate(float dt);

	// Called before quitting
	bool CleanUp();

	// TODO 2: Create the factory methods

	bool DeleteUI_Item(UI* item);
	void DeleteAllUI();

	// Gui creation functions
	UI_Img* CreateUI_Image(SDL_Rect rect, iPoint pos, SDL_Rect* Click = nullptr, SDL_Rect* Hover = nullptr, UI* parent = nullptr, bool move = false);
	UI_Txt* CreateUI_Text(const char* text, iPoint pos, const char* hovertxt = nullptr, const char* clicktxt = nullptr, UI* parent = nullptr, bool move = false);
	UI_Button* CreateUI_Button(const char* text, iPoint pos, SDL_Rect rect, SDL_Rect clickrect, SDL_Rect hoverrect, Events::ButtonEvents feedback, UI* parent = nullptr, bool move = false);
	UI_Window* CreateUI_Window(const char* text, iPoint pos, SDL_Rect rect, UI* parent = nullptr, bool move = false);
	UI_Input* CreateUI_Input(const char* text, iPoint pos, SDL_Rect rect, UI* parent = nullptr, bool move = false);
	UI_ScrollBar* CreateUI_ScrollBar(iPoint pos, SDL_Rect bar, SDL_Rect pointer, int initial, Events::ScrollEvents feedback, UI* parent = nullptr, bool move = false);

	void DrawUI();
	void UpdateUI();
	void LogicUI();

	void MoveUI();

	void CheckInteractebleFocus();

	bool EventGestion();

	SDL_Texture* GetAtlas() const;

	p2DynArray<Events*> events;

	UI* MoveFocus;
	UI* InteractFocus;
	UI* EventFocus;

private:
	bool debug = false;

	p2List<UI*> ui_list;

	SDL_Texture* atlas;
	p2SString atlas_file_name;
};

#endif // __j1GUI_H__