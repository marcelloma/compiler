#include <mpdecimal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  mpd_context_t *mpd_ctx;
} runtime_env;

runtime_env *build_runtime_env();

void release_runtime_env(runtime_env *env);