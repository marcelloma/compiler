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
  add_printf_function(env);
  add_malloc_function(env);
  add_free_function(env);
  add_mpd_new_function(env);
  add_mpd_set_string_function(env);
  add_get_mpd_ctx_function(env);
  add_dec_minus(env);
  add_dec_add(env);
  add_dec_sub(env);
  add_dec_mul(env);
  add_dec_div(env);
  add_dec_from_str(env);

  return env;
}

void add_printf_function(compiler_env *env) {
  // int printf(const char *__restrict__, ...)
  gcc_jit_param *format_param = gcc_jit_context_new_param(env->ctx, NULL, env->string_type, "__restrict__");
  env->printf = gcc_jit_context_new_function(env->ctx, NULL, GCC_JIT_FUNCTION_IMPORTED, env->int_type, "printf", 1,
                                             &format_param, 1);
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

void add_get_mpd_ctx_function(compiler_env *env) {
  // mpd_context_t *get_mpd_ctx(runtime_env *env)
  gcc_jit_param *env_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "env");
  env->get_mpd_ctx = gcc_jit_context_new_function(env->ctx, NULL, GCC_JIT_FUNCTION_IMPORTED, env->void_ptr_type,
                                                  "get_mpd_ctx", 1, &env_param, 0);
}

void add_dec_from_str(compiler_env *env) {
  // mpd_t *dec_from_str(const char *str, runtime_env *env);
  gcc_jit_param *str_param = gcc_jit_context_new_param(env->ctx, NULL, env->string_type, "str");
  gcc_jit_param *env_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "env");
  gcc_jit_param *params[2] = {str_param, env_param};
  env->dec_from_str = gcc_jit_context_new_function(env->ctx, NULL, GCC_JIT_FUNCTION_IMPORTED, env->void_ptr_type,
                                                   "dec_from_str", 2, params, 0);
}

void add_dec_minus(compiler_env *env) {
  // mpd_t *dec_minus(mpd_t *dec, runtime_env *env);
  gcc_jit_param *dec_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "dec");
  gcc_jit_param *ctx_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "ctx");
  gcc_jit_param *params[2] = {dec_param, ctx_param};
  env->dec_minus = gcc_jit_context_new_function(env->ctx, NULL, GCC_JIT_FUNCTION_IMPORTED, env->void_ptr_type,
                                                "dec_minus", 2, params, 0);
}

void add_dec_add(compiler_env *env) {
  // mpd_t *dec_add(mpd_t *dec_a, mpd_t *dec_b, runtime_env *env);
  gcc_jit_param *dec_a_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "dec_a");
  gcc_jit_param *dec_b_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "dec_b");
  gcc_jit_param *ctx_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "ctx");
  gcc_jit_param *params[3] = {dec_a_param, dec_b_param, ctx_param};
  env->dec_add = gcc_jit_context_new_function(env->ctx, NULL, GCC_JIT_FUNCTION_IMPORTED, env->void_ptr_type,
                                                "dec_add", 3, params, 0);
}

void add_dec_sub(compiler_env *env) {
  // mpd_t *dec_sub(mpd_t *dec_a, mpd_t *dec_b, runtime_env *env);
  gcc_jit_param *dec_a_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "dec_a");
  gcc_jit_param *dec_b_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "dec_b");
  gcc_jit_param *ctx_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "ctx");
  gcc_jit_param *params[3] = {dec_a_param, dec_b_param, ctx_param};
  env->dec_sub = gcc_jit_context_new_function(env->ctx, NULL, GCC_JIT_FUNCTION_IMPORTED, env->void_ptr_type,
                                                "dec_sub", 3, params, 0);
}

void add_dec_mul(compiler_env *env) {
  // mpd_t *dec_mul(mpd_t *dec_a, mpd_t *dec_b, runtime_env *env);
  gcc_jit_param *dec_a_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "dec_a");
  gcc_jit_param *dec_b_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "dec_b");
  gcc_jit_param *ctx_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "ctx");
  gcc_jit_param *params[3] = {dec_a_param, dec_b_param, ctx_param};
  env->dec_mul = gcc_jit_context_new_function(env->ctx, NULL, GCC_JIT_FUNCTION_IMPORTED, env->void_ptr_type,
                                                "dec_mul", 3, params, 0);
}

void add_dec_div(compiler_env *env) {
  // mpd_t *dec_div(mpd_t *dec_a, mpd_t *dec_b, runtime_env *env);
  gcc_jit_param *dec_a_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "dec_a");
  gcc_jit_param *dec_b_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "dec_b");
  gcc_jit_param *ctx_param = gcc_jit_context_new_param(env->ctx, NULL, env->void_ptr_type, "ctx");
  gcc_jit_param *params[3] = {dec_a_param, dec_b_param, ctx_param};

  env->dec_div = gcc_jit_context_new_function(env->ctx, NULL, GCC_JIT_FUNCTION_IMPORTED, env->void_ptr_type,
                                                "dec_div", 3, params, 0);
}

void release_compiler_env(compiler_env *env) {
  gcc_jit_context_release(env->ctx);

  free(env);
}