#include <stdio.h>
#include <stdlib.h>

#define INIT	10

int main (int argc, char** argv) {
  int i, j, k, tmp, max = 0;
  int *cert, size, allocated;
  FILE *input;

  if (argc <= 1) {
    printf ("c no certificate as input is provided\n");
    exit (0); }

  allocated = INIT;
  cert = (int*) malloc (sizeof (int) * allocated);

  size = 0;
  input = fopen (argv[1], "r");
  while (1) {
    char c;
    tmp = fscanf (input, "%c", &c);
    if (tmp == EOF) break;
    if (((int) (c - 'a')) >= 0)
      cert[size++] = (int) (c - 'a');
    if (cert[size - 1] > max) max = cert[size - 1];
    if (size == allocated) {
      allocated *= 2;
      cert = (int *) realloc (cert, sizeof(int) * allocated); } }
  fclose (input);

  printf ("c parsed certificate using %i sets and %i numbers\n", max + 1, size);

  max = 1;
  for (i = 0; i < size; i++)
    for (j = 1; (i + j*max) < size; j++)
      for (k = 1; (i + j*k) < size; k++) {
        if (cert[i+j*k] != cert[i]) break;
        if (k + 1 > max) max = k + 1; }

  printf ("c largest arithmetic progression is %i\n", max); }
