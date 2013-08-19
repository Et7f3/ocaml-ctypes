/*
 * Copyright (c) 2013 Jeremy Yallop.
 *
 * This file is distributed under the terms of the MIT License.
 * See the file LICENSE for details.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <errno.h>
#include <stdint.h>
#include <complex.h>

#include <caml/mlvalues.h>

#define ALIGNMENT(T) (offsetof(struct { char c; T t; }, t))
#define FULL_ENTRY(T, SIZE, ALIGNMENT) { #T, SIZE, ALIGNMENT }
#define ENTRY(ABBREV, T) FULL_ENTRY(ABBREV, sizeof(T), ALIGNMENT(T))

static struct details {
  const char *name;
  int size, alignment;
} details[] = {
  ENTRY(Char, char),
  ENTRY(Schar, signed char),
  ENTRY(Uchar, unsigned char),
  ENTRY(Short, short),
  ENTRY(Int, int),
  ENTRY(Long, long),
  ENTRY(Llong, long long),
  ENTRY(Ushort, unsigned short),
  ENTRY(Uint, unsigned int),
  ENTRY(Ulong, unsigned long),
  ENTRY(Ullong, unsigned long long),
  ENTRY(Size_t, size_t),
  ENTRY(Int8_t, int8_t),
  ENTRY(Int16_t, int16_t),
  ENTRY(Int32_t, int32_t),
  ENTRY(Int64_t, int64_t),
  ENTRY(Uint8_t, uint8_t),
  ENTRY(Uint16_t, uint16_t),
  ENTRY(Uint32_t, uint32_t),
  ENTRY(Uint64_t, uint64_t),
  ENTRY(Camlint, intnat),
  ENTRY(Nativeint, intnat),
  ENTRY(Float, float),
  ENTRY(Double, double),
  ENTRY(Complex32, float complex),
  ENTRY(Complex64, double complex),
};

void generate_function(FILE *fp, const char *name,
                       void (*cse)(FILE *fp, struct details*))
{
  int i;
  fprintf(fp, "let %s : type a. a prim -> int = function\n", name);
  for (i = 0; i < sizeof details / sizeof *details; i++) {
    fprintf(fp, " | %s -> ", details[i].name);
    cse(fp, &details[i]);
    fprintf(fp, "\n");
  }
}

void print_size(FILE *fp, struct details *d)
{
  fprintf(fp, "%d", d->size);
}

void print_alignment(FILE *fp, struct details *d)
{
  fprintf(fp, "%d", d->alignment);
}

int main(int argc, char **argv)
{
  FILE *fp;

  if (argc < 2) {
    fprintf(stderr, "Usage: %s filename\n", argv[0]);
    return EXIT_FAILURE;
  }
  else if ((fp = fopen(argv[1], "wb")) == NULL) {
    perror("fopen");
    return EXIT_FAILURE;
  }
  else {
    fprintf(fp, "open Primitives\n");
    generate_function(fp, "sizeof", print_size);
    generate_function(fp, "alignment", print_alignment);
    fprintf(fp, "let pointer_size = %d\n", (int)sizeof(void *));
    fprintf(fp, "let pointer_alignment = %d\n", (int)ALIGNMENT(void *));
    fclose(fp);
    return EXIT_SUCCESS;
  }
}
