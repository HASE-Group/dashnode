// mp_bus.c 
#include "Dashnode.h"

#include <ArrayManager.h>
#include <cstdio>
// --------------------------------------------------------
// mp_bus Constructor
// --------------------------------------------------------
mp_bus::mp_bus(const char* n ) :
  sim_entity(n),
  from_mips1(n),
  to_mips1(n),
  to_c_memory(n),
  from_c_memory(n)
{

  join_port(from_mips1,"from_mips1");
  join_port(to_mips1,"to_mips1");
  join_port(to_c_memory,"to_c_memory");
  join_port(from_c_memory,"from_c_memory");
}

// --------------------------------------------------------
// mp_bus Dump State general - used for writing parameter values to trace file
// -------------------------------------------------------- 
void mp_bus::dumpState(int stage, char type)
{
  char buf[255], tmp[80];
  int ind;
  sprintf( buf," 0 ");
  strcat( buf, "\n" );
  sim_trace(type, stage, 5, buf);
}

/* -------------------------------------------------------- */
/* mp_bus Dump State function - used for writing parameter values to trace file */
/* -------------------------------------------------------- */
void mp_bus::dump_state(int stage)
{
	dumpState(stage, 'P');
}

/* -------------------------------------------------------- */
/* mp_bus Dump State Immediate function - used for writing parameter values to trace file to be animated immediately instead of at the end of the step */
/* -------------------------------------------------------- */
void mp_bus::dump_state_i(int stage)
{
	dumpState(stage, 'I');
}

// --------------------------------------------------------
// mp_bus Startup code (copied from .hase/.sim file)
// --------------------------------------------------------
void mp_bus::startup()
{
  sim_event ev;

}
// -------------------------------------------------------- 
// mp_bus Report code (copied from .hase/.sim file)
// -------------------------------------------------------- 
void mp_bus::report()
{
  sim_event ev;

}
/* -------------------------------------------------------- */
/* mp_bus Body code (copied from .hase/.sim file) */
/* -------------------------------------------------------- */
void mp_bus::body()
{
  sim_event ev;

while(1)
{	
	sim_get_next(ev);

	/* Classify Event */

        if  (ev.scheduled_by() == from_mips1.get_dest()){
		SIM_CAST(p1_struct,pkt_from_mips1,ev);
		sim_hold(1.0);
		
		pkt_to_c_memory.p1_address = pkt_from_mips1.p1_address;
        	strcpy(pkt_to_c_memory.p1_rw,pkt_from_mips1.p1_rw);
		strcpy(pkt_to_c_memory.p1_id,pkt_from_mips1.p1_id);

	send_DATAPKT(to_c_memory,pkt_to_c_memory);

	}	

        if  (ev.scheduled_by() == from_c_memory.get_dest()){
		SIM_CAST(p1_struct,pkt_from_c_memory,ev);
		sim_hold(1.0);
		
		pkt_to_mips1.p1_address = pkt_from_c_memory.p1_address;
        	strcpy(pkt_to_mips1.p1_rw,pkt_from_c_memory.p1_rw);
		strcpy(pkt_to_mips1.p1_id,pkt_from_c_memory.p1_id);

	send_DATAPKT(to_mips1,pkt_to_mips1);

	}

}

}
