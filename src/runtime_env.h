#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpdecimal.h>

typedef struct
{
  mpd_context_t *mpd_ctx;
} runtime_env;

runtime_env *build_runtime_env();

void release_runtime_env(runtime_env *env);