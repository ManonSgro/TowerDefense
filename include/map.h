#ifndef MAP_H
#define MAP_H

typedef unsigned char byte;

enum NodeType
{
	START=1,
	END,
	STANDARD,
	INTERSECTION
};

typedef struct {
	 byte red,green,blue;
} PPMPixel;

typedef struct {
	 int x, y;
	 PPMPixel *data;
} PPMImage;

struct Node;
struct NodeList;

struct NodeList{
	struct Node* node;
	struct NodeList* next;
};

typedef struct NodeList NodeList;

struct Node{
	int x, y;
	int type;
	int id;
	NodeList* connected;
	//Node** successors;
	//int successors_size;
};

typedef struct Node Node;

typedef struct
{
	PPMImage* map_data;
	Node** nodes;
	int nodes_count;

	int energie;
	PPMPixel way_color;
	PPMPixel node_color;
	PPMPixel construction_color;
	PPMPixel in_color;
	PPMPixel out_color;
} Map;

void PPMPixelInit(PPMPixel*p, byte r, byte g, byte b);
bool PPMPixelEqual(const PPMPixel* p1, const PPMPixel* p2);
bool PPMPixelEqual2(const PPMPixel* p1, const PPMPixel* p2, unsigned int error_margin);
const PPMPixel* PPMImageGetPixel(const PPMImage* image, int x, int y);
void NodeListInit(NodeList* list, Node* node);
void NodeInit(Node* node);
void NodeAddConnection(Node* origin, Node* destination);
void MapInit(Map* map, PPMImage* map_data, int nodes_count);
void MapInitNodes(Map* map, int nodes_count);
void MapAddConnection(Map* map, int origin_index, int destination_index);
void MapSetValue(Map* map, const char* attribute_name, const int* values, int values_count);
const PPMPixel* MapGetPixel(const Map* map, int x, int y);
PPMPixel MapGetNodeTypeColor(const Map* map, int node_type);

#endif // MAP_H
