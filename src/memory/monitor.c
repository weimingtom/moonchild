#ifdef MOONCHILD_MONITOR

#include "monitor.h"
#include "moonchild.h"
#include "uthash.h"

static int total = 0;

typedef struct {
  void * pointer;
  char context[256];
  int size;
  UT_hash_handle hh;
} moon_allocation;

static moon_allocation * allocations = NULL;

int moon_monitor_get_total() {
  return total;
}

void * moon_monitor_malloc(const char * context, size_t size) {
  void * pointer = malloc(size);
  total += size;

  moon_allocation * allocation = (moon_allocation *) malloc(sizeof(moon_allocation));

  allocation->pointer = pointer;
  allocation->size = (int) size;
  sprintf(allocation->context, "%s", context);

  HASH_ADD_PTR(allocations, pointer, allocation);

  moon_debug("     *** malloc(%d) for '%s' [total = %d]\n", allocation->size, allocation->context, total);
  return pointer;
}

void moon_monitor_free(const char * context, void * pointer) {
  moon_allocation * allocation;
  HASH_FIND_PTR(allocations, &pointer, allocation);

  total -= allocation->size;

  moon_debug("     *** free(%d) for '%s' [total = %d]\n", allocation->size, allocation->context, total);
  free(pointer);
  HASH_DEL(allocations, allocation);
}

#endif
