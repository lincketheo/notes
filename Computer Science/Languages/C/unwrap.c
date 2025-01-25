
#define unwrap(expr, fail_expr) \
  ({                            \
    if ((expr) == NULL)         \
      (fail_expr);              \
    expr;                       \
  })

#define fopen_err(file, mode, expr) ({                      \
  unwrap(fopen(file, mode), ({                              \
    fprintf(stderr, "Failed to open filename: %s\n", file); \
    expr;                                                   \
  }));                                                      \
})

