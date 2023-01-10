// p_cache.c 
#include "Dashnode.h"

#include <ArrayManager.h>
#include <cstdio>
// Static Parameters
  int p_cache::P_delay = 1;
  int p_cache::P_size = 3;
// Class Linker
// User Init
// Lexer Functions

void p_cache::getca_line_structFromType(Type* t,ca_line_struct* pDest)
{
  (*pDest).ca_valid = (int)((Int*)((Struct*)t)->getField(0))->intValue();
  (*pDest).ca_mod = (int)((Int*)((Struct*)t)->getField(1))->intValue();
  (*pDest).ca_share = (int)((Int*)((Struct*)t)->getField(2))->intValue();
  (*pDest).ca_tag = (int)((Int*)((Struct*)t)->getField(3))->intValue();
  (*pDest).ca_cm_block = (int)((Int*)((Struct*)t)->getField(4))->intValue();
  (*pDest).ca_add1 = (int)((Int*)((Struct*)t)->getField(5))->intValue();
  (*pDest).ca_add2 = (int)((Int*)((Struct*)t)->getField(6))->intValue();
  (*pDest).ca_add3 = (int)((Int*)((Struct*)t)->getField(7))->intValue();
  (*pDest).ca_add4 = (int)((Int*)((Struct*)t)->getField(8))->intValue();
}
// --------------------------------------------------------
// p_cache Constructor
// --------------------------------------------------------
p_cache::p_cache(const char* n , char status0_i, char status1_i, int cache_size_i) :
  sim_entity(n),
  from_mips(n),
  to_mips(n),
  to_s_cache(n),
  from_s_cache(n),
  status0(status0_i),
  status1(status1_i),
 cache_size(cache_size_i),
  cache(cache_size_i,"cache")
{
    cache.m_pParentEntity = this;

  join_port(from_mips,"from_mips");
  join_port(to_mips,"to_mips");
  join_port(to_s_cache,"to_s_cache");
  join_port(from_s_cache,"from_s_cache");
	// --- Read array cache into memory
	cacheLength = 0;
  Type * __ca_line_structType[9];
  Int __int(0);  __ca_line_structType[0]=&__int;
  __ca_line_structType[1]=&__int;
  __ca_line_structType[2]=&__int;
  __ca_line_structType[3]=&__int;
  __ca_line_structType[4]=&__int;
  __ca_line_structType[5]=&__int;
  __ca_line_structType[6]=&__int;
  __ca_line_structType[7]=&__int;
  __ca_line_structType[8]=&__int;
  Struct __ca_line_struct(__ca_line_structType, 9);
  Array __cacheArray("cache", 256, __ca_line_struct);
	int __cacheTotal = readMemory(get_name(), __cacheArray);
	if ( __cacheTotal == -1)
		printf("No memory file found for %s.cache.\n", get_name());
	else if ( __cacheTotal < 0)
		printf("%s:Error reading memory cache, line %d.\n", get_name(), -(__cacheTotal + 2));
	else {
		cache.section = __cacheArray.getNoSection();
		for( int __i=0; __i<__cacheTotal; __i++) 
       getca_line_structFromType(__cacheArray.get(__i), &cache[__i]);
    cacheLength = __cacheTotal;
    printf("%s:%d items read in memory cache.\n", get_name(), cacheLength);
  }

  my_state = 0;
}

char p_cache::P_HIT=0;
char p_cache::P_MISS=1;
char p_cache::P_IDLE=2;
const char* p_cache::state_names[] = {"P_HIT ","P_MISS ","P_IDLE "};
// --------------------------------------------------------
// p_cache Dump State general - used for writing parameter values to trace file
// -------------------------------------------------------- 
void p_cache::dumpState(int stage, char type)
{
  char buf[255];
  sprintf( buf," 0 ");
  strcat (buf, state_names[my_state]);
  PutValue( buf, status0 );
  PutValue( buf, status1 );
  strcat( buf, "\n" );
  sim_trace(type, stage, 5, buf);
}

/* -------------------------------------------------------- */
/* p_cache Dump State function - used for writing parameter values to trace file */
/* -------------------------------------------------------- */
void p_cache::dump_state(int stage)
{
	dumpState(stage, 'P');
}

/* -------------------------------------------------------- */
/* p_cache Dump State Immediate function - used for writing parameter values to trace file to be animated immediately instead of at the end of the step */
/* -------------------------------------------------------- */
void p_cache::dump_state_i(int stage)
{
	dumpState(stage, 'I');
}

// --------------------------------------------------------
// p_cache Startup code (copied from .hase/.sim file)
// --------------------------------------------------------
void p_cache::startup()
{
  sim_event ev;

}
// -------------------------------------------------------- 
// p_cache Report code (copied from .hase/.sim file)
// -------------------------------------------------------- 
void p_cache::report()
{
  sim_event ev;

}
  
  void p_cache::do_mips_event(sim_event &ev){
  
  SIM_CAST(p1_struct, pkt_from_mips, ev);
  add_req=pkt_from_mips.p1_address;
  
  /* calculate where address lies in clus memory and cache */
  
  cm_block=(add_req/4);
  slot = cm_block % p_cache_size;
  tag = (add_req / 4) / p_cache_size;
  
  /* classify request as read or write */
  
  action = (char) pkt_from_mips.p1_rw[0];
  
  if (action=='r'){   // read cycle
  
  // check for validity / tag match
  current_line=cache[slot];
  
  if (current_line.ca_valid==0){
  
  /* invalid cache line therefore refer */
  refer=1;
  status0='R';
  status1='M';
  my_state=P_MISS;
  dump_state();
  }else{
  /* valid cache line now check tag */
  if (current_line.ca_tag!=tag){
  
  /*tag mismatch - refer */
  refer=1;
  status0='R';
  status1='M';
  my_state=P_MISS;
  dump_state();
  }else{
  /* hit ! */
  // do nothing - return packet later
  status0='R';
  status1='H';
  my_state=P_HIT;
  dump_state();
  }
  }
  }else{
  
  /* write cycle */
  add_req=pkt_from_mips.p1_address;
  cm_block=(add_req/4);
  slot = cm_block % p_cache_size;
  tag = (add_req / 4) / p_cache_size;
  current_line=cache[slot];
  
  if (current_line.ca_valid==0){
  /* invalid cache line - therefore refer to 2nd cache */
  refer=1;
  status0='W';
  status1='M';
  my_state=P_MISS;
  dump_state();
  }else{
  
  /* valid cache line - check tag. */
  if (current_line.ca_tag!=tag){
  
  /* tag mismatch - refer to 2nd cache */
  refer=1;
  status0='W';
  status1='M';
  my_state=P_MISS;
  dump_state();
  }else{
  /* hit! */
  /* we shall make the write-through below */
  status0='W';
  status1='H';
  my_state=P_HIT;
  dump_state();
  }
  }
  }
  
  /* now simulate cache delay and send approp. pkts */
  
  if (action=='r'){
  
  sim_hold(P_delay);
  my_state=P_IDLE;
  dump_state();
  
  /* is there referal to second cache? */
  
  if (refer != 0){
  pkt_to_s_cache = pkt_from_mips;
  send_DATAPKT(to_s_cache,pkt_to_s_cache);
  }else{
  /* send back a hit! */
  pkt_to_mips = pkt_from_mips;
  send_DATAPKT(to_mips,pkt_to_mips);
  status0='_';
  status1='_';
  dump_state();
  }
  }else{
  /* action = write */
  sim_hold(P_delay);
  my_state=P_IDLE;
  dump_state();
  if (refer != 0){
  
  /* write miss -  note in fuller  implementation send */
  /* misses to write buffer and allow mips to overcome */
  /* write-stall                                       */
  
  pkt_to_s_cache = pkt_from_mips;
  pkt_to_s_cache.p1_rw[1]='M';
  send_DATAPKT(to_s_cache,pkt_to_s_cache);
  }else{
  /* write hit */
  pkt_to_s_cache = pkt_from_mips;
  send_DATAPKT(to_s_cache,pkt_to_s_cache);
  
  pkt_to_mips=pkt_from_mips;
  
  pkt_to_mips.p1_rw[0]='W';
  pkt_to_mips.p1_rw[1]='A';
  pkt_to_mips.p1_rw[2]='c';
  pkt_to_mips.p1_rw[3]='k';
  //	    send_DATAPKT(to_mips,pkt_to_mips);
  // shouldn't send this packet back to MIPS until ack of write through
  // from S cache. Modified by rni, 19-03-2003.
  }
  }
  
  }
  
  void p_cache::do_s_cache_event(sim_event &ev){
  
  SIM_CAST(p1_struct,pkt_from_s_cache,ev);
  
  /* this event updates the cache with pkts from s_cache */
  
  if ((pkt_from_s_cache.p1_rw[0]=='W') || (pkt_from_s_cache.p1_rw[0]=='w'))
  
  // Altered to ensure policy is "no write allocate" in Primary Cache.
  // Returning packet from s_cache can be an ack (Wack) or miss (wM).
  // Previously one allocated, the other didn't!  (rni 22/3/2002)
  
  {
  status0='_';
  status1='_';
  dump_state();
  sim_hold(P_delay);
  my_state=P_IDLE;
  pkt_to_mips = pkt_from_s_cache;
  send_DATAPKT(to_mips,pkt_to_mips);
  }else{
  
  add_req = pkt_from_s_cache.p1_address;
  cm_block=(add_req/4);
  slot = cm_block % p_cache_size;
  tag = (add_req / 4) / p_cache_size;
  
  /* reset state display */
  
  status0='_';
  status1='_';
  
  current_line.ca_valid=1;
  current_line.ca_tag=tag;
  current_line.ca_cm_block=cm_block;
  current_line.ca_add1=(4*cm_block);
  current_line.ca_add2=(4*cm_block)+1;
  current_line.ca_add3=(4*cm_block)+2;
  current_line.ca_add4=(4*cm_block)+3;
  current_line.ca_mod=0;
  
  //cache[slot]=current_line;
  cache.Update(slot,current_line);
  
  /* Simulate processing delay */
  
  sim_hold(P_delay);
  my_state=P_IDLE;
  dump_state_i();
  pkt_to_mips = pkt_from_s_cache;
  send_DATAPKT(to_mips,pkt_to_mips);
  }
  }
  
  
/* -------------------------------------------------------- */
/* p_cache Body code (copied from .hase/.sim file) */
/* -------------------------------------------------------- */
void p_cache::body()
{
  sim_event ev;
  
  p_cache_size = 1 << P_size;
  clus_mem_size = 1 << Memory::M_size;
  
  /* calculate memory range of tag */
  
  tag_boundary = (clus_mem_size / (p_cache_size*4));
  
  /* Initialise the cache - set the state display */
  /* and read in the cache meory file from disk.   */
  
  current_line.ca_valid=0;
  current_line.ca_mod=0;
  current_line.ca_share=0;
  current_line.ca_tag=0;
  current_line.ca_cm_block=0;
  current_line.ca_add1=0;
  current_line.ca_add2=0;
  current_line.ca_add3=0;
  current_line.ca_add4=0;
  
  for (count=0;count<p_cache_size;count++){
  cache.Update(count,current_line);
  }
  
  status0='_';
  status1='_';
  
  my_state=P_IDLE;
  dump_state();
  
  
  while(1)
  {
  sim_get_next(ev);
  refer=0;
  
  /* Classify Event */
  
  if  (ev.scheduled_by() == from_mips.get_dest()){
  do_mips_event(ev);                // Deal with mips event
  }else{
  do_s_cache_event(ev);             // Deal with s_cache_event
  }
  }

}
