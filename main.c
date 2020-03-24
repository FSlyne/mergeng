#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAIN_C

#include "main.h"

/*	main() */

int main (int argc, char *argv[])
{
	int num_nodes = 0;
	FILE* fp_seed = NULL;
	char *s;

	int interval = 0;
	int grant = 0;
	int slot = 0;
	int prob = 0;
	int run = 1;

	char out_result[200] = "";

	seed = -1;

	/* Parsing of the Command line options. */

	while (--argc > 0)
	{
		if ((*++argv)[0] == '-')
		{
			s = argv[0] + 1;
			switch (*s)
			{
			case 's':
				if (strncmp (s, "slot=", 5) == 0)
				{
					s += 5;
					sscanf (s, "%d", &slot);
					if (slot <= 0)
					{
						error ("Warning:  Non positive max allowed frame size per VNO\n", stderr);
						exit(1);
					}
				}
				else if (strncmp(s, "seed=", 5) == 0)
				{
					s += 5;
					sscanf(s, "%ld", &seed);
				}
				else
					printf ("Warning: Invalid option -%s ignored\n", s);
				break;

			case 'i':
				if (strncmp(s, "interval=", 9) == 0)
				{
					s += 9;
					sscanf(s, "%d", &interval);
					if (interval <= 0)
					{
						error("Warning:  Non positive interval\n", stderr);
						exit(1);
					}
				}
				else
					printf("Warning: Invalid option -%s ignored\n", s);
				break;

			case 'g':
				if (strncmp(s, "grant=", 6) == 0)
				{
					s += 6;
					sscanf(s, "%d", &grant);
					if (grant <= 0)
					{
						error("Warning:  Non positive grant size\n", stderr);
						exit(1);
					}
				}
				else
					printf("Warning: Invalid option -%s ignored\n", s);
				break;

			case 'p':
				if (strncmp(s, "prob=", 5) == 0)
				{
					s += 5;
					sscanf(s, "%d", &prob);
					if (prob < 0)
					{
						error("Warning:  Non positive grant size\n", stderr);
						exit(1);
					}
				}
				else
					printf("Warning: Invalid option -%s ignored\n", s);
				break;

			case 'r':
				if (strncmp(s, "run=", 4) == 0)
				{
					s += 4;
					sscanf(s, "%d", &run);
					if (run <= 0)
					{
						error("Warning:  Non positive grant size\n", stderr);
						exit(1);
					}
				}
				else
					printf("Warning: Invalid option -%s ignored\n", s);
				break;

			case 'o':
				if (strncmp (s, "out_log=", 8) == 0)
				{
					s += 8;
					strcpy (out_result, s);
				}
				else
					printf ("Warning: Invalid option -%s ignored\n", s);
				break;

			default:
				printf ("Warning: Invalid option -%s ignored\n", s);
			}
		}
	}

	srand (time (NULL));
	seed = rand ();
	fp_seed = fopen ("seed.dat","w");
	fprintf (fp_seed,"%ld",seed);
	fclose (fp_seed);

	PON_vDBA_merging_engine(slot, interval, grant, prob, run);
}
