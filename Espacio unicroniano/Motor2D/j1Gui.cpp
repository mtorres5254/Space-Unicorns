#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Audio.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Pathfinding.h"
#include "j1SceneChange.h"
#include "j1Fonts.h"
#include "j1Entities.h"
#include "j1Map.h"
#include "j1Input.h"
#include "j1Collisions.h"
#include "j1Scene.h"
#include "j1Gui.h"

j1Gui::j1Gui() : j1Module()
{
	name.create("gui");
}

// Destructor
j1Gui::~j1Gui()
{}

// Called before render is available
bool j1Gui::Awake(pugi::xml_node& conf)
{
	LOG("Loading GUI atlas");
	bool ret = true;

	atlas_file_name = conf.child("atlas").attribute("file").as_string("");

	return ret;
}

// Called before the first frame
bool j1Gui::Start()
{
	atlas = App->tex->Load(atlas_file_name.GetString());

	return true;
}

// Update all guis
bool j1Gui::PreUpdate(float dt)
{	
	UpdateUI();
	

	if (App->input->GetKey(SDL_SCANCODE_F1) == true) {
		debug = !debug;
	}

	if (debug != true) {
		return true;
	}

	p2List_item<UI*>* ui_item;

	for (ui_item = ui_list.start; ui_item != NULL; ui_item = ui_item->next) {
		ui_item->data->DebugDraw();
	}

	return true;
}

bool j1Gui::Update(float dt) {
	bool ret = true;

	LogicUI();
	ret = EventGestion();

	return ret;
}

// Called after all Updates
bool j1Gui::PostUpdate(float dt)
{
	DrawUI();

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);
	p2List_item<UI*>* Aux;

	for (Aux = ui_list.end; Aux != nullptr; Aux = Aux->prev) {
		if (Aux->data->parent == nullptr) {
			if (Aux->data->movable == true) {
				MoveFocus = Aux->data->GetMovementFocus(p);
			}
		}
	}

	MoveUI();

	if (InteractFocus == nullptr) {
		App->input->DisableTextInput();
	}
	else {
		CheckInteractebleFocus();
	}

	return true;
}

// Called before quitting
bool j1Gui::CleanUp()
{
	LOG("Freeing GUI");

	return true;
}

// const getter for atlas
SDL_Texture* j1Gui::GetAtlas() const
{
	return atlas;
}

void j1Gui::DrawUI() {
	p2List_item<UI*>* AuxUI = ui_list.start;

	for (; AuxUI != NULL; AuxUI = AuxUI->next) {
		AuxUI->data->Draw();
	}
}

void j1Gui::UpdateUI() {
	p2List_item<UI*>* AuxUI = ui_list.start;

	for (; AuxUI != NULL; AuxUI = AuxUI->next) {
		AuxUI->data->Update();
	}
}

void j1Gui::LogicUI() {
	p2List_item<UI*>* AuxUI = ui_list.end;

	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);

	//LOG("%i x %i", p.x, p.y);

	for (; AuxUI != NULL; AuxUI = AuxUI->prev) {
		if (AuxUI->data->mouse == UI::MouseEvent::CLICK) {
			AuxUI->data->CheckOnClcick(p);
		}
		else {
			if (AuxUI->data->CheckOnHover(p) == true) {
				AuxUI->data->CheckOnClcick(p);

			}
		}		
	}
	for (AuxUI = ui_list.end; AuxUI != NULL; AuxUI = AuxUI->prev) {
		if (AuxUI->data->mouse != AuxUI->data->past_mouse) {
			if (AuxUI->data->DoMouseEvent() == true) {
				break;
			}
		}
	}
}

bool j1Gui::EventGestion() {
	Events* auxEvent;
	iPoint pos;
	iPoint pos2;
	UI* Aux;
	
	if (events.Pop(auxEvent) == true) {
		switch (auxEvent->buttonEvent)
		{
		case Events::ButtonEvents::NEWGAME:
			DeleteAllUI();
			App->scene_change->ChangeMap(2.0f, 1);
			break;
		case Events::ButtonEvents::CONTINUE:
			DeleteAllUI();
			App->LoadGame("save_game.xml");
			break;
		case Events::ButtonEvents::SETTINGS:
			App->scene->SettingsMenu();
			break;
		case Events::ButtonEvents::EXIT:
			return false;
			break;
		case Events::ButtonEvents::CLOSEWINDOW:
			DeleteUI_Item(EventFocus);
			break;
		case Events::ButtonEvents::BACKTOMENU:

			//delete data
			App->map->CleanUp();
			App->col->DeleteAll();
			App->entity->DestroyAll();
			App->scene->player = nullptr;
			App->scene->FloorEnemies.clear();
			App->scene->FlyEnemies.clear();

			//open start menu
			App->scene->StartMenu();
			break;
		default:
			break;
		}

		switch (auxEvent->scrollevent) {
		case Events::ScrollEvents::MUSIC:
			App->audio->SetMusicVolume(100 - auxEvent->volume);
			break;
		case Events::ScrollEvents::FX:
			App->audio->SetFxVolume(100 - auxEvent->volume);
			break;
		default:
			break;
		}
	}
	return true;
}

bool j1Gui::DeleteUI_Item(UI* item) {
	bool ret = true;

	p2List_item<UI*>* Aux;
	//delete children
	for (Aux = ui_list.start; Aux != NULL; Aux = Aux->next) {
		if (Aux->data->parent == item) {
			DeleteUI_Item(Aux->data);
		}
	}
	//delete the item
	for (Aux = ui_list.start; Aux != NULL; Aux = Aux->next) {
		if (Aux->data == item) {
			ui_list.del(Aux);
			break;
		}
	}

	return ret;
}

void j1Gui::DeleteAllUI() {
	ui_list.clear();
}

void j1Gui::MoveUI() {
	int x, y;

	App->input->GetMouseMotion(x, y);

	if (MoveFocus != nullptr) {
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
			MoveFocus->position.x += x;
			MoveFocus->position.y += y;
		}
	}
}

void j1Gui::CheckInteractebleFocus() {
	int x, y;
	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);

	if (InteractFocus->UnderMouse(p) == false) {
		if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
			InteractFocus = nullptr;
		}
	}
}

//UI Create functions
UI_Img* j1Gui::CreateUI_Image(SDL_Rect rect, iPoint pos, SDL_Rect* Click, SDL_Rect* Hover, UI* parent, bool move) {
	UI* ret = nullptr;

	if (Click == nullptr && Hover == nullptr) {
		ret = new UI_Img(rect, pos, move);
	}
	else {
		ret = new UI_Img(rect, pos, *Click, *Hover, parent, move);
	}

	if (ret != nullptr) {
		ui_list.add(ret);
	}

	return (UI_Img*)ret;
}

UI_Txt* j1Gui::CreateUI_Text(const char* text, iPoint pos, const char* hovertxt, const char* clicktxt, UI* parent, bool move) {
	UI* ret = nullptr;

	if (hovertxt == nullptr && clicktxt == nullptr) {
		ret = new UI_Txt(text, pos, move);
	}
	else {
		ret = new UI_Txt(text, pos, hovertxt, clicktxt,parent, move);
	}

	if (ret != nullptr) {
		ui_list.add(ret);
	}

	return (UI_Txt*)ret;
}

UI_Button* j1Gui::CreateUI_Button(const char* text, iPoint pos, SDL_Rect rect, SDL_Rect clickrect, SDL_Rect hoverrect, Events::ButtonEvents feedback, UI* parent, bool move) {
	UI* ret = nullptr;

	ret = new UI_Button(pos, text, rect, clickrect, hoverrect, feedback, parent, move);

	if (ret != nullptr) {
		ui_list.add(ret);
	}

	return (UI_Button*)ret;
}

UI_Window* j1Gui::CreateUI_Window(const char* text, iPoint pos, SDL_Rect rect, UI* parent, bool move) {
	UI* ret = nullptr;

	ret = new UI_Window(pos, text, rect, parent, move);

	if (ret != nullptr) {
		ui_list.add(ret);
	}

	return (UI_Window*)ret;
}

UI_Input* j1Gui::CreateUI_Input(const char* text, iPoint pos, SDL_Rect rect, UI* parent, bool move) {
	UI* ret = nullptr;

	ret = new UI_Input(pos, text, rect, parent, move);

	if (ret != nullptr) {
		ui_list.add(ret);
	}

	return (UI_Input*)ret;
}

UI_ScrollBar* j1Gui::CreateUI_ScrollBar(iPoint pos, SDL_Rect bar, SDL_Rect pointer, int initial, Events::ScrollEvents feedback, UI* parent, bool move) {
	UI* ret = nullptr;

	ret = new UI_ScrollBar(pos, bar, pointer, initial, feedback, parent, move);

	if (ret != nullptr) {
		ui_list.add(ret);
	}

	return (UI_ScrollBar*)ret;
}


// CLASS Gui ---------------------------------------------------

bool UI::CheckOnClcick(iPoint mouse_pos) {
	bool ret = false;

	if (mouse == MouseEvent::CLICK) {
		past_mouse = MouseEvent::CLICK;
	}
	else if (mouse == MouseEvent::HOVER && past_mouse == MouseEvent::CLICK) {
		past_mouse = MouseEvent::HOVER;
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_UP || App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_REPEAT) {
		ret = true;
		mouse = MouseEvent::CLICK;
	}

	if (ret == false) {
		mouse = MouseEvent::HOVER;
	}

	return ret;
}

bool UI::CheckOnHover(iPoint mouse_pos) {
	bool ret = false;

	if (mouse == MouseEvent::HOVER) {
		past_mouse = MouseEvent::HOVER;
	}
	else if (mouse == MouseEvent::NONE) {
		past_mouse = MouseEvent::NONE;
	}

	if (mouse_pos.x > focus_Rect.x && mouse_pos.x < focus_Rect.x + focus_Rect.w) {
		if (mouse_pos.y > focus_Rect.y && mouse_pos.y < focus_Rect.y + focus_Rect.h) {
			ret = true;
			mouse = MouseEvent::HOVER;
		}
	}

	if (ret == false) {
		mouse = MouseEvent::NONE;
	}

	return ret;
}

void UI::DebugDraw() {

	App->render->DrawQuad(focus_Rect, 255, 0, 0, 100, false);

}

UI* UI::GetMovementFocus(iPoint mouse_pos) {
	if (mouse_pos.x > focus_Rect.x && mouse_pos.x < focus_Rect.x + focus_Rect.w) {
		if (mouse_pos.y > focus_Rect.y && mouse_pos.y < focus_Rect.y + focus_Rect.h) {
			if (movable == true) {
				return this;
			}
		}
	}

	return nullptr;
}

bool UI::UnderMouse(iPoint mouse_pos) {
	if (mouse_pos.x > focus_Rect.x && mouse_pos.x < focus_Rect.x + focus_Rect.w && mouse_pos.y > focus_Rect.y && mouse_pos.y < focus_Rect.y + focus_Rect.h) {
		return true;
	}
	return false;
}

// UI Image --------------------------------------------------------------------------------
UI_Img::UI_Img(SDL_Rect rect, iPoint pos, bool move) : UI(UiType::IMG) {
	sprite = rect;
	normalSprite = rect;
	HoverSprite = rect;
	ClickSprite = rect;

	movable = move;

	focus_Rect.x = pos.x;
	focus_Rect.y = pos.y;
	focus_Rect.w = rect.w;
	focus_Rect.h = rect.h;

	position = pos;
}

UI_Img::UI_Img(SDL_Rect rect, iPoint pos, SDL_Rect Click, SDL_Rect Hover, UI* paren, bool move) : UI(UiType::IMG) {
	sprite = rect;
	normalSprite = rect;
	HoverSprite = Hover;
	ClickSprite = Click;

	movable = move;

	focus_Rect.x = pos.x;
	focus_Rect.y = pos.y;
	focus_Rect.w = rect.w;
	focus_Rect.h = rect.h;

	movable = move;
	parent = paren;
	position = pos;
}

void UI_Img::Draw() {
	SDL_Texture* atlas_texture = App->gui->GetAtlas();
	
	iPoint p = App->render->ScreenToWorld(position.x, position.y);

	App->render->Blit(atlas_texture, p.x, p.y, &sprite);
}

void UI_Img::Update() {
	iPoint p = App->render->ScreenToWorld(position.x, position.y);

	focus_Rect.x = p.x;
	focus_Rect.y = p.y;
}

bool UI_Img::DoMouseEvent() {
	//Hover condition
	if (mouse == MouseEvent::HOVER && past_mouse == MouseEvent::NONE) {
		//change at hover;
		sprite = HoverSprite;
		return true;
	}

	if (mouse == MouseEvent::NONE && past_mouse == MouseEvent::HOVER) {
		//back to normal
		sprite = normalSprite;
		return true;
	}

	//click condition
	if (mouse == MouseEvent::CLICK && past_mouse == MouseEvent::HOVER) {
		//change at click;
		sprite = ClickSprite;
		return true;
	}

	if (mouse == MouseEvent::HOVER && past_mouse == MouseEvent::CLICK) {
		//back to hover
		sprite = normalSprite;
		return true;
	}

	return false;
}


// UI Text --------------------------------------------------------------------------------
UI_Txt::UI_Txt(const char* txt, iPoint pos, bool move) : UI(UiType::TXT) {
	text.create(txt);
	normalText.create(txt);
	HoverText.create(txt);
	ClickText.create(txt);

	movable = move;

	int height, width;
	App->font->CalcSize(txt, width, height, App->font->default);
	focus_Rect = {pos.x, pos.y, width, height};

	position = pos;
}

UI_Txt::UI_Txt(const char* txt, iPoint pos, const char* hovertxt, const char* clicktxt, UI* paren, bool move) : UI(UiType::TXT) {
	text.create(txt);
	normalText.create(txt);
	HoverText.create(hovertxt);
	ClickText.create(clicktxt);

	movable = move;

	int height, width;
	App->font->CalcSize(txt, width, height, App->font->default);
	focus_Rect = { pos.x, pos.y, width, height };

	movable = move;
	parent = paren;
	position = pos;
}

void UI_Txt::Draw() {
	iPoint p = App->render->ScreenToWorld(position.x, position.y);

	SDL_Texture* texture = App->font->Print(text.GetString(), { 255,255,255 }, App->font->default);

	App->render->Blit(texture, p.x, p.y);

	SDL_DestroyTexture(texture);
}

void UI_Txt::Update() {
	iPoint p = App->render->ScreenToWorld(position.x, position.y);

	focus_Rect.x = p.x;
	focus_Rect.y = p.y;
}

bool UI_Txt::DoMouseEvent() {
	//Hover condition
	if (mouse == MouseEvent::HOVER && past_mouse == MouseEvent::NONE) {
		//change at hover;
		text.create(HoverText.GetString());
		return true;
	}

	if (mouse == MouseEvent::NONE && past_mouse == MouseEvent::HOVER) {
		//back to normal
		text.create(normalText.GetString());
		return true;
	}

	//click condition
	if (mouse == MouseEvent::CLICK && past_mouse == MouseEvent::HOVER) {
		//change at click;
		text.create(ClickText.GetString());
		return true;
	}

	if (mouse == MouseEvent::HOVER && past_mouse == MouseEvent::CLICK) {
		//back to hover
		text.create(normalText.GetString());
		return true;
	}

	return false;
}


// UI Button ------------------------------------------------------------------------------
UI_Button::UI_Button(iPoint pos, const char* txt, SDL_Rect rect, SDL_Rect clickrect, SDL_Rect hoverrect, Events::ButtonEvents feedback, UI* paren, bool move) : UI(UiType::BUTTON) {
	image = App->gui->CreateUI_Image(rect, pos, &clickrect, &hoverrect, this, move);

	int height, width;
	App->font->CalcSize(txt, width, height, App->font->default);

	iPoint textpos;
	textpos.x = (pos.x + rect.w / 2) - width / 2;
	textpos.y = (pos.y + rect.h / 2) - height / 2;

	text = App->gui->CreateUI_Text(txt, textpos, txt, txt, this, move);

	focus_Rect.x = pos.x;
	focus_Rect.y = pos.y;
	focus_Rect.w = rect.w;
	focus_Rect.h = rect.h;

	Feedback = new Events(feedback);
	movable = move;
	parent = paren;
	position = pos;
}

void UI_Button::Update() {
	iPoint p = App->render->ScreenToWorld(position.x, position.y);

	focus_Rect.x = p.x;
	focus_Rect.y = p.y;

	image->position = position;

	int height, width;
	App->font->CalcSize(text->text.GetString(), width, height, App->font->default);

	iPoint textpos;
	textpos.x = (position.x + focus_Rect.w / 2) - width / 2;
	textpos.y = (position.y + focus_Rect.h / 2) - height / 2;

	text->position = textpos;
}

bool UI_Button::DoMouseEvent() {
	image->DoMouseEvent();
	text->DoMouseEvent(); 

	//Hover condition
	if (mouse == MouseEvent::HOVER && past_mouse == MouseEvent::NONE) {
		//change at hover
		return true;
	}

	if (mouse == MouseEvent::NONE && past_mouse == MouseEvent::HOVER) {
		//back to normal
		return true;
	}

	//click condition
	if (mouse == MouseEvent::CLICK && past_mouse == MouseEvent::HOVER) {
		//change at click;
		LOG("Button clicked");
		App->gui->EventFocus = parent;
		
		return true;
	}

	if (mouse == MouseEvent::HOVER && past_mouse == MouseEvent::CLICK) {
		//back to hover
		App->gui->events.PushBack(Feedback);
		return true;
	}

	return false;
}


// UI Window ------------------------------------------------------------------------------
UI_Window::UI_Window(iPoint pos, const char* txt, SDL_Rect rect, UI* paren, bool move) : UI(UiType::WINDOW) {
	image = App->gui->CreateUI_Image(rect, pos, &rect, &rect, this, move);

	int height, width;
	App->font->CalcSize(txt, width, height, App->font->default);

	iPoint textpos;
	textpos.x = (pos.x + rect.w / 2) - width / 2;
	textpos.y = pos.y + 10;

	text = App->gui->CreateUI_Text(txt, textpos, txt, txt, this, move);

	focus_Rect.x = pos.x;
	focus_Rect.y = pos.y;
	focus_Rect.w = rect.w;
	focus_Rect.h = rect.h;

	movable = move;
	parent = paren;
	position = pos;
}

void UI_Window::Update() {
	iPoint p = App->render->ScreenToWorld(position.x, position.y);

	focus_Rect.x = p.x;
	focus_Rect.y = p.y;

	image->position = position;

	int height, width;
	App->font->CalcSize(text->text.GetString(), width, height, App->font->default);

	iPoint textpos;
	textpos.x = (position.x + focus_Rect.w / 2) - width / 2;
	textpos.y = position.y + 10;

	text->position = textpos;
}


// UI Input --------------------------------------------------------------------------------
UI_Input::UI_Input(iPoint pos, const char* txt, SDL_Rect rect, UI* paren, bool move) : UI(UiType::INPUT) {
	image = App->gui->CreateUI_Image(rect, pos, &rect, &rect, this, move);

	int height, width;
	App->font->CalcSize(txt, width, height, App->font->default);

	iPoint textpos;
	textpos.x = 24;
	textpos.y = (pos.y + rect.h / 2) - height / 2;

	text = App->gui->CreateUI_Text(txt, textpos, txt, txt, this, move);

	focus_Rect.x = pos.x;
	focus_Rect.y = pos.y;
	focus_Rect.w = rect.w;
	focus_Rect.h = rect.h;

	movable = move;
	parent = paren;
	position = pos;
}

void UI_Input::Update() {
	iPoint p = App->render->ScreenToWorld(position.x, position.y);

	focus_Rect.x = p.x;
	focus_Rect.y = p.y;

	image->position = position;

	int height, width;
	App->font->CalcSize(text->text.GetString(), width, height, App->font->default);

	iPoint textpos;
	textpos.x = position.x + 24;
	textpos.y = (position.y + focus_Rect.h / 2) - height / 2;

	text->position = textpos;

	//pp->render->DrawLine(textpos.x + width, textpos.y, textpos.x + width, textpos.y + height, 255, 255, 255, 100, true);

	if (App->gui->InteractFocus == this) {
		text->text.create(App->input->GetText());
	}
}

bool UI_Input::DoMouseEvent() {
	image->DoMouseEvent();
	text->DoMouseEvent();

	//Hover condition
	if (mouse == MouseEvent::HOVER && past_mouse == MouseEvent::NONE) {
		//change at hover
		return true;
	}

	if (mouse == MouseEvent::NONE && past_mouse == MouseEvent::HOVER) {
		//back to normal
		return true;
	}

	//click condition
	if (mouse == MouseEvent::CLICK && past_mouse == MouseEvent::HOVER) {
		//change at click;
		LOG("Click Input, getting focus");
		App->gui->InteractFocus = this;
		App->input->EnableTextInput();
		return true;
	}

	if (mouse == MouseEvent::HOVER && past_mouse == MouseEvent::CLICK) {
		//back to hover
		return true;
	}

	return false;
}


// UI Scroll Bar -----------------------------------------------------------------------------
UI_ScrollBar::UI_ScrollBar(iPoint pos, SDL_Rect barRect, SDL_Rect pointerRect, int initial_point, Events::ScrollEvents feedback, UI* paren, bool move) : UI(UiType::SCROLLBAR) {
	image_bar = App->gui->CreateUI_Image(barRect, pos, &barRect, &barRect, this, move);

	iPoint pointerpos;

	pointerpos.x = (pos.x + barRect.w / 2) - (pointerRect.w / 2);
	pointerpos.y = pos.y + ((barRect.h / 100) * initial_point - pointerRect.h / 2);

	image_pointer = App->gui->CreateUI_Image(pointerRect, pointerpos, &pointerRect, &pointerRect, this, move);

	focus_Rect.x = pos.x;
	focus_Rect.y = pos.y;
	focus_Rect.w = barRect.w;
	focus_Rect.h = barRect.h;

	bar_pointer = initial_point;

	Feedback = new Events(feedback,initial_point);

	parent = paren;
	movable = move;
	position = pos;

}

void UI_ScrollBar::Update() {
	focus_Rect.x = position.x;
	focus_Rect.y = position.y;


	image_bar->position = position;

	iPoint pointerpos;
	pointerpos.x = (position.x + image_bar->sprite.w / 2) - (image_pointer->sprite.w / 2);
	pointerpos.y = position.y + ((image_bar->sprite.h / 100) * bar_pointer - image_pointer->sprite.h / 2);

	LOG("%i", bar_pointer);

	image_pointer->position = pointerpos;

	if (App->gui->InteractFocus == this) {
		int x, y;
		App->input->GetMousePosition(x, y);
		iPoint p = App->render->ScreenToWorld(x, y);

		bar_pointer = (int)((y) / (image_bar->sprite.h / 100) - position.y) + 136;
	}

	Feedback->volume = bar_pointer;
}

bool UI_ScrollBar::DoMouseEvent() {
	image_bar->DoMouseEvent();
	image_pointer->DoMouseEvent();

	//Hover condition
	if (mouse == MouseEvent::HOVER && past_mouse == MouseEvent::NONE) {
		//change at hover
		return true;
	}

	if (mouse == MouseEvent::NONE && past_mouse == MouseEvent::HOVER) {
		//back to normal
		return true;
	}

	//click condition
	if (mouse == MouseEvent::CLICK && past_mouse == MouseEvent::HOVER) {
		//change at click
		App->gui->InteractFocus = this;
		App->gui->events.PushBack(Feedback);
		return true;
	}

	if (mouse == MouseEvent::HOVER && past_mouse == MouseEvent::CLICK) {
		//back to hover
		App->gui->InteractFocus = nullptr;
		return true;
	}

	return false;
}
