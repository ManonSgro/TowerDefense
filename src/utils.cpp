#include <stdarg.h>

#include "utils.h"

void** ALLOCATIONS = (void**) malloc(sizeof(void*) * HIGHMAXSIZE);
int ALLOCATIONS_COUNT = 0;

String *StringNew()
{
	String* s;
	NEW(s, String);
	NEW_ARRAY(s->_data, char, LOWMAXSIZE);
	return s;
}

const char *StringData(const String* s)
{
	return s->_data;
}

void StringSet(const String* s, const char* characters, size_t char_count)
{
	memcpy(s->_data, characters, char_count);
}

void StringFree(const String *s)
{
	DELETE(s->_data);
}

void freeAllAllocations()
{
	for (int i=0; i<ALLOCATIONS_COUNT; i++)
		if (ALLOCATIONS[ALLOCATIONS_COUNT])
			free(ALLOCATIONS[ALLOCATIONS_COUNT]);
	ALLOCATIONS_COUNT = 0;
}

void registerAllocation(void *allocated)
{
	if (!ALLOCATIONS_COUNT)
		memset(ALLOCATIONS, 0, HIGHMAXSIZE);
	ALLOCATIONS[ALLOCATIONS_COUNT] = allocated;
	ALLOCATIONS_COUNT++;
}

void unregisterAllocation(void* allocated)
{
	if (!ALLOCATIONS_COUNT)
		return;
	int i;
	for (i=0; i<ALLOCATIONS_COUNT; ++i)
	{
		if (ALLOCATIONS[i] == allocated)
			break;
	}
	if (i != ALLOCATIONS_COUNT)
	{
		ALLOCATIONS_COUNT--;
		ALLOCATIONS[i] = ALLOCATIONS[ALLOCATIONS_COUNT];
	}
}

void error(const char *message, ...){
	va_list args;
	va_start(args, message);
	fprintf(stderr, "ERROR:");
	vfprintf(stderr, message, args);
	fprintf(stderr, "\n");
	va_end(args);
	freeAllAllocations();
	exit(EXIT_FAILURE);
}

void warning(const char *message, ...)
{
	va_list args;
	va_start(args, message);
	fprintf(stdout, "WARNING:");
	vfprintf(stdout, message, args);
	fprintf(stdout, "\n");
	va_end(args);
}

void debug(const char *message, ...)
{
#ifdef DEBUG
	va_list args;
	va_start(args, message);
	fprintf(stdout, "DEBUG:");
	vfprintf(stdout, message, args);
	fprintf(stdout, "\n\n");
	va_end(args);
#endif
}