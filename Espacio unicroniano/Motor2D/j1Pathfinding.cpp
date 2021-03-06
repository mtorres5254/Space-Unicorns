#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Map.h"
#include "j1PathFinding.h"

j1PathFinding::j1PathFinding() : j1Module(), map(NULL), last_path(DEFAULT_PATH_LENGTH),width(0), height(0)
{
	name.create("pathfinding");
}

// Destructor
j1PathFinding::~j1PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool j1PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	last_path.Clear();
	RELEASE_ARRAY(map);
	return true;
}

// Sets up the walkability map
void j1PathFinding::SetMap(uint width, uint height, uchar* data)
{
	this->width = width;
	this->height = height;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool j1PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool j1PathFinding::IsWalkable(const iPoint& pos, bool Fly) const
{
	uchar t = GetTileAt(pos);
	if (Fly == true) {
		return t != INVALID_WALK_CODE && t > 0;
	}
	else
	{
		return t != INVALID_WALK_CODE && t == 2;
	}
	
}

// Utility: return the walkability value of a tile
uchar j1PathFinding::GetTileAt(const iPoint& pos) const
{
	if(CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const p2DynArray<Path>* j1PathFinding::GetLastPath() const
{
	return &last_path;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::Find(const iPoint& point) const
{
	p2List_item<PathNode>* item = list.start;
	while(item)
	{
		if(item->data.pos == point)
			return item;
		item = item->next;
	}
	return NULL;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2List_item<PathNode>* PathList::GetNodeLowestScore() const
{
	p2List_item<PathNode>* ret = NULL;
	int min = 65535;

	p2List_item<PathNode>* item = list.end;
	while(item)
	{
		if(item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}
	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent, direction nextNodeDir) : g(g), h(h), pos(pos), parent(parent), nextNodeDir(nextNodeDir)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& list_to_fill) const
{
	iPoint cell;
	uint before = list_to_fill.list.count();

	// north
	cell.create(pos.x, pos.y + 1);
	if(App->pathfinding->IsWalkable(cell, App->pathfinding->fly_path))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, DIR_UP));

	// south
	cell.create(pos.x, pos.y - 1);
	if(App->pathfinding->IsWalkable(cell, App->pathfinding->fly_path))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, DIR_DOWN));

	// east
	cell.create(pos.x + 1, pos.y);
	if(App->pathfinding->IsWalkable(cell, App->pathfinding->fly_path))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, DIR_RIGHT));

	// west
	cell.create(pos.x - 1, pos.y);
	if(App->pathfinding->IsWalkable(cell, App->pathfinding->fly_path))
		list_to_fill.list.add(PathNode(-1, -1, cell, this, DIR_LEFT));

	return list_to_fill.list.count();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceTo(destination);

	return g + h;
}

// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int j1PathFinding::CreatePath(const iPoint& origin, const iPoint& destination, bool Fly)
{
	BROFILER_CATEGORY("Pathfinding_Algorithm", Profiler::Color::Gold )

		//clear last path
		last_path.Clear();

		// 1: if origin or destination are not walkable, return -1
		if (IsWalkable(origin, Fly) == false || IsWalkable(destination, Fly) == false) {
			return -1;
		}

		fly_path = Fly;

		// 2: Create two lists: open, close
		// Add the origin tile to open
		// Iterate while we have tile in the open list
		PathList open;
		PathList close;

		PathNode originNode(0, origin.DistanceManhattan(destination), origin, NULL, DIR_NONE);
		open.list.add(originNode);

		while (open.list.count() > 0) {

			//  3: Move the lowest score cell from open list to the closed list
			p2List_item<PathNode>* NodeVisited = open.GetNodeLowestScore();
			close.list.add(NodeVisited->data);
			open.list.del(NodeVisited);

			//  4: If we just added the destination, we are done!
			// Backtrack to create the final path
			// Use the Pathnode::parent and Flip() the path when you are finish
			p2List_item<PathNode>* Node = close.list.end;
			if (Node->data.pos == destination && Fly) {
				Path FinalPath(Node->data.pos.x, Node->data.pos.y, Node->data.nextNodeDir);
				last_path.PushBack(FinalPath);

				const PathNode* PrevNode;
				const PathNode* AuxNode;
				PrevNode = &Node->data;

				for (AuxNode = PrevNode->parent; AuxNode; AuxNode = AuxNode->parent) {
					Path AuxPath(AuxNode->pos.x, AuxNode->pos.y, AuxNode->nextNodeDir);
					if (AuxNode->nextNodeDir != PrevNode->nextNodeDir) {
						last_path.PushBack(AuxPath);
					}


					PrevNode = AuxNode;
				}
				last_path.Flip();
				return 1;

			}
			else if (Node->data.pos.x == destination.x && !Fly) {
				Path FinalPath(Node->data.pos.x, Node->data.pos.y, Node->data.nextNodeDir);
				last_path.PushBack(FinalPath);

				const PathNode* PrevNode;
				const PathNode* AuxNode;
				PrevNode = &Node->data;

				for (AuxNode = PrevNode->parent; AuxNode; AuxNode = AuxNode->parent) {
					if (AuxNode->nextNodeDir != PrevNode->nextNodeDir) {
						Path AuxPath(AuxNode->pos.x, AuxNode->pos.y, AuxNode->nextNodeDir);
						last_path.PushBack(AuxPath);
					}

					PrevNode = AuxNode;
				}
				last_path.Flip();
				return 1;
			}

			//  5: Fill a list of all adjancent nodes
			PathList adjacent;
			Node->data.FindWalkableAdjacents(adjacent);
			p2List_item<PathNode>* AdjacentNode = adjacent.list.start;

			//  6: Iterate adjancent nodes:
			// ignore nodes in the closed list
			// If it is NOT found, calculate its F and add it to the open list
			// If it is already in the open list, check if it is a better path (compare G)
			// If it is a better path, Update the parent
			while (AdjacentNode != NULL) {
				AdjacentNode->data.CalculateF(destination);
				if (close.Find(AdjacentNode->data.pos) != NULL) {
					AdjacentNode = AdjacentNode->next;
					continue;
				}

				if (open.Find(AdjacentNode->data.pos) != NULL) {
					if (AdjacentNode->data.g < open.Find(AdjacentNode->data.pos)->data.g) {
						open.Find(AdjacentNode->data.pos)->data.g = AdjacentNode->data.pos.DistanceTo(origin);

						AdjacentNode = AdjacentNode->next;
						continue;
					}
				}
				else {
					PathNode NextNode(AdjacentNode->data.g, AdjacentNode->data.h, AdjacentNode->data.pos, AdjacentNode->data.parent, AdjacentNode->data.nextNodeDir);
					open.list.add(NextNode);

					AdjacentNode = AdjacentNode->next;
					continue;
				}

				AdjacentNode = AdjacentNode->next;
			}
		}
		return -1;
}

