/* main.h -- 22/08/19 */


#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <math.h>
#include <float.h>
/*----------------------------------------------------------------------------*
Tipi di dato scalare
*----------------------------------------------------------------------------*/
#ifndef BOOL
typedef int BOOL;
#define TRUE 1
#define FALSE 0
#endif


typedef double TRAFFIC;
typedef double CAPACITY;
typedef int NODEID;
typedef int ONUID;
typedef int VNOID;
typedef double COST;
#define MAX_COST MAX_DOUBLE


#include "matrix.h"
#include "random.h"
#include "vpon.h"

/* Global variables */

#ifdef MAIN_C
long int seed;
#else
extern long int seed;
#endif


/* Prototipi per matrix.c */
void error (char *msg, FILE * fp);
void *MMmalloc (size_t size, const char *f_name);

// added for MT-PON work
void PON_vDBA_merging_engine(int max_slot, int max_interval, int max_grant, int prob, int run);
void print_statistics();
void generate_Random_BWMap(int max_slot, int max_interval, int max_grant, int prob, int iteration);
BOOL find_collision(int k);
BOOL find_placement(int k, int collision_job);
BOOL find_collision2(int k);
BOOL find_placement2(int k, int collision_job);
void sorted_priority_merging_engine();
int fill_physical_bwmap_sorted_priority(int t_cont, int index);
BOOL check_PON_frame_slot_availability(int st, int ft);
void dummy_allocate_slots(int st, int ft);
void sort_ascending_bwmap();
void print_traffic_detail_VNO1();
void print_traffic_detail_VNO2();
int shift_unsatisfied_requests(int t_cont, int index);
BOOL check_shifting(int st, int ft, int t_cont);
int find_next_empty_slot(int i);
BOOL check_slot_validity(int st, int ft);
void store_unsatisfied_requests();
void calculate_results(int i);
void reset_everthing();
void served_traffic_confidence_interval(int run);
void delay_confidence_interval(int run);
int serve_previous_frame_requests(int i, int index);
