#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Window.h"
#include "j1Collisions.h"
#include "p2Log.h"

j1Collisions::j1Collisions()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	//colliders matrix
	matrix[COLLIDER_PLAYER][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_PLAYER][COLLIDER_WALL] = true;

	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = true;

	matrix[COLLIDER_FLOOR][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_ENEMY][COLLIDER_WALL] = false;

	matrix[COLLIDER_WALL][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_WALL][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_WALL][COLLIDER_FLOOR] = false;
	matrix[COLLIDER_WALL][COLLIDER_WALL] = false;

	name.create("map");
}

j1Collisions::~j1Collisions()
{}

bool j1Collisions::Awake(pugi::xml_node& config) {
	return true;
}

bool j1Collisions::LoadColliders(pugi::xml_node& node) {
	bool ret = true;
	COLLIDER_TYPE coltype;
	p2SString type;

	pugi::xml_node objectgroup;
	for (objectgroup = node.child("objectgroup"); objectgroup && ret; objectgroup = objectgroup.next_sibling("objectgroup"))
	{
		pugi::xml_node object;
		for (object = objectgroup.child("object"); object && ret; object = object.next_sibling("object")) {

			SDL_Rect rect;
			type = object.attribute("type").as_string();
			if (type == "floor")
			{
				coltype = COLLIDER_FLOOR;
				LOG("Colider floor");
			}
			else if (type == "wall") {
				coltype = COLLIDER_WALL;
				LOG("collider wall");
			}
			else
			{
				LOG("Collider type undefined");
				continue;
			}

			rect.x = object.attribute("x").as_uint();
			rect.y = object.attribute("y").as_uint();
			rect.w = object.attribute("width").as_uint();
			rect.h = object.attribute("heigth").as_uint();

			Collider* col = AddCollider(rect, coltype);
			App->map->data.colliders.add(col);
				LOG("%i x %i", rect.x, rect.y);
		}
	}
	return true;
}

bool j1Collisions::PreUpdate() {
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
	DebugDraw();

	return true;
}

void j1Collisions::DebugDraw() {
	if (App->input->GetKey(SDL_SCANCODE_F1) == KEY_DOWN) {
		debug = !debug;
	}

	if (debug == false) {
		return;
	}

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
		}
	}
}

bool j1Collisions::CleanUp()
{
	LOG("Freeing all colliders");

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] != nullptr)
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

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rectC, typeC, callbackC);
			break;
		}
	}

	return ret;
}

// -----------------------------------------------------

bool Collider::CheckCollision(const SDL_Rect& r) const
{
	return (rect.x < r.x + r.w &&
		rect.x + rect.w > r.x &&
		rect.y < r.y + r.h &&
		rect.h + rect.y > r.y);
}

//-----------------------------------------------------

void j1Collisions::DeleteCollider(Collider* collider) {
	if (collider != nullptr) {
		collider->to_delete = true;
	}
}