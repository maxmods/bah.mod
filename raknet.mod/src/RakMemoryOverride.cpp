#include "RakMemoryOverride.h"
#include "RakAssert.h"
#include <stdlib.h>

#if defined (_PS3)
#include "Console2Includes.h"
#endif

using namespace RakNet;

#if defined(_USE_RAK_MEMORY_OVERRIDE)
	#if defined(malloc)
	#pragma push_macro("malloc")
	#undef malloc
	#define RMO_MALLOC_UNDEF
	#endif

	#if defined(realloc)
	#pragma push_macro("realloc")
	#undef realloc
	#define RMO_REALLOC_UNDEF
	#endif

	#if defined(free)
	#pragma push_macro("free")
	#undef free
	#define RMO_FREE_UNDEF
	#endif
#endif

void DefaultOutOfMemoryHandler(const char *file, const long line)
{
	(void) file;
	(void) line;
	RakAssert(0);
}

void* (*rakMalloc) (size_t size) = RakNet::_RakMalloc;
void* (*rakRealloc) (void *p, size_t size) = RakNet::_RakRealloc;
void (*rakFree) (void *p) = RakNet::_RakFree;
void (*notifyOutOfMemory) (const char *file, const long line)=DefaultOutOfMemoryHandler;

void* RakNet::_RakMalloc (size_t size)
{
	return malloc(size);
}

void* RakNet::_RakRealloc (void *p, size_t size)
{
	return realloc(p,size);
}

void RakNet::_RakFree (void *p)
{
	free(p);
}

#if defined(_USE_RAK_MEMORY_OVERRIDE)
	#if defined(RMO_MALLOC_UNDEF)
	#pragma pop_macro("malloc")
	#undef RMO_MALLOC_UNDEF
	#endif

	#if defined(RMO_REALLOC_UNDEF)
	#pragma pop_macro("realloc")
	#undef RMO_REALLOC_UNDEF
	#endif

	#if defined(RMO_FREE_UNDEF)
	#pragma pop_macro("free")
	#undef RMO_FREE_UNDEF
	#endif
#endif
