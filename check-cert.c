#include <stdio.h>
#include <stdlib.h>

#define INIT	10

int main (int argc, char** argv) {
  int i, j, tmp, max = 0;
  int *cert, size, allocated;
  FILE *input;

  if (argc <= 1) {
    printf ("c no certificate as input is provided\n");
    exit (0); }

  allocated = INIT;
  cert = (int*) malloc (sizeof (int) * allocated);

  input = fopen (argv[1], "r");
  while (1) {
    char c;
    tmp = fscanf (input, "%c", &c);
    if (tmp == EOF) break;
    cert[size++] = (int) (c - 'a');
    if (cert[size - 1] > max) max = cert[size - 1];
    if (size == allocated) {
      allocated *= 2;
      cert = (int *) realloc (cert, sizeof(int) * allocated); } }
  fclose (input);

  printf("c parsed certificate using %i sets and %i numbers\n", max + 1, size);

}
