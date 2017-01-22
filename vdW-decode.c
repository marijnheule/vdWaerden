#include <stdio.h>
#include <stdlib.h>

int main (int argc, char** argv) {
  if (argc <= 4) {
    printf ("c no certificate as input is provided\n");
    exit (0); }

  FILE *input = fopen (argv[1], "r");
  int k = atoi (argv[2]);
  int l = atoi (argv[3]);
  int m = atoi (argv[4]);

  int i;
  int *cert = (int*) malloc (sizeof(int) * m);
  for (i = 0; i < m; i++) cert[i] = -1;

  int tmp = fscanf (input, " v ");
  while (1) {
    int lit;
    int tmp = fscanf (input, " %i ", &lit);
    if (tmp == EOF) break;
    if (abs(lit) > k * m) break;
    if (lit > 0) {
      cert[ (lit-1) / k ] = (lit - 1) % k; } }
  fclose (input);

  int j;
  for (j = 1; j < l; j++)
    for (i = 0; i < m; i++) {
      if (cert[i] >= 0)
        printf ("%c", cert[i] + 'a'); }

  printf("\n"); }
