// Dashnode.h - HASE++ Header file

#ifndef  PROJECT_HEADER_FILE
#define PROJECT_HEADER_FILE

#include <Array.h>
#include <ArrayLabel.h>
#include <simkernel.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define GET_NEXT(ev) if (sim_waiting(SIM_ANY)>0) sim_select(SIM_ANY,ev); else sim_wait(ev);
#define MEM_UPDATE(mem,index,data,stage) \
 {char buff[80]; sprintf(buff," 17 %s %d ",mem, index); PutValue(buff,data); strcat(buff,"\n"); m_pParentEntity->sim_trace('U', stage, 3,buff );}
#define MEM_READ(mem,index,stage) \
 {char buff[80]; sprintf(buff," 17 %s %d %d\n",mem,index, section);   m_pParentEntity->sim_trace( 'R', stage, 3,buff );}



 extern sim_system sim;
// Event Type Declarations
const int	DATAPKT = 0;

// Structure declarations for messages
class p1_struct {
public:
  int p1_address;
  char p1_rw[100];
  char p1_id[100];
	p1_struct();
    p1_struct& operator=(p1_struct&);
    p1_struct& operator=(p1_struct*);
    ~p1_struct();
};

class mem_trace_struct {
public:
  int mt_address;
  char mt_rw[100];
  char mt_id[100];
	mem_trace_struct();
    mem_trace_struct& operator=(mem_trace_struct&);
    mem_trace_struct& operator=(mem_trace_struct*);
    ~mem_trace_struct();
};

class ca_line_struct {
public:
  int ca_valid;
  int ca_mod;
  int ca_share;
  int ca_tag;
  int ca_cm_block;
  int ca_add1;
  int ca_add2;
  int ca_add3;
  int ca_add4;
	ca_line_struct();
    ca_line_struct& operator=(ca_line_struct&);
    ca_line_struct& operator=(ca_line_struct*);
    ~ca_line_struct();
};

class string_array
{
    public:
    char** vec;
    sim_entity* m_pParentEntity;
    int section, max, line;
  char *InstanceName;
  string_array(const string_array &x);
  string_array(int x);
string_array();
  string_array(int size, char *name);
char* operator[](int);
  string_array& operator=(char*&);
string_array& operator=(string_array& x);
  void Update(int index, char* data, int stage = 0);
    ~string_array();
};
class parameter_value_array
{
    public:
    int* vec;
    sim_entity* m_pParentEntity;
    int section, max, line;
  char *InstanceName;
  parameter_value_array(const parameter_value_array &x);
  parameter_value_array(int x);
parameter_value_array();
  parameter_value_array(int size, char *name);
int& operator[](int);
  parameter_value_array& operator=(int&);
parameter_value_array& operator=(parameter_value_array& x);
  void Update(int index, int data, int stage = 0);
    ~parameter_value_array();
};
class cache_memory
{
    public:
    ca_line_struct* vec;
    sim_entity* m_pParentEntity;
    int section, max, line;
  char *InstanceName;
  cache_memory(const cache_memory &x);
  cache_memory(int x);
cache_memory();
  cache_memory(int size, char *name);
ca_line_struct& operator[](int);
  cache_memory& operator=(ca_line_struct&);
cache_memory& operator=(cache_memory& x);
  void Update(int index, ca_line_struct data, int stage = 0);
    ~cache_memory();
};
class memory_trace
{
    public:
    mem_trace_struct* vec;
    sim_entity* m_pParentEntity;
    int section, max, line;
  char *InstanceName;
  memory_trace(const memory_trace &x);
  memory_trace(int x);
memory_trace();
  memory_trace(int size, char *name);
mem_trace_struct& operator[](int);
  memory_trace& operator=(mem_trace_struct&);
memory_trace& operator=(memory_trace& x);
  void Update(int index, mem_trace_struct data, int stage = 0);
    ~memory_trace();
};


// Global Parameter declarations
 #define NAMELENGTH 128

// Structures for initialising entities
struct TimerData {
  int Cycles;
};

struct mipsData {
int mem_trace_size;
  int TC;
};

struct p_cacheData {
  char status0;
  char status1;
int cache_size;
};

struct s_cacheData {
  char status0;
  char status1;
int cache_size;
};

struct mp_busData {
};

struct MemoryData {
  char action;
};

#include <global_fns.h>

// Class definitions for entities

class Timer : public sim_entity {
public :
  static   int Max_cycles;
  int Cycles;

// variables 

  int max_cycles;
  bool stop;
//entity references

  mips * myMIPS;

  Timer(const char *n, int Cycles_i);
  void dumpState(int stage, char type);
  void dump_state(int stage=0);
  void dump_state_i(int stage=0);
  void startup();
  void report();
  void body();
private:
  static const char* state_names[];
  static char NORMAL;
  char my_state;
};

class mips : public sim_entity {
public :
  sim_port to_p_cache;
  sim_port from_p_cache;
  static   int MIPS_delay;
  memory_trace mem_trace;
  int mem_trace_size;
  int mem_traceLength;
  int TC;

// Variable Declarations

  mem_trace_struct trace_file_line;      // Holds a line of the trace file.
  p1_struct pkt_to_p_cache;              // Packet bound for p_cache.
  char action;
  int max_address;
  bool stop, error;

//entity references

  Timer * myTimer;

  mips(const char *n, int mem_trace_size_i, int TC_i);
  void dumpState(int stage, char type);
  void dump_state(int stage=0);
  void dump_state_i(int stage=0);

void getmem_trace_structFromType(Type *t, mem_trace_struct *mem_trace_structInstance );
  void startup();
  void report();
  void body();
private:
  static const char* state_names[];
  static char M_STOPPED, M_RUNNING, M_WAITING;
  char my_state;
};

class p_cache : public sim_entity {
public :
  sim_port from_mips;
  sim_port to_mips;
  sim_port to_s_cache;
  sim_port from_s_cache;
  static   int P_delay;
  static   int P_size;
  char status0;
  char status1;
  cache_memory cache;
  int cache_size;
  int cacheLength;

/* Message Containers */

p1_struct pkt_from_mips;       // Packet inbound from mips.
p1_struct pkt_to_mips;         // Packet bound for mips.
p1_struct pkt_to_s_cache;      // Packet bound for s_cache.
p1_struct pkt_from_s_cache;    // Packet inbound from s_cache. 

/* Variables for cache related calculations */

int p_cache_size;
int clus_mem_size;
int add_req;                   // Requested address
int cm_block;                  // Cluster memory block
int slot;                      // Cache slot for this entry
int tag;                       // tag for this entry
int tag_boundary;              // Where tag boundarys fall in cluster memory.
char action;                   // is operation read / write?
int refer;
ca_line_struct current_line; // cache memory line processing space.
int count;                     // misc counter for loops.
 
void do_mips_event(sim_event&);
void do_s_cache_event(sim_event&);


  p_cache(const char *n, char status0_i, char status1_i, int cache_size_i);
  void dumpState(int stage, char type);
  void dump_state(int stage=0);
  void dump_state_i(int stage=0);

void getca_line_structFromType(Type *t, ca_line_struct *ca_line_structInstance );
  void startup();
  void report();
  void body();
private:
  static const char* state_names[];
  static char P_HIT, P_MISS, P_IDLE;
  char my_state;
};

class s_cache : public sim_entity {
public :
  sim_port from_p_cache;
  sim_port to_p_cache;
  sim_port to_mp_bus;
  sim_port from_mp_bus;
  static   int S_delay;
  static   int S_size;
  char status0;
  char status1;
  cache_memory cache;
  int cache_size;
  int cacheLength;

/* Message containiers */

p1_struct pkt_from_p_cache;  // Packet inbound from p_cache.
p1_struct pkt_to_p_cache;    // Packet bound for p_cache.
p1_struct pkt_to_mp_bus;     // Packet bound for mp_bus. 
p1_struct pkt_from_mp_bus;   // Packet inbound from mp_bus.   

// sim_event ev;                  // The event container.

/* Vars for cache related calculations         */
/* these are used to caclulate cache positions */
/* etc. via use of entity parameters / globals */                

int s_cache_size;
int clus_mem_size;
int add_req;                   // Requested address
int cm_block;                  // Cluster memory block in which address resides
int slot;                      // Cache slot for this entry.
int tag;                       // Cache tag for this address.
int tag_boundary;              // Where tag boundarys fall in clus_memory.	

char action;                   // Is operation 'r'ead or 'w'rite 
ca_line_struct current_line; // Current cache memory line.
int refer;                    // referal to memory?
int count;
void do_mpbus_event(sim_event&);
void do_p_cache_event(sim_event&);

  s_cache(const char *n, char status0_i, char status1_i, int cache_size_i);
  void dumpState(int stage, char type);
  void dump_state(int stage=0);
  void dump_state_i(int stage=0);

void getca_line_structFromType(Type *t, ca_line_struct *ca_line_structInstance );
  void startup();
  void report();
  void body();
private:
  static const char* state_names[];
  static char S_HIT, S_MISS, S_IDLE;
  char my_state;
};

class mp_bus : public sim_entity {
public :
  sim_port from_mips1;
  sim_port to_mips1;
  sim_port to_c_memory;
  sim_port from_c_memory;

//entity references

// Variable Declarations

p1_struct pkt_from_mips1;      // Packet inbound from mips1.
p1_struct pkt_to_mips1;        // Packet outbound to mips1
p1_struct pkt_to_c_memory;     // Packet bound for mp_bus.      
p1_struct pkt_from_c_memory;   // Packet inbound from c_memory.

  mp_bus(const char *n);
  void dumpState(int stage, char type);
  void dump_state(int stage=0);
  void dump_state_i(int stage=0);
  void startup();
  void report();
  void body();
private:
};

class Memory : public sim_entity {
public :
  sim_port from_mp_bus;
  sim_port to_mp_bus;
  static   int M_delay;
  static   int M_size;
  char action;

//
// Variable Declarations
//

p1_struct pkt_from_mp_bus;   // Packet inbound from mp_bus.
p1_struct pkt_to_mp_bus;     // Packet bound for mp_bus.      

  Memory(const char *n, char action_i);
  void dumpState(int stage, char type);
  void dump_state(int stage=0);
  void dump_state_i(int stage=0);
  void startup();
  void report();
  void body();
private:
};

// Declarations for functions to write state to trace
  extern void PutValue( char *buff, int val );
  extern void PutValue( char *buff, unsigned val );
  extern void PutValue( char *buff, char val );
  extern void PutValue( char *buff, double val );
  extern void PutValue( char *buff,char* val );
  extern void PutValue( char *buff, long val );
  extern void PutValue( char *buff, bool val );
  extern void PutValue( char *buff, p1_struct val );
extern void send_DATAPKT(sim_port &dest, p1_struct val, int colour=17, int stage=0);
extern void send_DATAPKT(sim_port &dest, p1_struct val, sim_time delay,int colour=17,int stage=0);
  extern void PutValue( char *buff, mem_trace_struct val );
  extern void PutValue( char *buff, ca_line_struct val );
  extern void PutValue( char *buff, const string_array& val);
  extern void PutValue( char *buff, const string_array* val);
  extern void PutValue( char *buff, const parameter_value_array& val);
  extern void PutValue( char *buff, const parameter_value_array* val);
  extern void PutValue( char *buff, const cache_memory& val);
  extern void PutValue( char *buff, const cache_memory* val);
  extern void PutValue( char *buff, const memory_trace& val);
  extern void PutValue( char *buff, const memory_trace* val);

#endif
