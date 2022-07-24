#include <mpdecimal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  mpd_context_t *mpd_ctx;
} runtime_env;

runtime_env *build_runtime_env();
mpd_context_t *get_mpd_ctx(runtime_env *env);
mpd_t *dec_from_str(const char *str, runtime_env *env);
mpd_t *dec_minus(mpd_t *dec, runtime_env *env);
void release_runtime_env(runtime_env *env);