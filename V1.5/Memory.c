// Memory.c 
#include "Dashnode.h"

#include <ArrayManager.h>
  int Memory::M_delay = 4;
  int Memory::M_size = 10;
// --------------------------------------------------------
// Memory Constructor
// --------------------------------------------------------
Memory::Memory(char*n , char action_i) :
  sim_entity(n),
  from_mp_bus(n),
  to_mp_bus(n),
  action(action_i)
{
  join_port(from_mp_bus,"from_mp_bus");
  join_port(to_mp_bus,"to_mp_bus");
}

// --------------------------------------------------------
// Memory Dump State general - used for writing parameter values to trace file
// -------------------------------------------------------- 
void Memory::dumpState(int stage, char type)
{
  char buf[255], tmp[80];
  int ind;
  sprintf( buf," 0 ");
  PutValue( buf, action );
  strcat( buf, "\n" );
  sim_trace(type, stage, 5, buf);
}

/* -------------------------------------------------------- */
/* Memory Dump State function - used for writing parameter values to trace file */
/* -------------------------------------------------------- */
void Memory::dump_state(int stage)
{
	dumpState(stage, 'P');
}

/* -------------------------------------------------------- */
/* Memory Dump State Immediate function - used for writing parameter values to trace file to be animated immediately instead of at the end of the step */
/* -------------------------------------------------------- */
void Memory::dump_state_i(int stage)
{
	dumpState(stage, 'I');
}

// --------------------------------------------------------
// Memory Startup code (copied from .hase/.sim file)
// --------------------------------------------------------
void Memory::startup()
{
  sim_event ev;

}
// -------------------------------------------------------- 
// Memory Report code (copied from .hase/.sim file)
// -------------------------------------------------------- 
void Memory::report()
{
  sim_event ev;

}
/* -------------------------------------------------------- */
/* Memory Body code (copied from .hase/.sim file) */
/* -------------------------------------------------------- */
void Memory::body()
{
  sim_event ev;

 action = '_';
 dump_state();

while(1)
{
	sim_get_next(ev);

	SIM_CAST(p1_struct,pkt_from_mp_bus,ev);
	if (pkt_from_mp_bus.p1_rw[0] == 'r')
	 {action = 'R';}
	else if (pkt_from_mp_bus.p1_rw[0] == 'w')
	 {action = 'W';}
	else if (pkt_from_mp_bus.p1_rw[0] == 'u')
	 {action = 'U';}
	dump_state();

	sim_hold(M_delay);
	action = '_';
	dump_state();

	// clause below only sends back write packets if allocating
	// in 2nd cache level.

	if (pkt_from_mp_bus.p1_rw[0] != 'u'){
		
		pkt_to_mp_bus.p1_address = pkt_from_mp_bus.p1_address;
        	strcpy(pkt_to_mp_bus.p1_rw,pkt_from_mp_bus.p1_rw);
		strcpy(pkt_to_mp_bus.p1_id,pkt_from_mp_bus.p1_id);

		send_DATAPKT(to_mp_bus,pkt_to_mp_bus);
	}
}

}
