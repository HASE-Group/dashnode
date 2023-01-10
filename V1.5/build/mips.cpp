// mips.c 
#include "Dashnode.h"

#include <ArrayManager.h>
#include <cstdio>
// Static Parameters
  int mips::MIPS_delay = 2;
// Class Linker
// User Init
// Lexer Functions

void mips::getmem_trace_structFromType(Type* t,mem_trace_struct* pDest)
{
  (*pDest).mt_address = (int)((Int*)((Struct*)t)->getField(0))->intValue();
 strcpy((*pDest).mt_rw,((String*)((Struct*)t)->getField(1))->toString());
 strcpy((*pDest).mt_id,((String*)((Struct*)t)->getField(2))->toString());
}
// --------------------------------------------------------
// mips Constructor
// --------------------------------------------------------
mips::mips(const char* n , int mem_trace_size_i, int TC_i) :
  sim_entity(n),
  to_p_cache(n),
  from_p_cache(n),
 mem_trace_size(mem_trace_size_i),
  mem_trace(mem_trace_size_i,"mem_trace"),
  TC(TC_i)
{
    mem_trace.m_pParentEntity = this;

  join_port(to_p_cache,"to_p_cache");
  join_port(from_p_cache,"from_p_cache");
	// --- Read array mem_trace into memory
	mem_traceLength = 0;
  Type * __mem_trace_structType[3];
  Int __int(0);  __mem_trace_structType[0]=&__int;
 String __char("(undefined)", ' ');
  String __charLine("(undefined)");
  __mem_trace_structType[1]=&__char;
  __mem_trace_structType[2]=&__char;
  Struct __mem_trace_struct(__mem_trace_structType, 3);
  Array __mem_traceArray("mem_trace", 65536, __mem_trace_struct);
	int __mem_traceTotal = readMemory(get_name(), __mem_traceArray);
	if ( __mem_traceTotal == -1)
		printf("No memory file found for %s.mem_trace.\n", get_name());
	else if ( __mem_traceTotal < 0)
		printf("%s:Error reading memory mem_trace, line %d.\n", get_name(), -(__mem_traceTotal + 2));
	else {
		mem_trace.section = __mem_traceArray.getNoSection();
		for( int __i=0; __i<__mem_traceTotal; __i++) 
       getmem_trace_structFromType(__mem_traceArray.get(__i), &mem_trace[__i]);
    mem_traceLength = __mem_traceTotal;
    printf("%s:%d items read in memory mem_trace.\n", get_name(), mem_traceLength);
  }

  my_state = 0;
}

char mips::M_STOPPED=0;
char mips::M_RUNNING=1;
char mips::M_WAITING=2;
const char* mips::state_names[] = {"M_STOPPED ","M_RUNNING ","M_WAITING "};
// --------------------------------------------------------
// mips Dump State general - used for writing parameter values to trace file
// -------------------------------------------------------- 
void mips::dumpState(int stage, char type)
{
  char buf[255];
  sprintf( buf," 0 ");
  strcat (buf, state_names[my_state]);
  sprintf(buf+strlen(buf), "%d ", TC);
  strcat( buf, "\n" );
  sim_trace(type, stage, 5, buf);
}

/* -------------------------------------------------------- */
/* mips Dump State function - used for writing parameter values to trace file */
/* -------------------------------------------------------- */
void mips::dump_state(int stage)
{
	dumpState(stage, 'P');
}

/* -------------------------------------------------------- */
/* mips Dump State Immediate function - used for writing parameter values to trace file to be animated immediately instead of at the end of the step */
/* -------------------------------------------------------- */
void mips::dump_state_i(int stage)
{
	dumpState(stage, 'I');
}

// --------------------------------------------------------
// mips Startup code (copied from .hase/.sim file)
// --------------------------------------------------------
void mips::startup()
{
  sim_event ev;

}
// -------------------------------------------------------- 
// mips Report code (copied from .hase/.sim file)
// -------------------------------------------------------- 
void mips::report()
{
  sim_event ev;
  
  if(error)
  {printf("mem_trace %d contains an invalid memory address (%d)\n", TC, pkt_to_p_cache.p1_address);}

}
/* -------------------------------------------------------- */
/* mips Body code (copied from .hase/.sim file) */
/* -------------------------------------------------------- */
void mips::body()
{
  sim_event ev;
  
  // Initialise variables
  
  stop = false;
  TC = 0;
  myTimer = (Timer*)sim.get_entity(sim.get_entity_id("TIMER"));
  max_address =  1 << Memory::M_size;
  
  // Generate addresses, from mem_trace file.
  
  error = false;
  action = 'r';
  my_state=M_WAITING;
  
  while ((action != 'z') && (!error) && (!myTimer->stop))
  {
  // update state info
  
  dump_state();
  
  // model processor delay
  
  sim_hold(MIPS_delay);
  
  // load next line of trace file in
  
  trace_file_line = mem_trace[TC];
  action = (char) trace_file_line.mt_rw[0];
  
  // set up and send the request for the address found
  // in the trace file line to the primary cache!
  
  pkt_to_p_cache.p1_address = trace_file_line.mt_address;
  strcpy(pkt_to_p_cache.p1_rw,trace_file_line.mt_rw);
  strcpy(pkt_to_p_cache.p1_id,trace_file_line.mt_id);
  
  // check address for NOP value of -1     (no longer used)
  // used to skip through input trace file
  
  if (pkt_to_p_cache.p1_address > max_address
  || pkt_to_p_cache.p1_address < 0)
  {
  error = true;
  }
  
  else if ((pkt_to_p_cache.p1_address!=-1) && (action != 'z'))
  {
  send_DATAPKT(to_p_cache,pkt_to_p_cache);
  TC ++;
  my_state=M_WAITING;
  dump_state();
  
  
  sim_wait(ev);
  }
  my_state=M_RUNNING;
  dump_state();
  }
  stop = true;
  my_state=M_STOPPED;
  dump_state();
  

}
