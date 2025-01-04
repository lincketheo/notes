#include <stdio.h>

struct document_s;

typedef enum {
  DRAFT,
  MODERATION,
  PUBLISHED,
} state_t;

typedef struct {
  union {
    int draft;
    int moderation;
    int published;
  };
  state_t type;
  struct document_s* d;
} state;

typedef struct {
  state state;
} document;

document create_document(state initial)
{
  return (document) {
    .state = initial,
  };
}

int main()
{
}
