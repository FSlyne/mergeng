
#include "main.h"
#include "vpon.h"


int allocated_slots[3000];
int slot_counter = 0;
BW_MAP_Random physical_bwmap, unsatisfied_bwmap, vno1_bwmap, vno2_bwmap, dropped_bwmap;

int served_perc[100][4];
int delay_perc[100][4];

 FILE* fp_vno1_grant = NULL;                // Uncomment this if want to Open Input Traffic File
 FILE* fp_vno2_grant = NULL;                // Uncomment this if want to Open Input Traffic File

///////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////
/////////////////////// Code for PON VDBA Merging Engine ////////////////////////
/////////////////////////////////////////////////////////////////////////////////


void PON_vDBA_merging_engine(int max_slot, int max_interval, int max_grant, int prob, int run)
{
	int i;
	char name[10];
	
	 fp_vno1_grant = fopen("VNO1_grant.txt", "r");         // Uncomment this if want to Open Input Traffic File
	 fp_vno2_grant = fopen("VNO2_grant.txt", "r");         // Uncomment this if want to Open Input Traffic File

	for (i = 0; i <= run - 1 ; i++)
	{
//		mkdir(itoa(i, name, 10));
		// write a function to produce a vBWMap for each VNO
		generate_Random_BWMap(max_slot, max_interval, max_grant, prob, i);

		// Write a function to print traffic characteristics in a file

		 print_traffic_detail_VNO1();             // Uncomment this if want data from Input Traffic File
		 print_traffic_detail_VNO2();             // Uncomment this if want data from Input Traffic File

		// write a function to merge the vBWMaps and produce a single BWMap
		sorted_priority_merging_engine();

		// write a function to print results and statistics
		print_statistics();
		calculate_results(i);
		reset_everthing();
	}
	served_traffic_confidence_interval(run);
	delay_confidence_interval(run);
	fclose(fp_vno1_grant);                           // Uncomment this if want data from Input Traffic File
	fclose(fp_vno2_grant);                           // Uncomment this if want data from Input Traffic File
}


void generate_Random_BWMap(int max_slot, int max_interval, int max_grant, int prob, int iteration)
{
	int i = 1, t_cont = 1, start_time = 0, grant_size = 0, total_slots = 0, k = 0;
	int interval = 0, job_id = 1, total_grant = 0, allowed_grant_VNO1 = 576, allowed_grant_VNO2 = 576;            // allowed_grant is the Offered Load[%] on BWMap i.e if 576 load is 100%
	double grant_percentage = 0.0;
	int allowed_gap;
	FILE* fp_tmp = NULL;


	// Uncomment this section if u want to read traffic from file //

	fp_tmp = fopen("VNO1.dat", "a");
	if (fp_tmp == NULL)
	{
		error("Can not open file to write VNO1 bandwidth map details", stderr);
		exit(1);
	}

	allowed_gap = 1152 - allowed_grant_VNO1;
	while (total_slots < (max_slot - max_grant))
	{
//		grant_size = uniform(1, max_grant, &seed);          // comment this
//		if (fp_vno1_grant != NULL)

			fscanf(fp_vno1_grant,"%d",&grant_size);         // Uncomment this if want to read values from Input Traffic File
		total_grant = total_grant + grant_size;

		if (total_grant >= allowed_grant_VNO1)
		{
			total_grant = total_grant - grant_size;
			break;
		}
		vno1_bwmap.allocated_slots[k].grant_size = grant_size;

		grant_percentage = (double)grant_size / (double)allowed_grant_VNO1;

		if (prob == 10)
			t_cont = rand() % 4;
		else
			t_cont = Prob_weighted_random_number(prob, &seed);

		vno1_bwmap.allocated_slots[k].traffic_class = t_cont;

		vno1_bwmap.allocated_slots[k].slot_occupied = TRUE;

		vno1_bwmap.allocated_slots[k].start_time = start_time;

		vno1_bwmap.allocated_slots[k].onu_id = 1;

		interval = grant_percentage * allowed_gap;                      // Calculating interval on the basis of the percentage of the grant size or the Allowed Gap in BWMap
//		interval = uniform(1, max_interval, &seed);

		vno1_bwmap.allocated_slots[k].job_id = k+1;

		vno1_bwmap.allocated_slots[k].delay = 0;

		fprintf(fp_tmp, "%d %d %d %d %d\n", start_time, start_time + grant_size, grant_size, t_cont, k + 1);

		total_slots = total_slots + grant_size + interval;
		start_time = total_slots;

//		printf("VNO ID: 1, Traffic class: %d, Start time is %d, grant size is %d and interval is %d\n", vno1_bwmap.allocated_slots[k].traffic_class, vno1_bwmap.allocated_slots[k].start_time, vno1_bwmap.allocated_slots[k].grant_size, interval);
		k++;
		job_id++;
	}
	fprintf(fp_tmp, "\n");
	fclose(fp_tmp);

	// Uncomment this section if you want to read traffic from file //

	fp_tmp = fopen("VNO2.dat", "a");
	if (fp_tmp == NULL)
	{
		error("Can not open file to write VNO2 bandwidth map details", stderr);
		exit(1);
	}

	printf("\n");
	total_slots = 0;
	k = 0;
	start_time = 0;
	total_grant = 0;

	allowed_gap = 1152 - allowed_grant_VNO2;
	while (total_slots < (max_slot - max_grant))
	{
//		if (fp_vno2_grant != NULL)
			fscanf(fp_vno2_grant, "%d", &grant_size);            // Uncomment this if want to read values from Input Traffic File
	//	grant_size = uniform(1, max_grant, &seed);                // comment this
		total_grant = total_grant + grant_size;

		if (total_grant >= allowed_grant_VNO1)
		{
			total_grant = total_grant - grant_size;
			break;
		}
		vno2_bwmap.allocated_slots[k].grant_size = grant_size;

		grant_percentage = (double)grant_size / (double)allowed_grant_VNO1;

		if (prob == 10)
			t_cont = rand() % 4;
		else
			t_cont = Prob_weighted_random_number(prob, &seed);

		vno2_bwmap.allocated_slots[k].traffic_class = t_cont;

		vno2_bwmap.allocated_slots[k].slot_occupied = TRUE;

		vno2_bwmap.allocated_slots[k].start_time = start_time;

		vno2_bwmap.allocated_slots[k].onu_id = 2;

		interval = grant_percentage * allowed_gap;                           // Interval value is calculated on the basis of %age of traffic served
//		interval = uniform(1, max_interval, &seed);

		vno2_bwmap.allocated_slots[k].job_id = k + 1;

		vno2_bwmap.allocated_slots[k].delay = 0;

		fprintf(fp_tmp, "%d %d %d %d %d\n", start_time, start_time + grant_size, grant_size, t_cont, k + 1);

		total_slots = total_slots + grant_size + interval;
		start_time = total_slots;
	
//		printf("VNO ID: 2, Traffic class: %d, Start time is %d, grant size is %d and interval is %d\n", vno2_bwmap.allocated_slots[k].traffic_class, vno2_bwmap.allocated_slots[k].start_time, vno2_bwmap.allocated_slots[k].grant_size, interval);
		k++;
	}

	fprintf(fp_tmp, "\n");
	fclose(fp_tmp);
}


void print_traffic_detail_VNO1()
{
	int trf_total = 0, trf0 = 0, trf1 = 0, trf2 = 0, trf3 = 0, i = 0, this_trf = 0;
	FILE* fp_tmp = NULL;

	fp_tmp = fopen("VNO1_traffic_details.dat", "a");
	if (fp_tmp == NULL)
	{
		error("Can not open file to write traffic characteristics", stderr);
		exit(1);
	}
	fprintf(fp_tmp, "\n");

	while (vno1_bwmap.allocated_slots[i].slot_occupied == TRUE)
	{
		this_trf = vno1_bwmap.allocated_slots[i].grant_size;
		trf_total = trf_total + this_trf;

		if (vno1_bwmap.allocated_slots[i].traffic_class == 0)
			trf0 = trf0 + this_trf;
		else if (vno1_bwmap.allocated_slots[i].traffic_class == 1)
			trf1 = trf1 + this_trf;
		else if (vno1_bwmap.allocated_slots[i].traffic_class == 2)
			trf2 = trf2 + this_trf;
		else if (vno1_bwmap.allocated_slots[i].traffic_class == 3)
			trf3 = trf3 + this_trf;
	
		i++;
	}

	fprintf(fp_tmp, "%d\n", trf_total);
//	fprintf(fp_tmp, "%d %d %d %d\n", trf0, trf1, trf2, trf3);
//	fprintf(fp_tmp, "\n");
	fclose(fp_tmp);
}


void print_traffic_detail_VNO2()
{
	int trf_total = 0, trf0 = 0, trf1 = 0, trf2 = 0, trf3 = 0, i = 0, this_trf = 0;
	FILE* fp_tmp = NULL;

	fp_tmp = fopen("VNO2_traffic_details.dat", "a");
	if (fp_tmp == NULL)
	{
		error("Can not open file to write traffic characteristics", stderr);
		exit(1);
	}
	fprintf(fp_tmp, "\n");

	while (vno2_bwmap.allocated_slots[i].slot_occupied == TRUE)
	{
		this_trf = vno2_bwmap.allocated_slots[i].grant_size;
		trf_total = trf_total + this_trf;

		if (vno2_bwmap.allocated_slots[i].traffic_class == 0)
			trf0 = trf0 + this_trf;
		else if (vno2_bwmap.allocated_slots[i].traffic_class == 1)
			trf1 = trf1 + this_trf;
		else if (vno2_bwmap.allocated_slots[i].traffic_class == 2)
			trf2 = trf2 + this_trf;
		else if (vno2_bwmap.allocated_slots[i].traffic_class == 3)
			trf3 = trf3 + this_trf;

		i++;
	}

	fprintf(fp_tmp, "%d\n", trf_total);
//	fprintf(fp_tmp, "%d %d %d %d\n", trf0, trf1, trf2, trf3);
//	fprintf(fp_tmp, "\n");
	fclose(fp_tmp);
}


int find_collision (int k)
{
	int start_time, finish_time, check_start, check_finish, i = 0;

	start_time = vno1_bwmap.allocated_slots[k].start_time;
	finish_time = start_time + vno1_bwmap.allocated_slots[k].grant_size;

	while (vno2_bwmap.allocated_slots[i].slot_occupied == TRUE)
	{
		check_start = vno2_bwmap.allocated_slots[i].start_time;
		check_finish = check_start + vno2_bwmap.allocated_slots[i].grant_size;

		if (check_start == start_time)
			return vno2_bwmap.allocated_slots[i].job_id;
		else if (check_start > start_time && check_start < finish_time)
			return vno2_bwmap.allocated_slots[i].job_id;
		else
			i++;
	}
	return 0;
}


BOOL find_placement (int k, int collision_job)
{
	int i = 0;

	while (vno2_bwmap.allocated_slots[i].job_id != collision_job)
	{
		i++;
	}
	if (vno1_bwmap.allocated_slots[k].traffic_class >= vno2_bwmap.allocated_slots[i].traffic_class)
		return TRUE;
	else
		return FALSE;
}


int find_collision2(int k)
{
	int start_time, finish_time, check_start, check_finish, i = 0;

	start_time = vno2_bwmap.allocated_slots[k].start_time;
	finish_time = start_time + vno2_bwmap.allocated_slots[k].grant_size;

	while (vno1_bwmap.allocated_slots[i].slot_occupied == TRUE)
	{
		check_start = vno1_bwmap.allocated_slots[i].start_time;
		check_finish = check_start + vno1_bwmap.allocated_slots[i].grant_size;

		if (check_start == start_time)
			return vno1_bwmap.allocated_slots[i].job_id;
		else if ((check_start > start_time && check_start < finish_time) || (check_finish < finish_time && check_finish > start_time))
			return vno1_bwmap.allocated_slots[i].job_id;
		else
			i++;
	}
	return 0;
}


BOOL find_placement2(int k, int collision_job)
{
	int i = 0;

	while (vno1_bwmap.allocated_slots[i].job_id != collision_job)
	{
		i++;
	}
	if (vno2_bwmap.allocated_slots[k].traffic_class >= vno1_bwmap.allocated_slots[i].traffic_class)
		return TRUE;
	else
		return FALSE;
}


void sorted_priority_merging_engine()
{
	int i = 1, k = 0;
	int index = 0;
	BOOL slot_occupied = TRUE;

	FILE* fp_tmp = NULL;

	for (i = 3; i >= 0; i--)
	{
		index = serve_previous_frame_requests (i, index);
		index = fill_physical_bwmap_sorted_priority (i, index);
		index = shift_unsatisfied_requests(i, index);
	}

	store_unsatisfied_requests();

	fp_tmp = fopen("final_allocation_map.dat", "w");
	if (fp_tmp == NULL)
	{
		error("Can not open file to write VNO1 bandwidth map details", stderr);
		exit(1);
	}

	sort_ascending_bwmap();

//	printf("\n**********Printing final physical BW Map**********\n");
	while (slot_occupied == TRUE)
	{
//		printf("VNO ID: %d, Traffic class: %d, Start time is %d and grant size is %d\n", physical_bwmap.allocated_slots[k].onu_id, physical_bwmap.allocated_slots[k].traffic_class, physical_bwmap.allocated_slots[k].start_time, physical_bwmap.allocated_slots[k].grant_size);
		fprintf(fp_tmp,"VNO ID: %d, Traffic class: %d, Start time is %d, grant size is %d and delay is %d\n", physical_bwmap.allocated_slots[k].onu_id, physical_bwmap.allocated_slots[k].traffic_class, physical_bwmap.allocated_slots[k].start_time, physical_bwmap.allocated_slots[k].grant_size, physical_bwmap.allocated_slots[k].delay);
		k++;
		slot_occupied = physical_bwmap.allocated_slots[k].slot_occupied;
	}
	fclose(fp_tmp);
}

////////////////////////////**********************************************//////////////////////////
//////////////////////////  Here I have to do make another physical BWMap /////////////////////////

int fill_physical_bwmap_sorted_priority (int t_cont, int index)
{
	int k = 0, start_time = 0;
	BOOL collision = FALSE, placement = FALSE, slot_availability;
	BOOL slot_occupied = vno1_bwmap.allocated_slots[k].slot_occupied;

	if (index > 0)
		start_time = physical_bwmap.allocated_slots[index - 1].start_time + physical_bwmap.allocated_slots[index - 1].grant_size;

	while (slot_occupied == TRUE)
	{
		if (vno1_bwmap.allocated_slots[k].traffic_class == t_cont)
		{
			collision = find_collision(k);
			if(collision > 0)
				placement = find_placement(k, collision);

			if (collision == 0 || placement == TRUE)
			{
				slot_availability = check_PON_frame_slot_availability(vno1_bwmap.allocated_slots[k].start_time, vno1_bwmap.allocated_slots[k].grant_size);

				if (slot_availability == TRUE)
				{
					dummy_allocate_slots(vno1_bwmap.allocated_slots[k].start_time, vno1_bwmap.allocated_slots[k].grant_size);

					vno1_bwmap.allocated_slots[k].allocated = TRUE;
					physical_bwmap.allocated_slots[index] = vno1_bwmap.allocated_slots[k];
					index++;
				}
				else
					vno1_bwmap.allocated_slots[k].allocated = FALSE;
			}
		}
		k++;
		slot_occupied = vno1_bwmap.allocated_slots[k].slot_occupied;
	}

	k = 0;
	slot_occupied = TRUE;

	while (slot_occupied == TRUE)
	{
		if (vno2_bwmap.allocated_slots[k].traffic_class == t_cont)
		{
			collision = find_collision2(k);
			if (collision > 0)
				placement = find_placement2(k, collision);

			if (collision == 0 || placement == TRUE)
			{
				slot_availability = check_PON_frame_slot_availability(vno2_bwmap.allocated_slots[k].start_time, vno2_bwmap.allocated_slots[k].grant_size);

				if (slot_availability == TRUE)
				{
					dummy_allocate_slots(vno2_bwmap.allocated_slots[k].start_time, vno2_bwmap.allocated_slots[k].grant_size);

					vno2_bwmap.allocated_slots[k].allocated = TRUE;
					physical_bwmap.allocated_slots[index] = vno2_bwmap.allocated_slots[k];
					index++;
				}
				else
					vno2_bwmap.allocated_slots[k].allocated = FALSE;
			}
		}
		k++;
		slot_occupied = vno2_bwmap.allocated_slots[k].slot_occupied;
	}

	return index;
}


BOOL check_PON_frame_slot_availability (int st, int ft)
{
	int i;
	ft = st + ft;
	for (i = st; i < ft; i++)
	{
		if (allocated_slots[i] == 1)
			return FALSE;
	}
	return TRUE;
}


void dummy_allocate_slots(int st, int ft)
{
	int i;
	ft = st + ft;
	for (i = st; i < ft; i++)
		allocated_slots[i] = 1;
}


void sort_ascending_bwmap()
{
	int i = 0, count = 0, j;
	BOOL slot_occupied = TRUE;
	SLOT_Random_FRAME temp;

	while (slot_occupied == TRUE)
	{
		count++;
		slot_occupied = physical_bwmap.allocated_slots[count].slot_occupied;
	}

	for (i = 0; i < count - 1; i++)
	{
		for (j = 0; j < count - i - 1; j++)
		{
			if (physical_bwmap.allocated_slots[j].start_time > physical_bwmap.allocated_slots[j + 1].start_time)
			{
				temp = physical_bwmap.allocated_slots[j];
				physical_bwmap.allocated_slots[j] = physical_bwmap.allocated_slots[j + 1];
				physical_bwmap.allocated_slots[j + 1] = temp;
			}
		}
	}
}


void print_statistics()
{
	int i, k = 0, vno1_dropped = 0, vno2_dropped = 0;
	int vno1_dropped_class[4] = { 0, 0, 0, 0};
	int vno2_dropped_class[4] = { 0, 0, 0, 0};
	BOOL slot_occupied = TRUE;
	FILE* fp_tmp = NULL;

	fp_tmp = fopen("dropped_requests_details.dat", "w");
	if (fp_tmp == NULL)
	{
		error("Can not open file to write VNO1 bandwidth map details", stderr);
		exit(1);
	}

	while (slot_occupied == TRUE)
	{
		if (vno1_bwmap.allocated_slots[k].allocated == FALSE)
		{
			if (vno1_bwmap.allocated_slots[k].traffic_class == 0)
				vno1_dropped_class[0]++;
			else if (vno1_bwmap.allocated_slots[k].traffic_class == 1)
				vno1_dropped_class[1]++;
			else if (vno1_bwmap.allocated_slots[k].traffic_class == 2)
				vno1_dropped_class[2]++;
			else if (vno1_bwmap.allocated_slots[k].traffic_class == 3)
				vno1_dropped_class[3]++;
			
			fprintf(fp_tmp, "VNO ID: %d, Traffic class: %d, Start time is %d and grant size is %d\n", vno1_bwmap.allocated_slots[k].onu_id, vno1_bwmap.allocated_slots[k].traffic_class, vno1_bwmap.allocated_slots[k].start_time, vno1_bwmap.allocated_slots[k].grant_size);
			vno1_dropped++;
		}
		k++;
		slot_occupied = vno1_bwmap.allocated_slots[k].slot_occupied;
	}

	k = 0;
	slot_occupied = TRUE;

	while (slot_occupied == TRUE)
	{
		if (vno2_bwmap.allocated_slots[k].allocated == FALSE)
		{
			if (vno2_bwmap.allocated_slots[k].traffic_class == 0)
				vno2_dropped_class[0]++;
			else if (vno2_bwmap.allocated_slots[k].traffic_class == 1)
				vno2_dropped_class[1]++;
			else if (vno2_bwmap.allocated_slots[k].traffic_class == 2)
				vno2_dropped_class[2]++;
			else if (vno2_bwmap.allocated_slots[k].traffic_class == 3)
				vno2_dropped_class[3]++;

			fprintf(fp_tmp, "VNO ID: %d, Traffic class: %d, Start time is %d and grant size is %d\n", vno2_bwmap.allocated_slots[k].onu_id, vno2_bwmap.allocated_slots[k].traffic_class, vno2_bwmap.allocated_slots[k].start_time, vno2_bwmap.allocated_slots[k].grant_size);
			vno2_dropped++;
		}
		k++;
		slot_occupied = vno2_bwmap.allocated_slots[k].slot_occupied;
	}
	
	fclose(fp_tmp);

	fp_tmp = fopen("dropped_requests_numbers.dat", "w");
	if (fp_tmp == NULL)
	{
		error("Can not open file to write VNO1 bandwidth map details", stderr);
		exit(1);
	}

	fprintf(fp_tmp, "\nVNO ID: 1, Dropped Requests = %d\n\n", vno1_dropped);
	fprintf(fp_tmp, "\nVNO ID: 2, Dropped Requests = %d\n\n", vno2_dropped);

	for (i = 0; i < 4; i++)
	{
		fprintf(fp_tmp, "\nVNO ID: 1, Dropped Requests TCONT %d = %d", i, vno1_dropped_class[i]);
		fprintf(fp_tmp, "\nVNO ID: 2, Dropped Requests TCONT %d = %d", i, vno2_dropped_class[i]);
	}

	fclose(fp_tmp);
}


int shift_unsatisfied_requests(int t_cont, int index)
{
	int k = 0, start_time = 0;
	BOOL collision = FALSE, placement = FALSE, slot_availability;
	BOOL slot_occupied = vno1_bwmap.allocated_slots[k].slot_occupied;

	while (slot_occupied == TRUE)
	{
		if (vno1_bwmap.allocated_slots[k].traffic_class == t_cont && vno1_bwmap.allocated_slots[k].allocated == FALSE)
		{
			slot_availability = check_shifting(vno1_bwmap.allocated_slots[k].start_time, vno1_bwmap.allocated_slots[k].grant_size, t_cont);

			if (slot_availability == TRUE)
			{
				dummy_allocate_slots(slot_counter, vno1_bwmap.allocated_slots[k].grant_size);

				vno1_bwmap.allocated_slots[k].allocated = TRUE;
				physical_bwmap.allocated_slots[index] = vno1_bwmap.allocated_slots[k];
				physical_bwmap.allocated_slots[index].start_time = slot_counter;
				physical_bwmap.allocated_slots[index].delay = slot_counter - vno1_bwmap.allocated_slots[k].start_time;
				index++;
			}
			else
				vno1_bwmap.allocated_slots[k].allocated = FALSE;
		}
		k++;
		slot_occupied = vno1_bwmap.allocated_slots[k].slot_occupied;
	}

	k = 0;
	slot_occupied = TRUE;

	while (slot_occupied == TRUE)
	{
		if (vno2_bwmap.allocated_slots[k].traffic_class == t_cont && vno2_bwmap.allocated_slots[k].allocated == FALSE)
		{
			slot_availability = check_shifting(vno2_bwmap.allocated_slots[k].start_time, vno2_bwmap.allocated_slots[k].grant_size, t_cont);

			if (slot_availability == TRUE)
			{
				dummy_allocate_slots(slot_counter, vno2_bwmap.allocated_slots[k].grant_size);

				vno2_bwmap.allocated_slots[k].allocated = TRUE;
				physical_bwmap.allocated_slots[index] = vno2_bwmap.allocated_slots[k];
				physical_bwmap.allocated_slots[index].start_time = slot_counter;
				physical_bwmap.allocated_slots[index].delay = slot_counter - vno2_bwmap.allocated_slots[k].start_time;
				index++;
			}
			else
				vno2_bwmap.allocated_slots[k].allocated = FALSE;
		}
		k++;
		slot_occupied = vno2_bwmap.allocated_slots[k].slot_occupied;
	}
	return index;
}


BOOL check_shifting(int st, int ft, int t_cont)
{
	int i, req_slots;
	BOOL slots_available = FALSE;

	ft = st + ft;
	req_slots = ft - st;

	if (t_cont <= 1)
		slot_counter = 0;
	else
		slot_counter = st;

	while (slots_available == FALSE)
	{
		slot_counter = find_next_empty_slot(slot_counter);
		slots_available = check_slot_validity(slot_counter, req_slots);
		if (slots_available == TRUE)
			return TRUE;
		else if (slot_counter >= 1152)
			return FALSE;
		else
			continue;
	}
}


int find_next_empty_slot (int i)
{
	while (allocated_slots[i] != 0)
	{
		i++;
	}
	return i;
}


BOOL check_slot_validity(int st, int ft)
{
	int i;
	ft = st + ft;
	for (i = st; i < ft; i++)
	{
		if (allocated_slots[i] == 1)
		{
			slot_counter = i;
			while (allocated_slots[slot_counter] != 0)
				slot_counter++;
			return FALSE;
		}
	}
	return TRUE;
}


void store_unsatisfied_requests()
{
	int i = 0, k = 0;
	BOOL slot_occupied = TRUE;

	while (slot_occupied == TRUE)
	{
		if (vno1_bwmap.allocated_slots[k].allocated == FALSE)
		{
			unsatisfied_bwmap.allocated_slots[i] = vno1_bwmap.allocated_slots[k];
			i++;
		}
		k++;
		slot_occupied = vno1_bwmap.allocated_slots[k].slot_occupied;
	}

	k = 0;
	slot_occupied = TRUE;

	while (slot_occupied == TRUE)
	{
		if (vno2_bwmap.allocated_slots[k].allocated == FALSE)
		{
			unsatisfied_bwmap.allocated_slots[i] = vno2_bwmap.allocated_slots[k];
			i++;
		}
		k++;
		slot_occupied = vno2_bwmap.allocated_slots[k].slot_occupied;
	}
}


void calculate_results(int run)
{
	int i, k = 0, t_cont;
	double total, served;
	double perc_served = 0.0;

	int total_traffic[4] = { 0, 0, 0, 0};
	int served_traffic[4] = { 0, 0, 0, 0};
	int total_merged_requests[4] = { 0, 0, 0, 0};
	int total_delay[4] = { 0, 0, 0, 0};

	BOOL slot_occupied = TRUE;
	FILE* fp_tmp = NULL;
	FILE* fp_delay = NULL;

	fp_tmp = fopen("served_traffic.dat", "a");
	if (fp_tmp == NULL)
	{
		error("Can not open file to write VNO1 bandwidth map details", stderr);
		exit(1);
	}

	fp_delay = fopen("delayed_traffic.dat", "a");
	if (fp_delay == NULL)
	{
		error("Can not open file to write VNO1 bandwidth map details", stderr);
		exit(1);
	}

	while (slot_occupied == TRUE)
	{
		t_cont = vno1_bwmap.allocated_slots[k].traffic_class;
		total_traffic[t_cont] += vno1_bwmap.allocated_slots[k].grant_size;
		k++;
		slot_occupied = vno1_bwmap.allocated_slots[k].slot_occupied;
	}

	k = 0;
	slot_occupied = TRUE;

	while (slot_occupied == TRUE)
	{
		t_cont = vno2_bwmap.allocated_slots[k].traffic_class;
		total_traffic[t_cont] += vno2_bwmap.allocated_slots[k].grant_size;
		k++;
		slot_occupied = vno2_bwmap.allocated_slots[k].slot_occupied;
	}

	k = 0;
	slot_occupied = TRUE;

	while (slot_occupied == TRUE)
	{
		t_cont = physical_bwmap.allocated_slots[k].traffic_class;
		served_traffic[t_cont] += physical_bwmap.allocated_slots[k].grant_size;
		if (physical_bwmap.allocated_slots[k].delay > 0)
		{
			total_delay[t_cont] += physical_bwmap.allocated_slots[k].delay;
		}
		total_merged_requests[t_cont]++;
		k++;
		slot_occupied = physical_bwmap.allocated_slots[k].slot_occupied;
	}

	for (i = 0; i < 4; i++)
	{
		perc_served = 0;
		total = total_traffic[i];
		served = served_traffic[i];
		if (served > 0)
			perc_served = (served/total) * 100;
		if (total == 0)
			perc_served = 100;
		fprintf(fp_tmp, "%lf ", perc_served);
		served_perc[run][i] = perc_served;

		total = total_delay[i];	
		served = total_merged_requests[i];
		if (served > 0)
			perc_served = total / served;
		if (total == 0)
			perc_served = 0;
		fprintf(fp_delay, "%lf ", perc_served);
		delay_perc[run][i] = perc_served;
	}
	fprintf(fp_tmp, "\n");
	fprintf(fp_delay, "\n");

	fclose(fp_tmp);
	fclose(fp_delay);
}


void reset_everthing()
{
	int k = 0;
	BOOL slot_occupied = TRUE;

	while (slot_occupied == TRUE)
	{
		vno1_bwmap.allocated_slots[k].allocated = FALSE;
		vno1_bwmap.allocated_slots[k].start_time = 0;
		vno1_bwmap.allocated_slots[k].grant_size = 0;
		vno1_bwmap.allocated_slots[k].job_id = 0;
		vno1_bwmap.allocated_slots[k].delay = 0;
		vno1_bwmap.allocated_slots[k].slot_occupied = FALSE;
		k++;
		slot_occupied = vno1_bwmap.allocated_slots[k].slot_occupied;
	}

	k = 0;
	slot_occupied = TRUE;

	while (slot_occupied == TRUE)
	{
		vno2_bwmap.allocated_slots[k].allocated = FALSE;
		vno2_bwmap.allocated_slots[k].start_time = 0;
		vno2_bwmap.allocated_slots[k].grant_size = 0;
		vno2_bwmap.allocated_slots[k].job_id = 0;
		vno2_bwmap.allocated_slots[k].delay = 0;
		vno2_bwmap.allocated_slots[k].slot_occupied = FALSE;
		k++;
		slot_occupied = vno2_bwmap.allocated_slots[k].slot_occupied;
	}

	k = 0;
	slot_occupied = TRUE;

	while (slot_occupied == TRUE)
	{
		physical_bwmap.allocated_slots[k].allocated = FALSE;
		physical_bwmap.allocated_slots[k].start_time = 0;
		physical_bwmap.allocated_slots[k].grant_size = 0;
		physical_bwmap.allocated_slots[k].job_id = 0;
		physical_bwmap.allocated_slots[k].delay = 0;
		physical_bwmap.allocated_slots[k].slot_occupied = FALSE;
		k++;
		slot_occupied = physical_bwmap.allocated_slots[k].slot_occupied;
	}

	for (k = 0; k < 1152; k++)
		allocated_slots[k] = 0;
}


void served_traffic_confidence_interval(int run)
{
	double sum[4] = { 0.0, 0.0, 0.0, 0.0}, mean[4] = { 0.0, 0.0, 0.0, 0.0};
	double conf_diff[4] = { 0.0, 0.0, 0.0, 0.0}, standardDeviation[4] = { 0.0, 0.0, 0.0, 0.0};
	double z = 2.58;
	int i, t_cont = 0;
	FILE* fp_tmp = NULL;
	FILE* fp_tmp1 = NULL;
	FILE* fp_tmp2 = NULL;
	FILE* fp_tmp3 = NULL;
	FILE* fp_tmp4 = NULL;

	fp_tmp = fopen("served_traffic_conf.dat", "a");
	fp_tmp1 = fopen("served_traffic_conf_1.dat", "a");
	fp_tmp2 = fopen("served_traffic_conf_2.dat", "a");
	fp_tmp3 = fopen("served_traffic_conf_3.dat", "a");
	fp_tmp4 = fopen("served_traffic_conf_4.dat", "a");

	for (t_cont = 0; t_cont < 4; t_cont++)
	{
		for (i = 0; i < run; ++i)
		{
			sum[t_cont] += served_perc[i][t_cont];
		}
		mean[t_cont] = sum[t_cont] / run;
	}
	
	for (t_cont = 0; t_cont < 4; t_cont++)
	{
		for (i = 0; i < run; ++i) {
			standardDeviation[t_cont] += pow(served_perc[i][t_cont] - mean[t_cont], 2);
		}
		standardDeviation[t_cont] = sqrt(standardDeviation[t_cont] / run);
		conf_diff[t_cont] = z * (standardDeviation[t_cont] / sqrt(run));
	}

	for (t_cont = 0; t_cont < 4; t_cont++)
	{
		fprintf(fp_tmp, "%lf ", mean[t_cont]);
		fprintf(fp_tmp, "%lf ", conf_diff[t_cont]);
		fprintf(fp_tmp, "\n");
	}
	fprintf(fp_tmp, "\n");

	fprintf(fp_tmp1, "%lf ", mean[0]);
	fprintf(fp_tmp1, "%lf ", conf_diff[0]);
	fprintf(fp_tmp1, "\n");

	fprintf(fp_tmp2, "%lf ", mean[1]);
	fprintf(fp_tmp2, "%lf ", conf_diff[1]);
	fprintf(fp_tmp2, "\n");

	fprintf(fp_tmp3, "%lf ", mean[2]);
	fprintf(fp_tmp3, "%lf ", conf_diff[2]);
	fprintf(fp_tmp3, "\n");

	fprintf(fp_tmp4, "%lf ", mean[3]);
	fprintf(fp_tmp4, "%lf ", conf_diff[3]);
	fprintf(fp_tmp4, "\n");

	fclose(fp_tmp);
	fclose(fp_tmp1);
	fclose(fp_tmp2);
	fclose(fp_tmp3);
	fclose(fp_tmp4);
}


void delay_confidence_interval(int run)
{
	double sum[4] = { 0.0, 0.0, 0.0, 0.0}, mean[4] = { 0.0, 0.0, 0.0, 0.0};
	double conf_diff[4] = { 0.0, 0.0, 0.0, 0.0}, standardDeviation[4] = { 0.0, 0.0, 0.0, 0.0};
	double z = 2.58;
	int i, t_cont = 0;
	FILE* fp_tmp = NULL;
	FILE* fp_tmp1 = NULL;
	FILE* fp_tmp2 = NULL;
	FILE* fp_tmp3 = NULL;
	FILE* fp_tmp4 = NULL;

	fp_tmp = fopen("delayed_traffic_conf.dat", "a");
	fp_tmp1 = fopen("delayed_traffic_conf_1.dat", "a");
	fp_tmp2 = fopen("delayed_traffic_conf_2.dat", "a");
	fp_tmp3 = fopen("delayed_traffic_conf_3.dat", "a");
	fp_tmp4 = fopen("delayed_traffic_conf_4.dat", "a");

	for (t_cont = 0; t_cont < 4; t_cont++)
	{
		for (i = 0; i < run; ++i)
		{
			sum[t_cont] += delay_perc[i][t_cont];
		}
		mean[t_cont] = sum[t_cont] / run;
	}

	for (t_cont = 0; t_cont < 4; t_cont++)
	{
		for (i = 0; i < run; ++i) {
			standardDeviation[t_cont] += pow(delay_perc[i][t_cont] - mean[t_cont], 2);
		}
		standardDeviation[t_cont] = sqrt(standardDeviation[t_cont] / run);
		conf_diff[t_cont] = z * (standardDeviation[t_cont] / sqrt(run));
	}

	for (t_cont = 0; t_cont < 4; t_cont++)
	{
		fprintf(fp_tmp, "%lf ", mean[t_cont]);
		fprintf(fp_tmp, "%lf ", conf_diff[t_cont]);
		fprintf(fp_tmp, "\n");
	}
	fprintf(fp_tmp, "\n");

	fprintf(fp_tmp1, "%lf ", mean[0]);
	fprintf(fp_tmp1, "%lf ", conf_diff[0]);
	fprintf(fp_tmp1, "\n");

	fprintf(fp_tmp2, "%lf ", mean[1]);
	fprintf(fp_tmp2, "%lf ", conf_diff[1]);
	fprintf(fp_tmp2, "\n");

	fprintf(fp_tmp3, "%lf ", mean[2]);
	fprintf(fp_tmp3, "%lf ", conf_diff[2]);
	fprintf(fp_tmp3, "\n");

	fprintf(fp_tmp4, "%lf ", mean[3]);
	fprintf(fp_tmp4, "%lf ", conf_diff[3]);
	fprintf(fp_tmp4, "\n");

	fclose(fp_tmp);
	fclose(fp_tmp1);
	fclose(fp_tmp2);
	fclose(fp_tmp3);
	fclose(fp_tmp4);
}


// code added for OFC'19 work

int serve_previous_frame_requests(int i, int index)
{
	return index;
}


void fama()
{

}
