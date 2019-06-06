#include <stdio.h>
#include "map_reader.h"
#include "utils.h"
#define STEPNAME(step) case step: return #step;

enum STEPS {
	VERSION = 0,
	PPMMAP = 1,
	VALUES = 2,
	NODES_COUNT = 3,
	NODES = 4,
};

PPMImage* read_ppm_map(const char* filename)
{
	debug("READING MAP %s", filename);
	int ppm_version;
	PPMImage *img;
	FILE *fp;
	int c, rgb_comp_color;

	NEW(img, PPMImage);

	//open PPM file for reading
	fp = fopen(filename, "r");
	if (!fp) {
		error("Cannot read PPM %s", filename);
	}

	//read/check the image format
	if (fscanf(fp, "P%d", &ppm_version) < 1) {
		error("Cannot read PPM %s: "
			  "Invalid image format", filename);
	}

	//check for comments
	c = getc(fp);
	while (c == '#') {
		while (getc(fp) != '\n') ;
		c = getc(fp);
	}

	ungetc(c, fp);
	//read image size information
	if (fscanf(fp, "%d %d", &img->x, &img->y) != 2) {
		error("Cannot read PPM %s: Invalid image size", filename);
	}

	//read rgb component
	if (fscanf(fp, "%d", &rgb_comp_color) != 1) {
		error("Cannot read PPM %s: Invalid rgb component", filename);
	}

	while (fgetc(fp) != '\n') ;
	//memory allocation for pixel data
	NEW_ARRAY(img->data, PPMPixel, img->x * img->y);

	//read pixel data from file
	unsigned char* data = (unsigned char*) img->data;
	for (int i=0; i<img->x*img->y*3; i++)
	{
		if (fscanf(fp, "%u", data+i) != 1) {
			error("Cannot read PPM %s: Invalid rgb component", filename);
		}
	}
	fclose(fp);
	return img;
}

const char* step_name(int step)
{
	switch(step)
	{
	STEPNAME(VERSION);
	STEPNAME(PPMMAP);
	STEPNAME(VALUES);
	STEPNAME(NODES_COUNT);
	STEPNAME(NODES);
	}
	return "UNKNOW";
}

void sscanf_array(const char* buff, int* numbers, int* numbers_count)
{
	char str_value[LOWMAXSIZE];
	int buff_index=0;
	(*numbers_count) = 0;

	while (sscanf(buff+buff_index, "%s", str_value) == 1)
	{
		if(sscanf(str_value, "%d", numbers+(*numbers_count)) != 1)
			break;
		buff_index += strlen(str_value)+1;
		(*numbers_count)++;
		if (buff[buff_index] == '\0' || buff[buff_index] == '\n')
			break;
	}
}



Map* read_idt(const char* filename)
{
	char buff[HIGHMAXSIZE];
	char str_value[LOWMAXSIZE];
	int numbers_value[LOWMAXSIZE];
	int numbers_count;
	int step = VERSION;
	PPMImage* ppm_map;
	Map* map = NULL;
	int int_value;
	unsigned char reading = 1;
	FILE* fp = fopen(filename, "r");

	if (fp == NULL){
		error("Cannot open file %s",filename);
	}

	reading = (fgets(buff, HIGHMAXSIZE, fp) != NULL);
	while (reading)
	{
		if (buff[0] == '#')
		{
			debug("COMMENT: %s", buff);
		}
		else
		{
			switch(step)
			{
			case VERSION:
				debug("READING %s (%s)", filename, buff);
				step = PPMMAP;
				break;
			case PPMMAP:
				if (sscanf(buff, "carte %s", str_value) != 1)
				{
					error("IDT is invalid -> step: %s; line: %s", step_name(step), buff);
				}
				ppm_map = read_ppm_map(str_value);
				NEW_N_INIT(map, Map, ppm_map, 0);
				step = VALUES;
				break;
			case VALUES:
				if (sscanf(buff, "%d", &int_value) == 1)
				{
					step = NODES_COUNT;
					continue;
				}
				if (sscanf(buff, "%s", str_value) != 1)
				{
					error("IDT is invalid -> step: %s; line: %s", step_name(step), buff);
				}
				sscanf_array(buff+strlen(str_value)+1, numbers_value, &numbers_count);
				MapSetValue(map, str_value, numbers_value, numbers_count);
				break;
			case NODES_COUNT:
				MapInitNodes(map, int_value);
				step = NODES;
				break;
			case NODES:
				numbers_count=0;
				sscanf_array(buff, numbers_value, &numbers_count);
				map->nodes[numbers_value[0]]->type = numbers_value[1];
				map->nodes[numbers_value[0]]->x = numbers_value[2];
				map->nodes[numbers_value[0]]->y = numbers_value[3];

				for(int_value=4; int_value<numbers_count;int_value++){
					printf("Node id=%d value=%d\n", numbers_value[0], numbers_value[int_value]);
					MapAddConnection(map, numbers_value[0], numbers_value[int_value]);
				}
				break;
			default:
				reading = 0;
				continue;
			}
		}
		reading = (fgets(buff, HIGHMAXSIZE, fp) != NULL);
	}
	fclose(fp);
	if (!map)
	{
		error("IDT is invalid, map has not been created -> last step: %s", step_name(step));
	}
	return map;
}

void bresenham_x_y(int start_x, int start_y, int end_x, int end_y,
									 void (*visitor)(int, int, int, void**), int argc, void** argv)
{
	int dx = abs(end_x - start_x);
	int dy = abs(end_y - start_y);

	int inc_ex=dy*2, inc_ey=-dx*2;
	int e=-dx;
	int inc_y = end_y > start_y ? 1 : -1;
	int inc_x = end_x > start_x ? 1 : -1;
	int x=start_x, y=start_y;
	if (inc_x>0)
	{
		while (x<end_x)
		{
			(*visitor)(x, y, argc, argv);
			++x;
			e+=inc_ex;
			if (e>0)
			{
				y+=inc_y;
				e+=inc_ey;
			}
		}
	}
	else
	{
		while (x>end_x)
		{
			(*visitor)(x, y, argc, argv);
			--x;
			e+=inc_ex;
			if (e>0)
			{
				y+=inc_y;
				e+=inc_ey;
			}
		}
	}
	(*visitor)(x, y, argc, argv);
}

void bresenham_y_x(int start_x, int start_y, int end_x, int end_y,
									 void (*visitor)(int, int, int, void**), int argc, void** argv)
{
	int dx = abs(end_x - start_x);
	int dy = abs(end_y - start_y);

	int inc_ex=-dy*2, inc_ey=dx*2;
	int e=-dy;
	int inc_y = end_y > start_y ? 1 : -1;
	int inc_x = end_x > start_x ? 1 : -1;
	int x=start_x, y=start_y;
	if (inc_y>0)
	{
		while (y<end_y)
		{
			(*visitor)(x, y, argc, argv);
			++y;
			e+=inc_ey;
			if (e>0)
			{
				x+=inc_x;
				e+=inc_ex;
			}
		}
	}
	else
	{
		while (y>end_y)
		{
			(*visitor)(x, y, argc, argv);
			--y;
			e+=inc_ey;
			if (e>0)
			{
				x+=inc_x;
				e+=inc_ex;
			}
		}
	}
	(*visitor)(x, y, argc, argv);
}

void check_pixel(int x, int y, int argc, void** argv)
{
	const Map* map = (const Map*)argv[0];
	PPMPixel* colors = (PPMPixel*)argv[1];
	int* color_i = (int*)argv[2];
	const PPMPixel* current = MapGetPixel(map, x, y);
	debug("Color (%u, %u, %u)  at (%d, %d)", current->red, current->green, current->blue, x, y);

	if (!PPMPixelEqual2(current, colors+(*color_i), 100)) //ici on vérifie que lécart entre la couleur précédente et la nouvelle (un écart trop petit fait boguer la map03)
	{
			++(*color_i);
			if ((*color_i)==3 || !PPMPixelEqual2(current, colors+(*color_i), 100))
			{
				error("Map is invalid, way color (%u, %u, %u)  at (%d, %d) was not expected !\n"
							"Expected: (%u, %u, %u), (%u, %u, %u) or (%u, %u, %u)",
							current->red, current->green, current->blue, x, y,
							colors[0].red, colors[0].green, colors[0].blue,
							colors[1].red, colors[1].green, colors[1].blue,
							colors[2].red, colors[2].green, colors[2].blue
						);
			}
	}
}

void check_nodes_way(const Map *map, const Node* origin, const Node* destination)
{
	PPMPixel dest_color = MapGetNodeTypeColor(map, destination->type);
	PPMPixel orig_color = MapGetNodeTypeColor(map, origin->type);

	int dx = abs(destination->x - origin->x);
	int dy = abs(destination->y - origin->y);
	PPMPixel colors[] = {
		orig_color,
		map->way_color,
		dest_color
	};
	int color_i=0;
	void* argv[] =
	{
		(void*)map,
		colors,
		&color_i
	};
	int argc = 3;
	if (dx > dy)
	{
		bresenham_x_y(origin->x, origin->y, destination->x, destination->y,
									check_pixel, argc, argv);
	}
	else
	{
		bresenham_y_x(origin->x, origin->y, destination->x, destination->y,
									check_pixel, argc, argv);
	}
	char* __temp__ = (char*) malloc(45 * sizeof(char));
	__temp__[0] = 120; __temp__[20] = 0;
	debug("Check memory: %s", __temp__);
	free(__temp__);
	fflush(stdout);
}

void check_map(const Map *map)
{
	if (!map)
	{
		error("MAP CHECK: map is NULL");
	}
	bool ok;
	for (int i=0; i<map->nodes_count; ++i)
	{
		Node* node = map->nodes[i];
		printf("Check node x=%d y=%d type=%d\n", node->x, node->y, node->type);
		const PPMPixel* pixel = MapGetPixel(map, node->x, node->y);
		printf("Color = %d %d %d\n", pixel->red, pixel->green, pixel->blue);
		//printf("Map data = x:%d y:%d\n", map->map_data->x, map->map_data->y);
		printf("Data size = %d\n", sizeof(map->map_data->data));
		printf("Data index : %d\n", node->x*map->map_data->x+node->y);
		//printf("Test : %d\n", node->);
		//PPMPixel* pixelTest = &map->map_data->data[450*map->map_data->x+100];
		//printf("Data 0 : %d\n", pixelTest->green);
		/*if (node->x >= map->map_data->x or node->y >= map->map_data->y)
		{
			fprintf(stderr, "Cannot access to this pixel (%d, %d)", node->x, node->y);
		}else{
			PPMPixel* myPixel = &map->map_data->data[node->x*map->map_data->x+node->y];
			printf("Color MyPixel = %d %d %d\n", myPixel->red, myPixel->green, myPixel->blue);
		}*/
		PPMPixel node_type_color = MapGetNodeTypeColor(map, node->type);
		if (!PPMPixelEqual(pixel, &node_type_color))
		{

			error("Node color does not correspond to its type Node_%d<%d, %d>\n"
				  "Node type color: (%u, %u, %u), should be (%u, %u, %u)",
				  node->id, node->x, node->y, pixel->red, pixel->green, pixel->blue,
				  node_type_color.red, node_type_color.green, node_type_color.blue);
		}
		NodeList* sibling = node->connected;
		while (sibling)
		{
			check_nodes_way(map, node, sibling->node);
			sibling = sibling->next;
		}
	}
	debug("MAP CHECK: success !");
}
