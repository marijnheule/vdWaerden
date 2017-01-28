#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define INIT	1000

//#define SBP
//#define INTERNAL_ROOT
#define MIN_PRIME	2
#define POWER		2
//#define ROTATION
//#define INV
//#define RELAX

#define PREPARTITION

unsigned int get_root (unsigned int prime, unsigned int primezip) {
  unsigned int i, j, result, tmp;

  for (i = 2; i < prime; i++) {
    tmp = i;
    for (j = 2; j < prime; j++) {
//    printf("c root_tmp %i %i\n", i, tmp );
      if( tmp == 1 ) goto next_root;
      tmp = (tmp * i) % primezip; }

   if( tmp == 1 ) return i;
   next_root:; }

  return 1; }

unsigned int greatest_divider_rec (unsigned int n) {
  unsigned int i, max = sqrt (n);

  for (i = 2; i <= max; i++)
    if ((n % i) ==  0)
      return greatest_divider_rec (n / i);

  return n; }

unsigned int smallest_divider (unsigned int n) {
  unsigned int i, max = sqrt(n);

  for (i = 2; i <= max; i++)
    if ((n % i) ==  0)
      return i;

  return n; }

unsigned int rotation_factor (unsigned int prime, unsigned int primezip) {
  unsigned int zip, zip_div;

  zip = primezip / prime;

  if (zip == 1) return prime;

  zip_div = greatest_divider_rec (zip);

  if (zip_div * zip_div == zip) return zip_div * prime;

  return prime; }

int main (int argc , char *argv[]) {
  int h, i, j, k, l, tmp;
  int nrofsets, progression, numbers, primezip, prime, full_size, q, qmin;
  int nrofvars, nrofclauses, root, root_start, rotation_start, inv_start, rot_fac;
  int power;
  int *rep;
  int *clauses;
  int alloc, num = 0, stored = 0;

  alloc   = INIT;
  clauses = malloc (sizeof(int) * alloc);

  if (argc <= 3) {
    printf ("c wrong input: ./internal_sat sets length prime\n");
    exit (0); }

  nrofsets    = atoi (argv[1]);
  progression = atoi (argv[2]);
  primezip    = atoi (argv[3]);
  power       = atoi (argv[4]);

  rep = (int*) malloc (sizeof(int) * primezip);
  for (i = 0; i < primezip; i++) rep[i] = i;

  full_size   = primezip * (progression - 1);
  prime       = greatest_divider_rec (primezip);
  root        = get_root (prime, primezip);
  rot_fac     = rotation_factor (prime, primezip);
  qmin        = smallest_divider (primezip / prime);

//  printf ("c root = %i\n", root);
//  printf ("c qmin = %i\n", qmin);

  if ((prime < MIN_PRIME) || (root == 1)) {
    printf("p cnf 1 2\n1 0\n -1 0\n");
    exit (0); }

  nrofclauses = 0;
  for (i = 1; i <= primezip; i++)
    for (j = 1; (j*(progression-1)) < (full_size - i + 1); j++)
      nrofclauses++;

  nrofvars    = primezip * nrofsets;
  nrofclauses = primezip + nrofsets * nrofclauses;

  nrofclauses += 1;
#ifdef SBP
  nrofclauses += (nrofsets-2) * (nrofsets-1) / 2;
#endif
#ifdef INTERNAL_ROOT
  root_start   = nrofvars;
  nrofvars    += qmin * nrofsets * nrofsets;
  nrofclauses += qmin * nrofsets * nrofsets * (nrofsets-1);
  nrofclauses += nrofsets * nrofsets * (primezip - (primezip / prime));
#endif
#ifdef ROTATION
  rotation_start  = nrofvars;
  nrofvars       += nrofsets * nrofsets;
  nrofclauses    += nrofsets * nrofsets * (nrofsets-1);
  nrofclauses    += nrofsets * nrofsets * (primezip - (primezip / prime));
#endif
#ifdef INV
  inv_start    = nrofvars;
  nrofvars    += nrofsets * nrofsets;
  nrofclauses += nrofsets * nrofsets * (nrofsets-1);
  nrofclauses += nrofsets * nrofsets * (primezip - (primezip / prime));
#endif
#ifdef PREPARTITION
  nrofclauses += primezip * nrofsets * 2;
#endif

  printf ("p cnf %i %i\n", nrofvars, nrofclauses);
  printf ("1 0\n"); // the 0 element is in the first set

#ifdef PREPARTITION
  for (i = 0; i < primezip; i++) {
    tmp = i;
    for (j = 1; j <= power; j++)
      tmp = (((tmp+1) * root) - 1) % primezip;
//    printf("c rep rel %i %i\n", i, tmp);
    if      (rep[tmp] > rep[i]) rep[tmp] = rep[ i ];
    else if (rep[tmp] < rep[i]) rep[ i ] = rep[tmp];
    for (j = 1; j <= nrofsets; j++) {
      printf("%i -%i 0\n-%i %i 0\n", i*nrofsets + j, tmp*nrofsets + j, i*nrofsets + j, tmp*nrofsets + j); } }

  int flag;
  do {
    flag = 0;
    for (i = 0; i < primezip; i++)
      if (rep[rep[i]] < rep[i]) {
//        printf ("c making %i the rep of %i\n", rep[rep[i]], i);
        rep[i] = rep[rep[i]]; flag = 1; } }
  while (flag);

  int count = 0;
  for (i = 0; i < primezip; i++)
    if (rep[i] == i) count++;
//      printf ("c rep[%i] = %i\n", i, rep[i]);
  printf ("c %i representatives\n", count);
#endif


  for (i = 0; i < primezip; i++) {
    for (j = 1; j <= nrofsets; j++)
      printf("%i ", i * nrofsets + j);
    printf("0\n"); }


#ifdef SBP
  for (i = 0; i < nrofsets; i++)
    for (j = i+3; j <= nrofsets; j++)  // why + 3?
      printf("-%i 0\n", primezip*nrofsets + i*nrofsets + j);
#endif

#ifdef INTERNAL_ROOT
  for (i = 0; i < nrofsets; i++) {
    tmp = root_start + i * nrofsets;
    for (j = 1; j <= nrofsets; j++)
      for (k = j + 1; k <= nrofsets; k++)
#ifdef RELAX
        for (l = k + 1; l <= nrofsets; l++)
          printf ("-%i -%i -%i 0\n", tmp + j, tmp + k, tmp +l); }
#else
	printf ("-%i -%i 0\n", tmp + j, tmp + k); }
#endif

  for (i = 0; i < nrofsets; i++)
    for (j = i + 1; j < nrofsets; j++)
      for (l = 1; l <= nrofsets; l++)
#ifdef RELAX
        for (k = j + 1; k <= nrofsets; k++)
          printf("-%i -%i -%i 0\n", rotation_start + i*nrofsets + l, rotation_start + j*nrofsets + l, rotation_start + k*nrofsets + l);
#else
        printf("-%i -%i 0\n", rotation_start + i*nrofsets + l, rotation_start + j*nrofsets + l);
#endif

  for (i = 0; i < primezip; i++) {
    if ((i % prime) == (prime - 1)) continue;
//    if( (i % primezip) == (primezip - 1) ) continue;

    tmp = i;
    for (j = 1; j <= POWER; j++)
      tmp = (((tmp+1) * root) - 1) % primezip;

    for (j = 1; j <= nrofsets; j++)
      for( k = 1; k <= nrofsets; k++ )
        printf("-%i -%i %i 0\n", i * nrofsets + j, tmp * nrofsets + k, root_start + (j-1) * nrofsets + k); }
#endif

#ifdef INV
  for (i = 0; i < nrofsets; i++) {
    tmp = inv_start + i*nrofsets;
    for (j = 1; j <= nrofsets; j++)
      for (k = j+1; k <= nrofsets; k++)
	printf("-%i -%i 0\n", tmp + j, tmp + k ); }

  for (i = 0; i < nrofsets; i++)
    for (j = i + 1; j < nrofsets; j++)
      for (k = 1; k <= nrofsets; k++)
        printf ("-%i -%i 0\n", inv_start + i*nrofsets + k, inv_start + j*nrofsets + k);

  for (i = 0; i < primezip; i++) {
    if ((i % prime) == (prime - 1)) continue;
    tmp = (primezip - i - 2);
    for (j = 1; j <= nrofsets; j++)
      for (k = 1; k <= nrofsets; k++)
        printf ("-%i -%i %i 0\n", i * nrofsets + j, tmp*nrofsets + k, inv_start + (j-1) * nrofsets + k); }
#endif

#ifdef ROTATION
  for (i = 0; i < nrofsets; i++) {
    tmp = rotation_start + i * nrofsets;
    for (j = 1; j <= nrofsets; j++)
      for (k = j + 1; k <= nrofsets; k++)
#ifdef RELAX
        for (l = k + 1; l <= nrofsets; l++)
          printf ("-%i -%i -%i 0\n", tmp + j, tmp + k, tmp +l); }
#else
	printf ("-%i -%i 0\n", tmp + j, tmp + k); }
#endif

  for (i = 0; i < nrofsets; i++)
    for (j = i + 1; j < nrofsets; j++)
      for (l = 1; l <= nrofsets; l++)
#ifdef RELAX
        for (k = j + 1; k <= nrofsets; k++)
          printf("-%i -%i -%i 0\n", rotation_start + i*nrofsets + l, rotation_start + j*nrofsets + l, rotation_start + k*nrofsets + l);
#else
        printf("-%i -%i 0\n", rotation_start + i*nrofsets + l, rotation_start + j*nrofsets + l);
#endif

  for (i = 0; i < primezip; i++) {
    if ((i % prime) == (prime - 1)) continue;
    tmp = (i + rot_fac) % primezip;
//    tmp = (i + prime) % primezip;
    for (j = 1; j <= nrofsets; j++)
      for (k = 1; k <= nrofsets; k++)
        printf("-%i -%i %i 0\n", i * nrofsets + j, tmp*nrofsets + k, rotation_start + (j-1) * nrofsets + k); }
#endif

  int *clause = (int*) malloc (sizeof (int) * progression);
  // print the actual constraints
  for (i = 1; i <= primezip; i++)
    for (j = 1; (j*(progression-1)) < (full_size - i + 1); j++) {
      int size = 0;
      for (k = 0; k < progression; k++) {
        int next = rep[(i + k * j - 1) % primezip];
        int flag = 1;
        for (h = 0; h < size; h++) {
          if (clause[h] == next) flag = 0;
          if (clause[h] >  next) h = size; }
        if (flag) {
          for (h = size; h > 0; h--) {
            if (clause[h-1] > next)  clause[h  ] = clause[h-1];
            else                     break; }
          clause[h] = next; size++; } }
      assert (size);
      if (stored + size >= alloc) {
        alloc *= 2;
        clauses = realloc (clauses, sizeof(int) * alloc); }
      for (k = 0; k < size; k++) clauses[stored++] = clause[k] + 1;
      clauses[stored++] = 0; num++; }
//      for (h = 1; h <= nrofsets; h++ ) {
//	for (k = 0; k < size; k++) {
//          printf ("-%i ", clause[k] * nrofsets + h); }
//        printf("0\n"); } }


  tmp = 0;
  for (i = 1; i <= num; i++) {
    for (h = 1; h <= nrofsets; h++) {
      for (k = 0; clauses[tmp + k] > 0; k++)
        printf ("-%i ", (clauses[tmp+k] - 1) * nrofsets + h);
      printf("0\n"); }
    tmp += k + 1; }

//  for (i = 0; i < stored; i++)
//  printf("%i\n", clauses[i]);

  return 1;
}
