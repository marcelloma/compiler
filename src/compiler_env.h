#include <libgccjit.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  gcc_jit_context *ctx;
  gcc_jit_type *size_t_type;
  gcc_jit_type *void_type;
  gcc_jit_type *void_ptr_type;
  gcc_jit_type *bool_type;
  gcc_jit_type *int_type;
  gcc_jit_type *double_type;
  gcc_jit_type *string_type;
  gcc_jit_type *money_type;
  gcc_jit_type *money_ptr_type;
  gcc_jit_field *money_amount_field;
  gcc_jit_field *money_currency_field;
  gcc_jit_function *printf;
  gcc_jit_function *malloc;
  gcc_jit_function *free;
  gcc_jit_function *mpd_new;
  gcc_jit_function *mpd_set_string;
  gcc_jit_function *dec_from_str;
  gcc_jit_function *dec_minus;
  gcc_jit_function *dec_add;
  gcc_jit_function *dec_sub;
  gcc_jit_function *dec_mul;
  gcc_jit_function *dec_div;
  gcc_jit_function *get_mpd_ctx;
  gcc_jit_param *r_env;
  gcc_jit_rvalue *mpd_ctx;
  gcc_jit_block *block;
  gcc_jit_function *func;
} compiler_env;

// typedef struct mpd_t {
//   uint8_t flags;
//   mpd_ssize_t exp;
//   mpd_ssize_t digits;
//   mpd_ssize_t len;
//   mpd_ssize_t alloc;
//   mpd_uint_t *data;
// } mpd_t;

// typedef struct mpd_context_t {
//     mpd_ssize_t prec;   /* precision */
//     mpd_ssize_t emax;   /* max positive exp */
//     mpd_ssize_t emin;   /* min negative exp */
//     uint32_t traps;     /* status events that should be trapped */
//     uint32_t status;    /* status flags */
//     uint32_t newtrap;   /* set by mpd_addstatus_raise() */
//     int      round;     /* rounding mode */
//     int      clamp;     /* clamp mode */
//     int      allcr;     /* all functions correctly rounded */
// } mpd_context_t;

compiler_env *build_compiler_env();
void add_printf_function(compiler_env *env);
void add_malloc_function(compiler_env *env);
void add_free_function(compiler_env *env);
void add_mpd_new_function(compiler_env *env);
void add_mpd_set_string_function(compiler_env *env);
void add_get_mpd_ctx_function(compiler_env *env);
void add_dec_from_str(compiler_env *env);
void add_dec_minus(compiler_env *env);
void add_dec_add(compiler_env *env);
void add_dec_sub(compiler_env *env);
void add_dec_mul(compiler_env *env);
void add_dec_div(compiler_env *env);
void release_compiler_env(compiler_env *env);