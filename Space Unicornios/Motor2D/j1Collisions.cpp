#include "j1App.h"
#include "j1Input.h"
#include "j1Render.h"
#include "j1Collisions.h"

j1Collisions::j1Collisions()
{
	for (uint i = 0; i < MAX_COLLIDERS; ++i)
		colliders[i] = nullptr;

	//colliders matrix
}

j1Collisions::~j1Collisions()
{}

bool j1Collisions::PreUpdate() {
	//Remove all colliders scheduled for deletion
}