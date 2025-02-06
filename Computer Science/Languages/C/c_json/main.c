#include "c_json.h"

int main() {
  struct json json = parse_simple_json("./example.json");
  print_simple_json(json);
  free_simple_json(json);
  return 0;
}
