#ifndef UTILS_H
#define UTILS_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void freeAllAllocations();

void debug(const char* message, ...);
void error(const char* message, ...);
void warning(const char* message, ...);

#define HIGHMAXSIZE 1000
#define LOWMAXSIZE 200

#define NEW(variable, type) \
	variable = (type*)malloc(sizeof(type)); \
	if (!variable) {\
		error("Memory allocation failure for %s", #variable);\
	}\
	registerAllocation((void*)variable)

#define NEW_N_INIT(variable, type, ...) \
	variable = (type*)malloc(sizeof(type)); \
	if (!variable) {\
		error("Memory allocation failure for %s", #variable);\
	}\
	registerAllocation((void*)variable); \
	type##Init(variable, ##__VA_ARGS__);

#define NEW_ARRAY(variable, type, size) \
	variable = (type*)malloc(sizeof(type) * size); \
	if (!variable) {\
		error("Memory allocation failure for " #variable);\
	}\
	registerAllocation((void*)variable)

#define DELETE(variable) \
	free(variable); \
	unregisterAllocation((void*)variable);


#define MAP(_map_type, _key_type, _value_type) \
	struct _map_type##Node;\
	struct _map_type##Node {\
	_value_type value;\
	_map_type##Node* left;\
	_map_type##Node* right;\
	};

struct String
{
	char* _data;
};

String* StringNew();
const char* StringData(const String* s);
void StringSet(const String* s, const char*characters);
void StringFree(const String* s);

MAP(MapStringInt, String, int)

extern void** ALLOCATIONS;
extern int ALLOCATIONS_COUNT;

void registerAllocation(void* allocated);
void unregisterAllocation(void* allocated);
void freeAllAllocations();


#endif // UTILS_H
