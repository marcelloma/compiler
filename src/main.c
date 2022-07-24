#include "../src/compiler.h"
#include "../src/runtime_env.h"
#include "../src/structure.h"

typedef void *(*calculated_field_fn)(void *);

int main(int argc, char **argv) {
  json_object *structure = read_structure_json();

  compiler_env *c_env = build_compiler_env();
  runtime_env *r_env = build_runtime_env();

  comp_calculated_fields(structure, c_env);

  gcc_jit_result *result = gcc_jit_context_compile(c_env->ctx);

  calculated_field_fn cf_123 = (calculated_field_fn)gcc_jit_result_get_code(result, "calculated_field_123");
  mpd_t *cf_123_result = (mpd_t *)cf_123(r_env);
  printf("calculated_field_123() -> %s\n", mpd_format(cf_123_result, "n", r_env->mpd_ctx));

  calculated_field_fn cf_456 = (calculated_field_fn)gcc_jit_result_get_code(result, "calculated_field_456");
  mpd_t *cf_456_result = (mpd_t *)cf_456(r_env);
  printf("calculated_field_456() -> %s\n", mpd_format(cf_456_result, "n", r_env->mpd_ctx));

  release_compiler_env(c_env);
  release_runtime_env(r_env);

  return 0;
}