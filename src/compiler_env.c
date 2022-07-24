#include "../src/compiler_env.h"

compiler_env *build_compiler_env() {
  compiler_env *env = malloc(sizeof(compiler_env));
  if (env == NULL) exit(1);

  env->ctx = gcc_jit_context_acquire();
  env->void_type = gcc_jit_context_get_type(env->ctx, GCC_JIT_TYPE_VOID);
  env->void_ptr_type = gcc_jit_context_get_type(env->ctx, GCC_JIT_TYPE_VOID_PTR);
  env->size_t_type = gcc_jit_context_get_type(env->ctx, GCC_JIT_TYPE_SIZE_T);
  env->bool_type = gcc_jit_context_get_type(env->ctx, GCC_JIT_TYPE_BOOL);
  env->int_type = gcc_jit_context_get_type(env->ctx, GCC_JIT_TYPE_INT);
  env->double_type = gcc_jit_context_get_type(env->ctx, GCC_JIT_TYPE_DOUBLE);
  env->double_type_ptr = gcc_jit_type_get_pointer(env->double_type);
  env->string_type = gcc_jit_context_get_type(env->ctx, GCC_JIT_TYPE_CONST_CHAR_PTR);

  env->money_amount_field = gcc_jit_context_new_field(env->ctx, NULL, env->double_type, "amount");
  env->money_currency_field = gcc_jit_context_new_field(env->ctx, NULL, env->string_type, "currency");
  gcc_jit_field *money_fields[2] = {env->money_amount_field, env->money_currency_field};
  gcc_jit_struct *money = gcc_jit_context_new_struct_type(env->ctx, NULL, "money", 2, money_fields);
  env->money_type = gcc_jit_struct_as_type(money);
  env->money_ptr_type = gcc_jit_type_get_pointer(env->money_type);

  // In order to use malloc/free we need to figure out how to get sizeof a type.
  // I don't see a builtin way to do this, but found this online:
  // https://stackoverflow.com/questions/46583467/gccjit-get-the-size-of-a-type
  // Alternatively I suppose we could run sizeof for every type we intend to use and just have it precalculated.
  add_malloc_function(env);
  add_free_function(env);
  add_mpd_new_function(env);
  add_mpd_set_string_function(env);

  return env;
}

void add_malloc_function(compiler_env *env) {
  // void * malloc(size_t __size)
  gcc_jit_param *size_param = gcc_jit_context_new_param(env->ctx, NULL, env->size_t_type, "__size");
  env->malloc = gcc_jit_context_new_function(env->ctx, NULL, GCC_JIT_FUNCTION_IMPORTED, env->void_ptr_type, "malloc", 1,
                                             &size_param, 0);
}

void add_free_function(compiler_env *env) {
  // void free(void *)
  gcc_jit_param *ptr_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "ptr");
  env->free = gcc_jit_context_new_function(env->ctx, NULL, GCC_JIT_FUNCTION_IMPORTED, env->void_ptr_type, "free", 1,
                                           &ptr_param, 0);
}

void add_mpd_new_function(compiler_env *env) {
  // mpd_t * mpd_new(mpd_context_t *ctx)
  gcc_jit_param *ctx_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "ctx");
  env->mpd_new = gcc_jit_context_new_function(env->ctx, NULL, GCC_JIT_FUNCTION_IMPORTED, env->void_ptr_type, "mpd_new",
                                              1, &ctx_param, 0);
}

void add_mpd_set_string_function(compiler_env *env) {
  // void mpd_set_string(mpd_t *result, const char *s, mpd_context_t *ctx)
  gcc_jit_param *result_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "result");
  gcc_jit_param *s_param = gcc_jit_context_new_param(env->ctx, NULL, env->string_type, "s");
  gcc_jit_param *ctx_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "ctx");
  gcc_jit_param *params[3] = {result_param, s_param, ctx_param};
  env->mpd_set_string = gcc_jit_context_new_function(env->ctx, NULL, GCC_JIT_FUNCTION_IMPORTED, env->void_type,
                                                     "mpd_set_string", 3, params, 0);
}

void release_compiler_env(compiler_env *env) {
  gcc_jit_context_release(env->ctx);

  free(env);
}