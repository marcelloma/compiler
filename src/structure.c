#include "../src/structure.h"

json_object *read_structure_json()
{
  FILE *file = fopen(STRUCTURE_FILE, "r");

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  fseek(file, 0, SEEK_SET);

  char *file_contents = malloc(file_size + 1);
  fread(file_contents, file_size, 1, file);

  json_object *structure = json_tokener_parse(file_contents);
  free(file_contents);
  fclose(file);

  return structure;
}