#include "../src/runtime_env.h"

runtime_env *build_runtime_env() {
  runtime_env *env = malloc(sizeof(runtime_env));

  env->mpd_ctx = malloc(sizeof(mpd_context_t));
  mpd_init(env->mpd_ctx, 10);

  return env;
}

void release_runtime_env(runtime_env *env) {
  mpd_free(env->mpd_ctx);

  free(env);
}