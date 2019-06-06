#ifndef MAP_READER_H
#define MAP_READER_H

#include "map.h"

PPMImage* read_ppm_map(const char* filename);
Map* read_idt(const char* filename);
void check_map(const Map* map);


#endif // MAP_READER_H
