#include "../src/structure.h"
#include "../src/runtime_env.h"
#include "../src/compiler.h"

typedef double (*calculated_field_fn) ();

int main(int argc, char **argv)
{
  compiler_env *c_env = build_compiler_env();
  runtime_env *r_env = build_runtime_env();

  json_object *structure = read_structure_json();

  comp_calculated_fields(structure, c_env);

  gcc_jit_result *result = gcc_jit_context_compile(c_env->ctx);

  calculated_field_fn cf_123 =
    (calculated_field_fn)gcc_jit_result_get_code (result, "calculated_field_123");

  calculated_field_fn cf_456 =
    (calculated_field_fn)gcc_jit_result_get_code (result, "calculated_field_456");

  printf("calculated_field_123() -> %f\n", cf_123());
  printf("calculated_field_456() -> %f\n", cf_456());

  release_compiler_env(c_env);
  release_runtime_env(r_env);

  return 0;
}