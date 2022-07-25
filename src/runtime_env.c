#include "../src/runtime_env.h"

runtime_env *build_runtime_env() {
  runtime_env *env = malloc(sizeof(runtime_env));

  env->mpd_ctx = malloc(sizeof(mpd_context_t));
  mpd_init(env->mpd_ctx, 10);

  return env;
}

mpd_context_t *get_mpd_ctx(runtime_env *env) { return env->mpd_ctx; }

mpd_t *dec_from_str(const char *str, runtime_env *env) {
  mpd_t *dec = mpd_new(env->mpd_ctx);
  mpd_set_string(dec, str, env->mpd_ctx);
  return dec;
}

mpd_t *dec_add(mpd_t *dec_a, mpd_t *dec_b, runtime_env *env) {
  mpd_t *dec = mpd_new(env->mpd_ctx);
  mpd_add(dec, dec_a, dec_b, env->mpd_ctx);
  return dec;
}

mpd_t *dec_sub(mpd_t *dec_a, mpd_t *dec_b, runtime_env *env) {
  mpd_t *dec = mpd_new(env->mpd_ctx);
  mpd_sub(dec, dec_a, dec_b, env->mpd_ctx);
  return dec;
}

mpd_t *dec_mul(mpd_t *dec_a, mpd_t *dec_b, runtime_env *env) {
  mpd_t *dec = mpd_new(env->mpd_ctx);
  mpd_mul(dec, dec_a, dec_b, env->mpd_ctx);
  return dec;
}

mpd_t *dec_div(mpd_t *dec_a, mpd_t *dec_b, runtime_env *env) {
  mpd_t *dec = mpd_new(env->mpd_ctx);
  mpd_div(dec, dec_a, dec_b, env->mpd_ctx);
  return dec;
}

mpd_t *dec_minus(mpd_t *dec, runtime_env *env) {
  mpd_minus(dec, dec, env->mpd_ctx);
  return dec;
}



void release_runtime_env(runtime_env *env) {
  mpd_free(env->mpd_ctx);

  free(env);
}