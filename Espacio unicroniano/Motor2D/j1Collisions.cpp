#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Player.h"
#include "j1Window.h"
#include "j1Scene.h"
#include "j1Collisions.h"
#include "p2Log.h"

j1Collisions::j1Collisions()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	//colliders matrix
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_DEAD] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_END] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_CAM_UP] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_CAM_DOWN] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_CAM_LEFT] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_CAM_RIGHT] = true;

	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_DEAD] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_SHOT] = true;

	matrix[COLLIDER_FLOOR][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_WALL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WALL][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_DEAD][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_DEAD][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_END][COLLIDER_PLAYER] = true;

	matrix[COLLIDER_CAM_UP][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_CAM_DOWN][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_CAM_LEFT][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_CAM_RIGHT][COLLIDER_PLAYER] = true;

	matrix[COLLIDER_SHOT][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_SHOT][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_SHOT][COLLIDER_WALL] = true;


	name.create("map");

	
}

j1Collisions::~j1Collisions()
{}

bool j1Collisions::Start() {

	p2List_item<ObjectLayer*>* obj_lay;
	for (obj_lay = App->map->data.obj_layers.start; obj_lay; obj_lay = obj_lay->next) {
		LoadFromObjectLayer(obj_lay->data);
	}

	return true;
}

bool j1Collisions::Awake(pugi::xml_node& config) {
	return true;
}

bool j1Collisions::PreUpdate(float dt) {
	BROFILER_CATEGORY("Collisions_PreUpdate", Profiler::Color::LawnGreen )

	//Remove all colliders scheduled for deletion
	for (uint i = 0; i < MAX_COLLIDERS; ++i) {
		if (colliders[i] != nullptr && colliders[i]->to_delete == true) {
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	//calculate collisions
	Collider* c1;
	Collider* c2;

	for(uint i = 0; i < MAX_COLLIDERS; ++i) 
	{

		//skip empty colliders
		if (colliders[i] == nullptr) {
			continue;
		}

		c1 = colliders[i];

		//avoid checking collisions already checked
		for (uint k = i + 1; k < MAX_COLLIDERS; ++k) {

			//skip empty colliders
			if (colliders[k] == nullptr) {
				continue;
			}

			c2 = colliders[k];

			if(c1->CheckCollision(c2->rect) == true)
			{
				if (matrix[c1->type][c2->type] && c1->callback) {
					c1->callback->OnCollision(c1, c2);
				}

				if (matrix[c2->type][c1->type] && c2->callback) {
					c2->callback->OnCollision(c2, c1);
				}
			}
		}
	}
	return true;
}

bool j1Collisions::Update(float dt)
{
	BROFILER_CATEGORY("Collisions_Update", Profiler::Color::Lime )
	DebugDraw();

	return true;
}

void j1Collisions::DebugDraw() {
	if (App->input->GetKey(SDL_SCANCODE_F9) == KEY_DOWN) {
		debug = !debug;
	}

	if (debug == false) {
		return;
	}
	int x;
	int y;

	App->input->GetMousePosition(x, y);
	iPoint p = App->render->ScreenToWorld(x, y);

	App->render->DrawLine(App->scene->player->position.x, App->scene->player->position.y, p.x, p.y, 255, 255, 255, 100);

	LOG("showing colliders");

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_PLAYER: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
			break;
		case COLLIDER_ENEMY: // red
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
		case COLLIDER_FLOOR: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_WALL:
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 0, alpha);
			break;
		case COLLIDER_END:
			App->render->DrawQuad(colliders[i]->rect, 60, 55, 255, alpha);
			break;
		case COLLIDER_DEAD:
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 0, alpha);
			break;
		case COLLIDER_CAM_DOWN:
		case COLLIDER_CAM_LEFT:
		case COLLIDER_CAM_RIGHT:
		case COLLIDER_CAM_UP:
			App->render->DrawQuad(colliders[i]->rect, 167, 175, 216, alpha);
			break;
		case COLLIDER_SHOT:
			App->render->DrawQuad(colliders[i]->rect, 255, 0, 255, alpha);
			break;
		}
	}
}

bool j1Collisions::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if ((colliders[i] != nullptr)&& (colliders[i]->type != COLLIDER_PLAYER))
		{
			delete colliders[i];
			colliders[i] = nullptr;
		}
	}

	return true;
}

Collider* j1Collisions::AddCollider(SDL_Rect rectC, COLLIDER_TYPE typeC, j1Module* callbackC)
{
	Collider* ret = nullptr;
	for (uint j = 0; j < MAX_COLLIDERS; ++j)
	{
		if (colliders[j] == nullptr)
		{
				ret = colliders[j] = new Collider(rectC, typeC, callbackC);
				break;
		}
	}
		return ret;
}

// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	//return !(r.x + r.w<rect.x || r.x>rect.x + rect.w || r.y + r.h<rect.y || r.y>rect.y + rect.h);
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.h + rect.y > r.y);
}

//-----------------------------------------------------
void Collider::SetPos(int x, int y) {
	rect.x = x;
	rect.y = y;
}

void j1Collisions::DeleteCollider(Collider* collider) {
	if (collider != nullptr) {
		collider->to_delete = true;
	}
}

void j1Collisions::DeleteAll() {
	for (int i = 0; i < MAX_COLLIDERS; i++) {
		if (colliders[i] != nullptr) {
			colliders[i]->to_delete = true;
		}
	}
}

bool j1Collisions::LoadFromObjectLayer(ObjectLayer* layer) {
	if (layer->name == "Colliders") {
		
		p2List_item<ObjectData*>* obj;
		for (obj = layer->list.start; obj; obj = obj->next) {
			COLLIDER_TYPE col_type = COLLIDER_NONE;
			j1Module* callback = nullptr;
			if (obj->data->name == "Floor") {
				col_type = COLLIDER_FLOOR;
				callback = App->map;
			}
			else if (obj->data->name == "Dead") {
				col_type = COLLIDER_DEAD;
				callback = App->map;
			}
			else if (obj->data->name == "End") {
				col_type = COLLIDER_END;
				callback = App->map;
			}
			else if (obj->data->name == "Wall") {
				col_type = COLLIDER_WALL;
				callback = App->map;
			}
			SDL_Rect rect;
			rect.x = obj->data->x;
			rect.y = obj->data->y;
			rect.w = obj->data->w;
			rect.h = obj->data->h;

			AddCollider(rect, col_type, callback);
		}
	}
	return true;
}
