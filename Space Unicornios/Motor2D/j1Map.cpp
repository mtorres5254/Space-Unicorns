#include "p2Defs.h"
#include "p2Log.h"
#include "j1App.h"
#include "j1Render.h"
#include "j1Textures.h"
#include "j1Map.h"
#include "j1Collisions.h"
#include "j1Player.h"
#include <math.h>

j1Map::j1Map() : j1Module(), map_loaded(false)
{
	name.create("map");
}

// Destructor
j1Map::~j1Map()
{}

// Called before render is available
bool j1Map::Awake(pugi::xml_node& config)
{
	LOG("Loading Map Parser");
	bool ret = true;

	folder.create(config.child("folder").child_value());

	return ret;
}

void j1Map::Draw()
{
	if(map_loaded == false)
		return;

	//(old): Prepare the loop to draw all tilesets + Blit
	p2List_item<MapLayer*>* layer = data.layers.start; // for now we just use the first layer and tileset
	p2List_item<CustomProperties*>* prop = layer->data->properties.start;

	p2List_item<ImageLayer*>* img_layer = data.img_layers.start;
	//(old): Complete the draw function
	int layer_height = layer->data->height;
	int layer_width = layer->data->width;

	while (img_layer != NULL) {
		App->render->Blit(img_layer->data->text, img_layer->data->offsetx, img_layer->data->offsety);
		img_layer = img_layer->next;
	}
	while (layer != NULL) {
			int pos = 0;
			for (int h = 0; h < layer_height; h++)
			{
				for (int w = 0; w < layer_width; w++)
				{
					if (layer->data->data[pos] == 0) {
						pos++;
						continue;
					}
					p2List_item<TileSet*>* tileset = GetTilset(layer->data->data[pos]);
					SDL_Rect section = tileset->data->GetTileRect(layer->data->data[pos]);
					iPoint position = MapToWorld(w, h);
					App->render->Blit(tileset->data->texture, position.x, position.y, &section);
					pos++;
				}
			}
		layer = layer->next;
	}
}

p2List_item<TileSet*>* j1Map::GetTilset(int id) {
	p2List_item<TileSet*>* tileset = data.tilesets.end;

	while (tileset != NULL) {
		if (id >= tileset->data->firstgid) {
			return tileset;
		}
		tileset = tileset->prev;
	}

}

iPoint j1Map::MapToWorld(int x, int y) const
{
	iPoint ret(0,0);
	//(old): Create a method that translates x,y coordinates from map positions to world positions
	if (data.type == MAPTYPE_ORTHOGONAL) {
		ret.x = x * data.tile_width;
		ret.y = y * data.tile_height;
	}	
	//: Add isometric map to world coordinates
	if (data.type == MAPTYPE_ISOMETRIC) {
		ret.x = (x - y)*(data.tile_width * 0.5f);
		ret.y = (x + y)*(data.tile_height * 0.5f);
	}
	return ret;
}


iPoint j1Map::WorldToMap(int x, int y) const
{
	iPoint ret(0,0);
	// TODO 2: Add orthographic world to map coordinates
	if (data.type == MAPTYPE_ORTHOGONAL) {
		ret.x = x / data.tile_width;
		ret.y = y / data.tile_height;
	}
	// TODO 3: Add the case for isometric maps to WorldToMap
	if (data.type == MAPTYPE_ISOMETRIC) {
		ret.x = x / data.tile_width + y / data.tile_height;
		ret.y = y / data.tile_height - x / data.tile_width;
	}
	return ret;
}

SDL_Rect TileSet::GetTileRect(int id) const
{
	SDL_Rect Section = {0, 0, 0, 0};
	//(old): Create a method that receives a tile id and returns it's Rectç

	int h = tile_height;
	int w = tile_width;
	Section.h = h;
	Section.w = w;
	int y = (id - 1) / num_tiles_width;
	int x = (id - 1) % num_tiles_width;
	//LOG("%i x %i", y, x);
	//LOG("%i x %i",tileset->data->num_tiles_width, tileset->data->num_tiles_height);
	Section.x = (margin * x) + (x * w);
	Section.y = (spacing * y) + (y * h);
	//LOG("%i %i %i %i",Section.x, Section.y, Section.w, Section.h);

	return Section;
}

// Called before quitting
bool j1Map::CleanUp()
{
	LOG("Unloading map");

	// Remove all tilesets
	p2List_item<TileSet*>* item;
	item = data.tilesets.start;

	while(item != NULL)
	{
		RELEASE(item->data);
		item = item->next;
	}
	data.tilesets.clear();

	// Remove all layers
	p2List_item<MapLayer*>* item2;
	item2 = data.layers.start;

	while(item2 != NULL)
	{
		RELEASE(item2->data);
		item2 = item2->next;
	}
	data.layers.clear();

	// Clean up the pugui tree
	map_file.reset();

	return true;
}

// Load new map
bool j1Map::Load(const char* file_name)
{
	bool ret = true;
	p2SString tmp("%s%s", folder.GetString(), file_name);

	pugi::xml_parse_result result = map_file.load_file(tmp.GetString());

	if(result == NULL)
	{
		LOG("Could not load map xml file %s. pugi error: %s", file_name, result.description());
		ret = false;
	}

	// Load general info ----------------------------------------------
	if(ret == true)
	{
		ret = LoadMap();
	}

	// Load all tilesets info ----------------------------------------------
	pugi::xml_node tileset;
	for(tileset = map_file.child("map").child("tileset"); tileset && ret; tileset = tileset.next_sibling("tileset"))
	{
		TileSet* set = new TileSet();

		if(ret == true)
		{
			ret = LoadTilesetDetails(tileset, set);
		}

		if(ret == true)
		{
			ret = LoadTilesetImage(tileset, set);
		}

		data.tilesets.add(set);
	}

	// Load layer info ----------------------------------------------
	pugi::xml_node layer;
	for(layer = map_file.child("map").child("layer"); layer && ret; layer = layer.next_sibling("layer"))
	{
		MapLayer* lay = new MapLayer();

		ret = LoadLayer(layer, lay);

		if(ret == true)
			data.layers.add(lay);
	}
	//Load images layer info ----------------------------------------
	pugi::xml_node img_layer;
	for (img_layer = map_file.child("map").child("imagelayer"); img_layer && ret; img_layer = img_layer.next_sibling("imagelayer")) {

		ImageLayer* img_lay = new ImageLayer();

		ret = LoadImgLayer(img_layer, img_lay);

		if (ret == true)
			data.img_layers.add(img_lay);
	}

	//Load colliders info -------------------------------------------
	if (ret == true) {
		ret = App->col->LoadColliders(map_file.child("map"));
	}

	if(ret == true)
	{
		LOG("Successfully parsed map XML file: %s", file_name);
		LOG("width: %d height: %d", data.width, data.height);
		LOG("tile_width: %d tile_height: %d", data.tile_width, data.tile_height);
		LOG("");

		p2List_item<TileSet*>* item = data.tilesets.start;
		while(item != NULL)
		{
			TileSet* s = item->data;
			LOG("Tileset ----");
			LOG("name: %s firstgid: %d", s->name.GetString(), s->firstgid);
			LOG("tile width: %d tile height: %d", s->tile_width, s->tile_height);
			LOG("spacing: %d margin: %d", s->spacing, s->margin);
			
			item = item->next;
		}

		p2List_item<MapLayer*>* item_layer = data.layers.start;
		while(item_layer != NULL)
		{
			MapLayer* l = item_layer->data;
			LOG("Layer ----");
			LOG("name: %s", l->name.GetString());
			LOG("tile width: %d tile height: %d", l->width, l->height);
			LOG("");
			item_layer = item_layer->next;
		}
	}

	map_loaded = ret;

	return ret;
}

// Load map general properties
bool j1Map::LoadMap()
{
	bool ret = true;
	pugi::xml_node map = map_file.child("map");

	if (map == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'map' tag.");
		ret = false;
	}
	else
	{
		data.width = map.attribute("width").as_int();
		data.height = map.attribute("height").as_int();
		data.tile_width = map.attribute("tilewidth").as_int();
		data.tile_height = map.attribute("tileheight").as_int();
		p2SString bg_color(map.attribute("backgroundcolor").as_string());

		data.background_color.r = 0;
		data.background_color.g = 0;
		data.background_color.b = 0;
		data.background_color.a = 0;

		if (bg_color.Length() > 0)
		{
			p2SString red, green, blue;
			bg_color.SubString(1, 2, red);
			bg_color.SubString(3, 4, green);
			bg_color.SubString(5, 6, blue);

			int v = 0;

			sscanf_s(red.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.r = v;

			sscanf_s(green.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.g = v;

			sscanf_s(blue.GetString(), "%x", &v);
			if (v >= 0 && v <= 255) data.background_color.b = v;
		}

		p2SString orientation(map.attribute("orientation").as_string());

		if (orientation == "orthogonal")
		{
			data.type = MAPTYPE_ORTHOGONAL;
		}
		else if (orientation == "isometric")
		{
			data.type = MAPTYPE_ISOMETRIC;
		}
		else if (orientation == "staggered")
		{
			data.type = MAPTYPE_STAGGERED;
		}
		else
		{
			data.type = MAPTYPE_UNKNOWN;
		}
	}

	return ret;
}

bool j1Map::LoadTilesetDetails(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	if (tileset_node.attribute("firstgid").as_int() != NULL) {
		set->firstgid = tileset_node.attribute("firstgid").as_int();
	}
		
	p2SString source = tileset_node.attribute("source").as_string();
	//LOG("%s", source.GetString());
	if (source.Length() > 1) {
		LOG("Loading tileset from another source");
		pugi::xml_document source_doc;
		p2SString tmp("%s%s", folder.GetString(), source.GetString());
		pugi::xml_parse_result tileset_result = source_doc.load_file(tmp.GetString());
		if (tileset_result == NULL)
		{
			LOG("Could not load map xml file %s. pugi error: %s", source.GetString(), tileset_result.description());
			ret = false;
		}
		pugi::xml_node tileset_extern = source_doc.child("tileset");
		ret = LoadTilesetDetails(tileset_extern, set);
		return ret;
	}
	else {
		set->name.create(tileset_node.attribute("name").as_string());
		set->tile_width = tileset_node.attribute("tilewidth").as_int();
		set->tile_height = tileset_node.attribute("tileheight").as_int();
		set->margin = tileset_node.attribute("margin").as_int();
		set->spacing = tileset_node.attribute("spacing").as_int();
		pugi::xml_node offset = tileset_node.child("tileoffset");

		if (offset != NULL)
		{
			set->offset_x = offset.attribute("x").as_int();
			set->offset_y = offset.attribute("y").as_int();
		}
		else
		{
			set->offset_x = 0;
			set->offset_y = 0;
		}

		return ret;
	}	
}

bool j1Map::LoadTilesetImage(pugi::xml_node& tileset_node, TileSet* set)
{
	bool ret = true;
	p2SString source = tileset_node.attribute("source").as_string();
	LOG("%s", source.GetString());
	if (source.Length() > 1 ) {
		LOG("Loading tileset from another source");
		pugi::xml_document source_doc;
		p2SString tmp("%s%s", folder.GetString(), source.GetString());
		LOG("%s", tmp.GetString());
		pugi::xml_parse_result tileset_result = source_doc.load_file(tmp.GetString());
		if (tileset_result == NULL)
		{
			LOG("Could not load map xml file %s. pugi error: %s", source.GetString(), tileset_result.description());
			ret = false;
		}
		pugi::xml_node tileset_extern = source_doc.child("tileset");
		ret = LoadTilesetImage(tileset_extern, set);
		return ret;
	}
	else {
		pugi::xml_node image = tileset_node.child("image");

		if (image == NULL)
		{
			LOG("Error parsing tileset xml file: Cannot find 'image' tag.");
			ret = false;
		}
		else
		{
			set->texture = App->tex->Load(PATH(folder.GetString(), image.attribute("source").as_string()));
			int w, h;
			SDL_QueryTexture(set->texture, NULL, NULL, &w, &h);
			set->tex_width = image.attribute("width").as_int();

			if (set->tex_width <= 0)
			{
				set->tex_width = w;
			}

			set->tex_height = image.attribute("height").as_int();

			if (set->tex_height <= 0)
			{
				set->tex_height = h;
			}

			set->num_tiles_width = set->tex_width / set->tile_width;
			set->num_tiles_height = set->tex_height / set->tile_height;
		}

		return ret;
	}
}

bool j1Map::LoadLayer(pugi::xml_node& node, MapLayer* layer)
{
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	layer->width = node.attribute("width").as_int();
	layer->height = node.attribute("height").as_int();

	pugi::xml_node properties_data = node.child("properties");

	if (properties_data == NULL)
	{
		LOG("There aren't custom properties");
	}
	else {
		LOG("loading custom properties");
		for (pugi::xml_node property_node = properties_data.child("property"); property_node; property_node = property_node.next_sibling("property")) {
			CustomProperties tmpProp;
			tmpProp.name = property_node.attribute("name").as_string();
			tmpProp.type = property_node.attribute("type").as_string();
			if (tmpProp.type.GetString() == "bool") {
				tmpProp.b_data = property_node.attribute("value").as_string();
			}
			if (tmpProp.type.GetString() == "int") {
				tmpProp.ui_data = property_node.attribute("value").as_uint();
			}
			layer->properties.add(&tmpProp);
		}
	}

	pugi::xml_node layer_data = node.child("data");

	if(layer_data == NULL)
	{
		LOG("Error parsing map xml file: Cannot find 'layer/data' tag.");
		ret = false;
		RELEASE(layer);
	}
	else
	{
		layer->data = new uint[layer->width*layer->height];
		memset(layer->data, 0, layer->width*layer->height);

		int i = 0;
		for(pugi::xml_node tile = layer_data.child("tile"); tile; tile = tile.next_sibling("tile"))
		{
			layer->data[i++] = tile.attribute("gid").as_int(0);
		}
	}

	return ret;
}

bool j1Map::LoadImgLayer(pugi::xml_node& node, ImageLayer* layer) {
	bool ret = true;

	layer->name = node.attribute("name").as_string();
	p2SString image;
	
	image = node.child("image").attribute("source").as_string();
	p2SString tmp("%s%s", folder.GetString(), image.GetString());

	layer->text = App->tex->Load(tmp.GetString());
	layer->img_width = node.child("image").attribute("width").as_int();
	layer->img_height = node.child("image").attribute("height").as_int();
	layer->offsetx = node.attribute("offsetx").as_int();
	layer->offsety = node.attribute("offsety").as_int();

	LOG("LOADING IMG LAYER:------------------");
	LOG("NAME: %s", layer->name.GetString());
	LOG("TEXT: %s. img: %i x %i", image.GetString(), layer->img_width, layer->img_height);
	LOG("offset x: %i y: %i", layer->offsetx, layer->offsety);

	return ret;
}

void j1Map::OnCollision(Collider* c1, Collider* c2) {
	if (c1->type == COLLIDER_FLOOR && c2->type == COLLIDER_PLAYER) {
		if (c2->rect.y < c1->rect.y) {
			if (c1->rect.y < c2->rect.y + c2->rect.h - 3) {
				if (c1->rect.x < c2->rect.x + c2->rect.w && c1->rect.x + c1->rect.w / 2 > c2->rect.x) {
					App->player->position.x = c1->rect.x - c2->rect.w;
				}
				if (c1->rect.x + c1->rect.w > c2->rect.x && c1->rect.x + c1->rect.w / 2 < c2->rect.x) {
					App->player->position.x = c1->rect.x + c1->rect.w;
				}
			}
		}
		else {
			if (c1->rect.x < c2->rect.x + c2->rect.w && c1->rect.x + c1->rect.w / 2 > c2->rect.x) {
				App->player->position.x = c1->rect.x - c2->rect.w;
			}
			if (c1->rect.x + c1->rect.w > c2->rect.x && c1->rect.x + c1->rect.w / 2 < c2->rect.x) {
				App->player->position.x = c1->rect.x + c1->rect.w;
			}
		}
	}
}