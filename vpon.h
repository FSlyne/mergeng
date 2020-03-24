#ifndef _VPON_
#define _VPON_

#define MAX_NUM_VNOs 3
#define TOTAL_NUM_ONUs 12
#define TOTAL_PON_NUM_SLOTS 300

/*----------------------------------------------------------------------------*
							Structure of VNO
*----------------------------------------------------------------------------*/

typedef struct _vno
{
	NODEID vno_id;					/* Identification number of ONU		*/
	BOOL vno_state;					/* ONU enable or disable			*/
	COST path_weight;				/* Distance from OLT				*/
	TRAFFIC traffic_flow;			/* Amount of requested traffic		*/
//	BW_MAP bandwidth_map;			/* Bandwidth map of this VNO		*/
//	ONU *onu_list;					/* ONUs linked with this VNO		*/
}
VNO;


#define vno_id(vno) (vno->vno_id)
#define vno_state(vno) (vno->vno_state)
#define onu_vno(vno,i) (vno->onu_list[i])


/*----------------------------------------------------------------------------*
							Structure of PON_Upstream_Frame
*----------------------------------------------------------------------------*/

typedef struct _slot_random_frame
{
	ONUID onu_id;
	int job_id;
	BOOL slot_occupied;
	BOOL allocated;
	int traffic_class;
	int start_time;
	int grant_size;
	int delay;
}
SLOT_Random_FRAME;


/*----------------------------------------------------------------------------*
							Structure of Random BandWidth(BW) MAP
*----------------------------------------------------------------------------*/

typedef struct _bwmap_random
{
	VNOID vno_id;								/* Identification number of VNO					*/
	SLOT_Random_FRAME allocated_slots[1000];	/* Slots allocated in this BWMap by this VNO	*/
}
BW_MAP_Random;


#endif
