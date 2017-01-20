#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

//#define BLOCKED
#define SBP
//#define INTERNAL_ROOT
#define MIN_PRIME	2
#define POWER		3
//#define MULTICOLOR	2
//#define ROTATION
#define INV

//#define PLUSPRIME
//#define NOFULLROTATION	0

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

unsigned int smallest_divider( unsigned int n ) {
        unsigned int i, max = sqrt(n);

        for( i = 2; i <= max; i++)
           if( (n % i) ==  0 )
		return i;

        return n;
}

unsigned int rotation_factor( unsigned int prime, unsigned int primezip ) {
        unsigned int zip, zip_div;

	zip = primezip / prime;

	if (zip == 1) return prime;

	zip_div = greatest_divider_rec( zip );

	if( zip_div * zip_div == zip ) return zip_div * prime;

        return prime;
}

int main (int argc , char *argv[] ) {
	int h, i, j, k, l, tmp;
	int nrofsets, progression, numbers, primezip, prime, full_size, multi_color, q, qmin;
	int nrofvars, nrofclauses, root, root_start, rotation_start, inv_start, rot_fac;

        if (argc <= 3) {
          printf ("c wrong input: ./internal_sat sets length prime\n");
          exit (0); }

        nrofsets    = atoi (argv[1]);
 	progression = atoi (argv[2]);
	primezip    = atoi (argv[3]);

	full_size   = primezip * (progression - 1);
	prime       = greatest_divider_rec( primezip );
	root        = get_root( prime, primezip );
	rot_fac     = rotation_factor( prime, primezip );

	qmin = smallest_divider( primezip / prime );

//	if( qmin > 3 ) qmin = 1;

//	qmin = 1;		// test

	printf("c root = %i\n", root );
	printf("c qmin = %i\n", qmin );

	if( (prime < MIN_PRIME) || (root == 1) )
	{
	    printf("p cnf 1 2\n1 0\n -1 0\n");
	    return 0;
	}

	nrofclauses = 0;
	for( i = 1; i <= primezip; i++ )
	    for( j = 1; (j*(progression-1)) < (full_size - i + 1); j++ )
		nrofclauses++;

	nrofvars    = primezip * nrofsets;
	nrofclauses = primezip + nrofsets * nrofclauses;

	nrofclauses += 1;
//	nrofclauses += primezip;
#ifdef SBP
	nrofclauses += (nrofsets-2) * (nrofsets-1) / 2;
#endif
#ifdef INTERNAL_ROOT
	root_start   = nrofvars;
	nrofvars    += qmin * nrofsets * nrofsets;
	nrofclauses += qmin * nrofsets * nrofsets * (nrofsets-1);
//	nrofvars    += MULTICOLOR * nrofsets * nrofsets;
//	nrofclauses += MULTICOLOR * nrofsets * nrofsets * (nrofsets-1);
//	nrofclauses += nrofsets * nrofsets * (primezip-1);
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

#ifdef PLUSPRIME
	if( primezip != prime )
	    nrofclauses += (primezip - (primezip/prime)) * (nrofsets-NOFULLROTATION) * 2;
#endif
  printf("p cnf %i %i\n", nrofvars, nrofclauses );

  printf("1 0\n");

  for (i = 0; i < primezip; i++) {
    for (j = 1; j <= nrofsets; j++)
      printf("%i ", i * nrofsets + j);
    printf("0\n"); }

#ifdef PLUSPRIME
  if (primezip != prime)
    for (i = 0; i < primezip; i++) {
      if ((i % prime) == prime - 1) continue;

      for( j = 1; j <= nrofsets-NOFULLROTATION; j++ ) {
		printf("%i -%i 0\n", i*nrofsets + j, ((i+prime) % primezip) * nrofsets + ((j+0)%nrofsets) + 1 );
		printf("-%i %i 0\n", i*nrofsets + j, ((i+prime) % primezip) * nrofsets + ((j+0)%nrofsets) + 1 );
//		printf("%i -%i 0\n", i*nrofsets + j, ((i+prime) % primezip) * nrofsets + ((j+1)%nrofsets) + 1 );
//		printf("-%i %i 0\n", i*nrofsets + j, ((i+prime) % primezip) * nrofsets + ((j+1)%nrofsets) + 1 );
    } }
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
        printf("-%i -%i 0\n", inv_start + i*nrofsets + k, inv_start + j*nrofsets + k );

  for (i = 0; i < primezip; i++) {
    if ((i % prime) == (prime - 1)) continue;

  tmp = (primezip - i - 2);
  for (j = 1; j <= nrofsets; j++)
    for (k = 1; k <= nrofsets; k++)
      printf("-%i -%i %i 0\n", i * nrofsets + j, tmp*nrofsets + k,
                               inv_start + (j-1) * nrofsets + k); }
#endif

#ifdef SBP
  for (i = 0; i < nrofsets; i++)
    for (j = i+3; j <= nrofsets; j++)  // why + 3?
      printf("-%i 0\n", primezip*nrofsets + i*nrofsets + j);
#endif

#ifdef INTERNAL_ROOT
  for (q = 0; q < qmin; q++) {
    multi_color = q * nrofsets * nrofsets;
    for (i = 0; i < nrofsets; i++) {
      tmp = root_start + multi_color + i * nrofsets;
      for (j = 1; j <= nrofsets; j++)
        for (k = j + 1; k <= nrofsets; k++)
          printf("-%i -%i 0\n", tmp + j, tmp + k ); } }

  for (q = 0; q < qmin; q++) {
    multi_color = q * nrofsets * nrofsets;
    for (i = 0; i < nrofsets; i++)
      for (j = i + 1; j < nrofsets; j++)
	for( k = 1; k <= nrofsets; k++)
          printf("-%i -%i 0\n", root_start + multi_color + i * nrofsets + k, root_start + multi_color + j * nrofsets + k ); }

  multi_color = 0;

  for( i = 0; i < primezip; i++ ) {
    if( (i % prime) == (prime - 1) ) continue;
//    if( (i % primezip) == (primezip - 1) ) continue;

#ifdef MULTICOLOR
    multi_color = (i % qmin) * nrofsets * nrofsets;
#endif
    tmp = i;
    for (j = 1; j <= POWER; j++)
      tmp = (((tmp+1) * root) - 1) % primezip;

    for (j = 1; j <= nrofsets; j++)
      for( k = 1; k <= nrofsets; k++ )
        printf("-%i -%i %i 0\n", i * nrofsets + j, tmp * nrofsets + k,
  	                         root_start + multi_color +  (j-1) * nrofsets + k); }
#endif

#ifdef ROTATION
  for (i = 0; i < nrofsets; i++) {
    tmp = rotation_start + i * nrofsets;
    for (j = 1; j <= nrofsets; j++)
      for (k = j + 1; k <= nrofsets; k++)
	printf ("-%i -%i 0\n", tmp + j, tmp + k); }

	for( i = 0; i < nrofsets; i++ )
	    for( j = i + 1; j < nrofsets; j++ )
	        for( k = 1; k <= nrofsets; k++ )
		    printf("-%i -%i 0\n", rotation_start + i*nrofsets + k, rotation_start + j*nrofsets + k );

	for( i = 0; i < primezip; i++ )
	{
	    if( (i % prime) == (prime - 1) ) continue;

	    tmp = (i + rot_fac) % primezip;
//	    tmp = (i + prime) % primezip;
	    for( j = 1; j <= nrofsets; j++ )
	        for( k = 1; k <= nrofsets; k++ )
		    printf("-%i -%i %i 0\n", i * nrofsets + j, tmp*nrofsets + k,
					     rotation_start + (j-1) * nrofsets + k  );
	}
#endif

  // print the actual constraints
  for (i = 1; i <= primezip; i++)
    for (j = 1; (j*(progression-1)) < (full_size - i + 1); j++ ) {
      for (h = 1; h <= nrofsets; h++ ) {
	for( k = 0; k < progression; k++ ) {
	  int tmp = (i + k * j - 1) % primezip;
	  printf ("-%i ", tmp * nrofsets + h); }
        printf("0\n"); } }

  return 1;
}