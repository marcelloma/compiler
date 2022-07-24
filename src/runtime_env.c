#include "../src/runtime_env.h"

runtime_env *build_runtime_env() {
  runtime_env *env = malloc(sizeof(runtime_env));

  env->mpd_ctx = malloc(sizeof(mpd_context_t));
  mpd_init(env->mpd_ctx, 10);

  return env;
}

mpd_context_t *get_mpd_ctx(runtime_env *env) { return env->mpd_ctx; }

mpd_t *dec_from_str(const char *str, runtime_env *env) {
  mpd_t *decimal = mpd_new(env->mpd_ctx);
  mpd_set_string(decimal, str, env->mpd_ctx);
  return decimal;
}

mpd_t *dec_minus(mpd_t *dec, runtime_env *env) {
  mpd_minus(dec, dec, env->mpd_ctx);
  return dec;
}

void release_runtime_env(runtime_env *env) {
  mpd_free(env->mpd_ctx);

  free(env);
}