#include "map.h"
#include "utils.h"
#include <vector>

using namespace std;


void MapInit(Map* map, PPMImage *map_data, int nodes_count)
{
	map->map_data = map_data;
	map->nodes = NULL;
	map->nodes_count = 0;
	map->energie = 100;
	PPMPixelInit(&map->in_color, 0,255,0);
	PPMPixelInit(&map->out_color, 255,0,0);
	PPMPixelInit(&map->construction_color, 255,255,0);
	PPMPixelInit(&map->way_color, 255,255,255);
	PPMPixelInit(&map->node_color, 0,0,0);
	MapInitNodes(map, nodes_count);
}


void MapInitNodes(Map* map, int nodes_count)
{
	if (map->nodes)
	{
		for (int i=0; i<nodes_count; ++i)
		{
			DELETE(map->nodes[i]);
		}
		DELETE(map->nodes);
	}
	map->nodes_count = nodes_count;
	if (!nodes_count)
		return;
	NEW_ARRAY(map->nodes, Node*, nodes_count);
	for (int i=0; i<nodes_count; ++i)
	{
		NEW_N_INIT(map->nodes[i], Node);
		map->nodes[i]->id = i;
	}
}

void NodeInit(Node *node)
{
	node->connected = NULL;
	node->type = STANDARD;
	node->x = node->y = 0;
}

void NodeListInit(NodeList *list, Node *node)
{
	list->node = node;
	list->next = NULL;
}
			
void NodeAddConnection(Node *origin, Node *destination)
{
	NodeList* newNode;
	NEW_N_INIT(newNode, NodeList, destination);
	newNode->next = origin->connected;
	origin->connected = newNode;
}

void MapAddConnection(Map *map, int origin_index, int destination_index)
{
	NodeAddConnection(map->nodes[origin_index], map->nodes[destination_index]);
}

void MapSetValue(Map *map, const char *attribute_name, const int *values, int values_count)
{
	if (strcmp(attribute_name, "energie") == 0)
	{
		map->energie = values[0];
	}
	else if (strcmp(attribute_name, "construct") == 0)
	{
		PPMPixelInit(&map->construction_color, values[0], values[1], values[2]);
	}
	else if (strcmp(attribute_name, "chemin") == 0)
	{
		PPMPixelInit(&map->way_color, values[0], values[1], values[2]);
	}
	else if (strcmp(attribute_name, "in") == 0)
	{
		PPMPixelInit(&map->in_color, values[0], values[1], values[2]);
	}
	else if (strcmp(attribute_name, "out") == 0)
	{
		PPMPixelInit(&map->out_color, values[0], values[1], values[2]);
	}
	else if (strcmp(attribute_name, "noeud") == 0)
	{
		PPMPixelInit(&map->node_color, values[0], values[1], values[2]);
	}
	else
	{
		warning("The given attribute does not exist: %s", attribute_name);
	}
}

void PPMPixelInit(PPMPixel* p, byte r, byte g, byte b)
{
	p->red = r; p->green = g; p->blue = b;
}

const PPMPixel *PPMImageGetPixel(const PPMImage *image, int x, int y)
{
	int index = y*image->x+x;
	if (x >= image->x or y >= image->y)
	{
		fprintf(stderr, "Cannot access to this pixel (%d, %d)", x, y);
		return NULL;
	}
	return &image->data[index];
}

const PPMPixel *MapGetPixel(const Map *map, int x, int y)
{
	return PPMImageGetPixel(map->map_data, x, y);
}

PPMPixel MapGetNodeTypeColor(const Map *map, int node_type)
{
	PPMPixel result;
	switch (node_type) {
	case INTERSECTION:
	case STANDARD:
		return map->node_color;
	case START:
		return map->in_color;
	case END:
		return map->out_color;
	default:
		warning("the given type %d is not referenced", node_type);
		PPMPixelInit(&result, 0,0,0);
		break;
	}
	return result;
}

bool PPMPixelEqual(const PPMPixel *p1, const PPMPixel *p2)
{
	return p1->blue == p2->blue && p1->green == p2->green &&
			p1->red == p1->red;
}

bool PPMPixelEqual2(const PPMPixel *p1, const PPMPixel *p2, unsigned int error_margin) //fonction qui vérifie lécart entre deux couleurs
{
	unsigned int diff = abs(p1->blue-p2->blue) +
			abs(p1->green-p2->green) +
			abs(p1->red-p1->red);
	return diff < error_margin;
}
