// Timer.c 
#include "Dashnode.h"

#include <ArrayManager.h>
#include <cstdio>
  int Timer::Max_cycles = 7;
// --------------------------------------------------------
// Timer Constructor
// --------------------------------------------------------
Timer::Timer(const char* n , int Cycles_i) :
  sim_entity(n),
  Cycles(Cycles_i)
{

  my_state = 0;
}

char Timer::NORMAL=0;
const char* Timer::state_names[] = {"NORMAL "};
// --------------------------------------------------------
// Timer Dump State general - used for writing parameter values to trace file
// -------------------------------------------------------- 
void Timer::dumpState(int stage, char type)
{
  char buf[255], tmp[80];
  int ind;
  sprintf( buf," 0 ");
  strcat (buf, state_names[my_state]);
  sprintf(buf+strlen(buf), "%d ", Cycles);
  strcat( buf, "\n" );
  sim_trace(type, stage, 5, buf);
}

/* -------------------------------------------------------- */
/* Timer Dump State function - used for writing parameter values to trace file */
/* -------------------------------------------------------- */
void Timer::dump_state(int stage)
{
	dumpState(stage, 'P');
}

/* -------------------------------------------------------- */
/* Timer Dump State Immediate function - used for writing parameter values to trace file to be animated immediately instead of at the end of the step */
/* -------------------------------------------------------- */
void Timer::dump_state_i(int stage)
{
	dumpState(stage, 'I');
}

// --------------------------------------------------------
// Timer Startup code (copied from .hase/.sim file)
// --------------------------------------------------------
void Timer::startup()
{
  sim_event ev;

}
// -------------------------------------------------------- 
// Timer Report code (copied from .hase/.sim file)
// -------------------------------------------------------- 
void Timer::report()
{
  sim_event ev;

}
/* -------------------------------------------------------- */
/* Timer Body code (copied from .hase/.sim file) */
/* -------------------------------------------------------- */
void Timer::body()
{
  sim_event ev;

  myMIPS = (mips*)sim.get_entity(sim.get_entity_id("MIPS"));
  max_cycles = 1 << Max_cycles;
  Cycles = 0;
  stop = false;
 
dump_state();

while (!myMIPS->stop && (Cycles < max_cycles))
{
 sim_hold(1);
 Cycles ++;
 dump_state();
}

stop = true;


}
