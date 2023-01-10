// s_cache.c 
#include "Dashnode.h"

#include <ArrayManager.h>
#include <cstdio>
// Static Parameters
  int s_cache::S_delay = 2;
  int s_cache::S_size = 4;
// Class Linker
// User Init
// Lexer Functions

void s_cache::getca_line_structFromType(Type* t,ca_line_struct* pDest)
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
// s_cache Constructor
// --------------------------------------------------------
s_cache::s_cache(const char* n , char status0_i, char status1_i, int cache_size_i) :
  sim_entity(n),
  from_p_cache(n),
  to_p_cache(n),
  to_mp_bus(n),
  from_mp_bus(n),
  status0(status0_i),
  status1(status1_i),
 cache_size(cache_size_i),
  cache(cache_size_i,"cache")
{
    cache.m_pParentEntity = this;

  join_port(from_p_cache,"from_p_cache");
  join_port(to_p_cache,"to_p_cache");
  join_port(to_mp_bus,"to_mp_bus");
  join_port(from_mp_bus,"from_mp_bus");
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
  Array __cacheArray("cache", 1024, __ca_line_struct);
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

char s_cache::S_HIT=0;
char s_cache::S_MISS=1;
char s_cache::S_IDLE=2;
const char* s_cache::state_names[] = {"S_HIT ","S_MISS ","S_IDLE "};
// --------------------------------------------------------
// s_cache Dump State general - used for writing parameter values to trace file
// -------------------------------------------------------- 
void s_cache::dumpState(int stage, char type)
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
/* s_cache Dump State function - used for writing parameter values to trace file */
/* -------------------------------------------------------- */
void s_cache::dump_state(int stage)
{
	dumpState(stage, 'P');
}

/* -------------------------------------------------------- */
/* s_cache Dump State Immediate function - used for writing parameter values to trace file to be animated immediately instead of at the end of the step */
/* -------------------------------------------------------- */
void s_cache::dump_state_i(int stage)
{
	dumpState(stage, 'I');
}

// --------------------------------------------------------
// s_cache Startup code (copied from .hase/.sim file)
// --------------------------------------------------------
void s_cache::startup()
{
  sim_event ev;

}
// -------------------------------------------------------- 
// s_cache Report code (copied from .hase/.sim file)
// -------------------------------------------------------- 
void s_cache::report()
{
  sim_event ev;

}
  
  void s_cache::do_mpbus_event(sim_event &ev){
  
  SIM_CAST(p1_struct,pkt_from_mp_bus,ev);
  
  /* this event is simply updating the cache with     */
  /* a value from cluster memory ...                  */
  /* NOTE as this cache is write-back we should check */
  /* if replaced lines have been modified, if so we   */
  /* need to 'write-back'!                            */
  
  add_req=pkt_from_mp_bus.p1_address;
  cm_block=(add_req/4);
  slot = cm_block % s_cache_size;
  tag = (add_req / 4) / s_cache_size;
  
  // simulate processing delay
  
  status0='_';
  status1='_';
  
  dump_state();
  
  
  
  if (cache[slot].ca_mod==1){ // This line has been modified
  // therefore do write-back.
  
  pkt_to_mp_bus.p1_address = cache[slot].ca_add1;
  pkt_to_mp_bus.p1_rw[0]   = 'u'; // write-update mode.
  pkt_to_mp_bus.p1_id[0]   = 'd';  // must be data!
  
  send_DATAPKT(to_mp_bus,pkt_to_mp_bus);
  }
  
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
  dump_state();
  sim_hold(S_delay);
  pkt_to_p_cache.p1_address = pkt_from_mp_bus.p1_address;
  strcpy(pkt_to_p_cache.p1_rw,pkt_from_mp_bus.p1_rw);
  strcpy(pkt_to_p_cache.p1_id,pkt_from_mp_bus.p1_id);
  
  send_DATAPKT(to_p_cache,pkt_to_p_cache);
  }
  
  
  /*************************/
  /* Handle p_cache events */
  /*************************/
  
  
  void s_cache::do_p_cache_event(sim_event &ev){
  
  SIM_CAST(p1_struct,pkt_from_p_cache,ev);
  
  add_req=pkt_from_p_cache.p1_address;
  
  /* Given requested address find out:            */
  /*	- its cluster memory block   (cm_block) */
  /*	- its cache slot             (slot)     */
  /*	- its cache tag              (tag)      */
  
  cm_block=(add_req/4);
  slot = cm_block % s_cache_size;
  tag = (add_req / 4) / s_cache_size;
  
  /* Now classify the action read/write */
  
  action = (char) pkt_from_p_cache.p1_rw[0];
  if (action=='r') {
  
  /* Read cycle - check for existing cache entry */
  /* check for validity and tag match            */
  
  current_line=cache[slot];
  if (current_line.ca_valid==0){
  
  /*invalid cache line therefore fetch.. */
  refer=1;
  status0='R';
  status1='M';
  my_state=S_MISS;
  dump_state();
  }else{
  
  /*valid cache line - now check tag */
  if (current_line.ca_tag!=tag){
  
  /* tag mismatch - refer */
  refer=1;
  status0='R';
  status1='M';
  my_state=S_MISS;
  dump_state();
  }else{
  /* Hit!!*/
  // do nothing. return pkt later.
  status0='R';
  status1='H';
  my_state=S_HIT;
  dump_state();
  }
  }
  }else {
  
  /* write cycle - write back cache */
  /* check whether write hit/miss   */
  
  add_req=pkt_from_p_cache.p1_address;
  cm_block=(add_req/4);
  slot = cm_block % s_cache_size;
  tag = (add_req / 4) / s_cache_size;
  current_line=cache[slot];
  if (current_line.ca_valid==0){
  
  /*invalid cache line therefore fetch.. */
  refer=1;
  status0='W';
  status1='M';
  my_state=S_MISS;
  dump_state();
  }else{
  /*valid cache line - now check tag */
  if (current_line.ca_tag!=tag){
  
  /* tag mismatch - refer */
  refer=1;
  status0='W';
  status1='M';
  my_state=S_MISS;
  dump_state();
  }else{
  /* Hit!!*/
  // note modification.
  status0='W';
  status1='H';
  my_state=S_HIT;
  dump_state();
  current_line.ca_mod=1;
  // cache[slot]=current_line;
  cache.Update(slot,current_line);
  }
  }
  }
  
  /* now simulate cache delay */
  
  if (action=='r'){
  sim_hold(S_delay);
  my_state=S_IDLE;
  dump_state();
  
  /* is there a referal to cluster memory? */
  
  if (refer != 0){
  pkt_to_mp_bus.p1_address = pkt_from_p_cache.p1_address;
  strcpy(pkt_to_mp_bus.p1_rw,pkt_from_p_cache.p1_rw);
  strcpy(pkt_to_mp_bus.p1_id,pkt_from_p_cache.p1_id);
  
  send_DATAPKT(to_mp_bus,pkt_to_mp_bus);
  }else{
  /* send back hit */
  
  pkt_to_p_cache.p1_address = pkt_from_p_cache.p1_address;
  strcpy(pkt_to_p_cache.p1_rw,pkt_from_p_cache.p1_rw);
  strcpy(pkt_to_p_cache.p1_id,pkt_from_p_cache.p1_id);
  
  send_DATAPKT(to_p_cache,pkt_to_p_cache);
  status0='_';
  status1='_';
  dump_state();
  }
  
  }else{  // write mode
  
  sim_hold(S_delay);
  my_state=S_IDLE;
  dump_state();
  
  if (refer != 0){
  pkt_to_mp_bus.p1_address = pkt_from_p_cache.p1_address;
  strcpy(pkt_to_mp_bus.p1_rw,pkt_from_p_cache.p1_rw);
  strcpy(pkt_to_mp_bus.p1_id,pkt_from_p_cache.p1_id);
  
  send_DATAPKT(to_mp_bus,pkt_to_mp_bus);
  }else{
  // modified by rni, 19-03-2003 to send Wack on a hit or a miss
  //if (pkt_from_p_cache.p1_rw[1]=='M')
  //{
  pkt_from_p_cache.p1_rw[1]=' ';
  pkt_to_p_cache=pkt_from_p_cache;
  pkt_to_p_cache.p1_rw[0]='W';
  pkt_to_p_cache.p1_rw[1]='A';
  pkt_to_p_cache.p1_rw[2]='c';
  pkt_to_p_cache.p1_rw[3]='k';
  send_DATAPKT(to_p_cache,pkt_to_p_cache);
  status0='_';
  status1='_';
  dump_state();
  //}
  }
  
  }
  }
  
/* -------------------------------------------------------- */
/* s_cache Body code (copied from .hase/.sim file) */
/* -------------------------------------------------------- */
void s_cache::body()
{
  sim_event ev;
  
  s_cache_size  = 1 << S_size;
  clus_mem_size = 1 << Memory::M_size;
  
  /* Caclulate cache tag range */
  
  tag_boundary = (clus_mem_size / (s_cache_size*4));
  current_line.ca_valid=0;
  current_line.ca_tag=0;
  current_line.ca_cm_block=0;
  current_line.ca_add1=0;
  current_line.ca_add2=0;
  current_line.ca_add3=0;
  current_line.ca_add4=0;
  current_line.ca_mod=0;
  current_line.ca_share=0;
  
  for (count=0;count<s_cache_size;count++){
  cache.Update(count,current_line);
  }
  
  status0='_';
  status1='_';
  
  my_state=S_IDLE;
  dump_state();
  while(1){
  
  /* Point of reference to event list for this entity */
  
  sim_get_next(ev);
  refer=0;
  
  /* Classify Event */
  
  if (ev.scheduled_by()==from_p_cache.get_dest()){
  do_p_cache_event(ev);          // Deal with P_cache events.
  
  }else{
  do_mpbus_event(ev);            // Deal with MPbus events.
  }
  }
  
  
  
  
  
  
  
  
  
  
  

}
