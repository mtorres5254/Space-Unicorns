#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
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

	matrix[COLLIDER_ENEMY][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_FLOOR] = true;
	matrix[COLLIDER_ENEMY][COLLIDER_ENEMY] = true;

	matrix[COLLIDER_FLOOR][COLLIDER_PLAYER] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_ENEMY] = true;
	matrix[COLLIDER_FLOOR][COLLIDER_FLOOR] = true;

	name.create("map");
}

j1Collisions::~j1Collisions()
{}

bool j1Collisions::Awake(pugi::xml_node& config) {
	LOG("Loading colliders");
	
	folder.create(config.child("folder").child_value());
}

bool j1Collisions::Load(const char* file_name) {
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = collisions_data.load_file(tmp.GetString());

	if (result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}
	//Load Collider

	pugi::xml_node objectgroup;
	for (objectgroup = collisions_data.child("map").child("objectgroup"); objectgroup && ret; objectgroup = objectgroup.next_sibling("objectgroup"))
	{
		pugi::xml_node object;
		for (object = objectgroup.child("object"); object && ret; object = object.next_sibling("object")) {
			SDL_Rect rect;
			rect.x = object.attribute("x").as_uint();
			rect.y = object.attribute("y").as_uint();
			rect.w = object.attribute("width").as_uint();
			rect.h = object.attribute("heigth").as_uint();

			COLLIDER_TYPE coltype;
			if (object.attribute("name").as_string() == "Floor") {
				coltype = COLLIDER_FLOOR;
				//j1Module* module_ptr = App->map;

				AddCollider(rect, coltype);
			}
		}
	}
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

bool j1Collisions::Update()
{
	DebugDraw();

	return true;
}

void j1Collisions::DebugDraw() {
	if (App->input->GetKey(SDL_SCANCODE_F1) ==  KEY_DOWN)
		debug = !debug;

	if (debug == false)
		return;

	Uint8 alpha = 80;
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
			continue;

		switch (colliders[i]->type)
		{
		case COLLIDER_NONE: // white
			App->render->DrawQuad(colliders[i]->rect, 255, 255, 255, alpha);
			break;
		case COLLIDER_PLAYER: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_ENEMY: // blue
			App->render->DrawQuad(colliders[i]->rect, 0, 0, 255, alpha);
			break;
		case COLLIDER_FLOOR: // green
			App->render->DrawQuad(colliders[i]->rect, 0, 255, 0, alpha);
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

Collider* j1Collisions::AddCollider(SDL_Rect rect, COLLIDER_TYPE typeC, j1Module* callbackC = nullptr)
{
	Collider* ret = nullptr;

	for (uint i = 0; i < MAX_COLLIDERS; ++i)
	{
		if (colliders[i] == nullptr)
		{
			ret = colliders[i] = new Collider(rect, typeC, callbackC);
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