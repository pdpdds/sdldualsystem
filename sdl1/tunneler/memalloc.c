#include <stdio.h>
#include <stdlib.h>

#include <gamelib.h>
#include <tweak.h>

#ifdef _MEM_STATS

	/* Oh, god, this is *SO* not thread-safe: */
	static unsigned __TOTAL_MEMORY_ALLOCATED = 0;
	static unsigned __MAX_MEMORY_ALLOCATED = 0;

	void *__get_mem(unsigned ammount, char *file, unsigned line) {
		unsigned *out = (unsigned *) malloc(ammount + sizeof(unsigned));
		if(!out) {
			gamelib_error("%s(%u): ", file, line);
			gamelib_error("Failed to allocate %u bytes.\n", ammount);
			abort();
		}
		
		out[0] = ammount;
		__TOTAL_MEMORY_ALLOCATED += ammount;
		if(__TOTAL_MEMORY_ALLOCATED > __MAX_MEMORY_ALLOCATED)
			__MAX_MEMORY_ALLOCATED = __TOTAL_MEMORY_ALLOCATED;
		
		return (void *)(&out[1]);
	}

	void free_mem(void *ptr) {
		unsigned *t = ((unsigned *)ptr - sizeof(unsigned));
		
		if(t && ptr) {
			__TOTAL_MEMORY_ALLOCATED -= t[0];
			free(t);
		}
	}

	void print_mem_stats() {
		gamelib_print("\n--- MEMORY STATISTICS: -----\n");
		gamelib_print("Memory still allocated: %u bytes\n", __TOTAL_MEMORY_ALLOCATED);
		gamelib_print("Max memory allocated:   %u bytes\n", __MAX_MEMORY_ALLOCATED);
	}


#else /* ! _MEM_STATS */

	void *__get_mem(unsigned ammount, char *file, unsigned line) {
		void *out = (void *) malloc( ammount );
		if(!out) {
			gamelib_error("%s(%d): ", file, line);
			gamelib_error("Failed to allocate %u bytes.\n", ammount);
			abort();
		}
		return out;
	}
	
	void free_mem(void *ptr) {
		if(ptr) free(ptr);
	}
	
	void print_mem_stats() {}

#endif /* _MEM_STATS */

