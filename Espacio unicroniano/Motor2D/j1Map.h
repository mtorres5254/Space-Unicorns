#ifndef __j1MAP_H__
#define __j1MAP_H__

#include "PugiXml/src/pugixml.hpp"
#include "p2List.h"
#include "p2Point.h"
#include "j1Module.h"

#include "SDL/include/SDL.h"

// ----------------------------------------------------

struct Properties
{
	struct Property
	{
		p2SString name;
		p2SString type;
		float f_value;
		int value;
	};

	~Properties()
	{
		p2List_item<Property*>* item;
		item = list.start;

		while(item != NULL)
		{
			RELEASE(item->data);
			item = item->next;
		}

		list.clear();
	}

	int Get(const char* name, int default_value = 0) const;
	float GetFloat(const char* name, int default_value = 0) const;

	p2List<Property*>	list;
};

// ----------------------------------------------------
struct MapLayer
{
	p2SString	name;
	int			width;
	int			height;
	uint*		data;
	Properties	properties;

	MapLayer() : data(NULL)
	{}

	~MapLayer()
	{
		RELEASE(data);
	}

	inline uint Get(int x, int y) const
	{
		return data[(y*width) + x];
	}
};

struct ImageLayer {
	p2SString name;
	int offsetx;
	int offsety;
	SDL_Texture* text;
	int img_width;
	int img_height;
	Properties	properties;
};

// ----------------------------------------------------

struct ObjectData {
	p2SString name;
	int x;
	int y;
	int w;
	int h;
};

struct ObjectLayer {
	p2SString name;
	p2List<ObjectData*> list;
};

struct TileSet
{
	SDL_Rect GetTileRect(int id) const;

	p2SString			name;
	int					firstgid;
	int					margin;
	int					spacing;
	int					tile_width;
	int					tile_height;
	SDL_Texture*		texture;
	int					tex_width;
	int					tex_height;
	int					num_tiles_width;
	int					num_tiles_height;
	int					offset_x;
	int					offset_y;
};

enum MapTypes
{
	MAPTYPE_UNKNOWN = 0,
	MAPTYPE_ORTHOGONAL,
	MAPTYPE_ISOMETRIC,
	MAPTYPE_STAGGERED
};
// ----------------------------------------------------
struct MapData
{
	int					 width;
	int					 height;
	int					 tile_width;
	int					 tile_height;
	SDL_Color			 background_color;
	MapTypes			 type;
	p2List<TileSet*>	 tilesets;
	p2List<MapLayer*>	 layers;
	p2List<ImageLayer*>  img_layers;
	p2List<ObjectLayer*> obj_layers;
};

// ----------------------------------------------------
class j1Map : public j1Module
{
public:

	j1Map();

	// Destructor
	virtual ~j1Map();

	// Called before render is available
	bool Awake(pugi::xml_node& conf);

	// Called each loop iteration
	void Draw();

	// Called before quitting
	bool CleanUp();

	// Load new map
	bool Load(const char* path);

	bool Load(pugi::xml_node&);
	bool Save(pugi::xml_node&) const;

	iPoint MapToWorld(int x, int y) const;
	iPoint WorldToMap(int x, int y) const;
	bool CreateWalkabilityMap(int& width, int& height, uchar** buffer) const;

private:

	bool LoadMap();
	bool LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set);
	bool LoadLayer(pugi::xml_node& node, MapLayer* layer);
	bool LoadProperties(pugi::xml_node& node, Properties& properties);
	bool LoadImgLayer(pugi::xml_node& node, ImageLayer* layer);
	bool LoadObjLayer(pugi::xml_node& node, ObjectLayer* layer);
	bool Culling(int x, int y);

	TileSet* GetTilesetFromTileId(int id) const;

public:

	MapData data;

private:

	pugi::xml_document	map_file;
	p2SString			folder;
	p2SString			map_loadded_string;
	bool				map_loaded;
};

#endif // __j1MAP_H__