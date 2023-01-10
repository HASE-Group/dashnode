// Main Project file for Dashnode.


#include "Dashnode.h"
#include <signal.h>
#include <ArrayManager.h>
sim_system sim;


// Global Parameter definitions

// Functions to write state to trace
void PutValue( char *buff, int val )
{
  char tmp[15];
  sprintf( tmp, "%d ", val );
  strcat( buff, tmp );
}

void PutValue( char *buff, unsigned val )
{
  char tmp[15];
  sprintf( tmp, "%u ", val );
  strcat( buff, tmp );
}

void PutValue( char *buff, char val )
{
  int l = strlen(buff); 
  buff[l]=val; 
  buff[l+1]=' '; 
  buff[l+2]='\0'; 
}

void PutValue( char *buff, double val )
{
  char tmp[15];
  sprintf( tmp, "%2lf ", val );
  strcat( buff, tmp );
}

void PutValue( char *buff, char* val )
{
  int len = strlen(buff), i;
  for (i=0; val[i]!='\0'; i++) buff[len+i]=val[i]; 
  buff[len+i]=' '; buff[len+i+1]='\0'; 
}

void PutValue( char *buff, long val )
{
  char tmp[15];
  sprintf( tmp, "%ld ", val );
  strcat( buff, tmp );
}

void PutValue( char *buff, bool val )
{
  char tmp[15];
  sprintf( tmp, "%s ", val ? "true" : "false");
  strcat( buff, tmp );
}

p1_struct& p1_struct::operator=(p1_struct& x)
{
  p1_address = x.p1_address;
  strcpy(p1_rw,x.p1_rw);
  strcpy(p1_id,x.p1_id);
  return *this;
}

p1_struct& p1_struct::operator=(p1_struct* x)
{
  p1_address = x->p1_address;
  strcpy(p1_rw,x->p1_rw);
  strcpy(p1_id,x->p1_id);
  return *this;
}

p1_struct::~p1_struct()
{
}

 void PutValue( char *buff, p1_struct val )
{
  PutValue( buff, val.p1_address);
  PutValue( buff, val.p1_rw);
  PutValue( buff, val.p1_id);
}

void send_DATAPKT(sim_port &dest, p1_struct val, int colour, int stage)
{ send_DATAPKT(dest, val, 0.0, colour, stage); }

void send_DATAPKT( sim_port &dest, p1_struct val, sim_time delay ,int colour, int stage) {
  sim_schedule(dest, delay, DATAPKT, new p1_struct(val));
  if (colour>0) {
    char buff[250];
    int nb = sprintf(buff," %d %s DATAPKT ", colour,dest.get_pname());
    PutValue(buff, val);
    strcat( buff, "\n");
    if (delay == 0.0) sim.current_ent()->sim_trace('S', stage, 4, buff);
    else sim.current_ent()->sim_trace('S', stage, 4, delay, buff);
  }
}

mem_trace_struct& mem_trace_struct::operator=(mem_trace_struct& x)
{
  mt_address = x.mt_address;
  strcpy(mt_rw,x.mt_rw);
  strcpy(mt_id,x.mt_id);
  return *this;
}

mem_trace_struct& mem_trace_struct::operator=(mem_trace_struct* x)
{
  mt_address = x->mt_address;
  strcpy(mt_rw,x->mt_rw);
  strcpy(mt_id,x->mt_id);
  return *this;
}

mem_trace_struct::~mem_trace_struct()
{
}

 void PutValue( char *buff, mem_trace_struct val )
{
  PutValue( buff, val.mt_address);
  PutValue( buff, val.mt_rw);
  PutValue( buff, val.mt_id);
}

ca_line_struct& ca_line_struct::operator=(ca_line_struct& x)
{
  ca_valid = x.ca_valid;
  ca_mod = x.ca_mod;
  ca_share = x.ca_share;
  ca_tag = x.ca_tag;
  ca_cm_block = x.ca_cm_block;
  ca_add1 = x.ca_add1;
  ca_add2 = x.ca_add2;
  ca_add3 = x.ca_add3;
  ca_add4 = x.ca_add4;
  return *this;
}

ca_line_struct& ca_line_struct::operator=(ca_line_struct* x)
{
  ca_valid = x->ca_valid;
  ca_mod = x->ca_mod;
  ca_share = x->ca_share;
  ca_tag = x->ca_tag;
  ca_cm_block = x->ca_cm_block;
  ca_add1 = x->ca_add1;
  ca_add2 = x->ca_add2;
  ca_add3 = x->ca_add3;
  ca_add4 = x->ca_add4;
  return *this;
}

ca_line_struct::~ca_line_struct()
{
}

 void PutValue( char *buff, ca_line_struct val )
{
  PutValue( buff, val.ca_valid);
  PutValue( buff, val.ca_mod);
  PutValue( buff, val.ca_share);
  PutValue( buff, val.ca_tag);
  PutValue( buff, val.ca_cm_block);
  PutValue( buff, val.ca_add1);
  PutValue( buff, val.ca_add2);
  PutValue( buff, val.ca_add3);
  PutValue( buff, val.ca_add4);
}

string_array::string_array(const string_array &x)
{
  section=x.section;
  max=x.max;
  line=x.line;
  InstanceName=new char[strlen(x.InstanceName)+2];
  strcpy(InstanceName,x.InstanceName);
  vec = new char*[max];
  for (int i=0; i<max; i++)
    vec[i]=x.vec[i];
}

string_array::string_array()
{
  section=0;
  max=1;
  line=0;
  InstanceName=new char[6];
  strcpy(InstanceName,"None\0");
  vec = new char*[max];
}

string_array::string_array(int x)
{
  section=0;
  max=x;
  line=0;
  InstanceName=new char[6];
  strcpy(InstanceName,"None\0");
  vec = new char*[max];
}

string_array::string_array(int size, char* name)
{
  section=0;
  max=size;
  InstanceName=new char[strlen(name)+2];
  strcpy(InstanceName,name);
  vec = new char*[max];
}

char* string_array::operator[](int p)
{
  line = p;
  if (sim.current_ent() != NULL && (strcmp(InstanceName,"None")!=0))
    MEM_READ(InstanceName,line,0);
  return vec[p];
}

string_array& string_array::operator=(char*& x)
{
  vec[line] = x;
  if (strcmp(InstanceName,"None")!=0)
    MEM_UPDATE(InstanceName,line,vec[line],0);
  return *this;
}

string_array& string_array::operator=(string_array& x)
{
  max=x.max;line=x.line;section=x.section;
  if (x.InstanceName!=NULL) {
  if (strlen(x.InstanceName)>strlen(InstanceName)) { 
    delete [] InstanceName; 
    InstanceName=new char[strlen(x.InstanceName)+2];
  }
  strcpy(InstanceName,x.InstanceName);

}
  delete [] vec;
  vec = new char*[max];
  for (int i=0;i<x.max;i++)
    vec[i]=x.vec[i];
  return *this;
}

void string_array::Update(int index, char* x, int stage)
{
  line=index;
  vec[line] = x;
  MEM_UPDATE(InstanceName,line,vec[line],0);
}

string_array::~string_array()
{
  delete [] InstanceName;
  delete [] vec;
}

void PutValue(char *buff, const string_array* val) { PutValue(buff, *val); }
void PutValue( char *buff, const string_array& val)
{
  for(int i=0;i<val.max;i++) {
    PutValue(buff,val.vec[i]);
  }
}

parameter_value_array::parameter_value_array(const parameter_value_array &x)
{
  section=x.section;
  max=x.max;
  line=x.line;
  InstanceName=new char[strlen(x.InstanceName)+2];
  strcpy(InstanceName,x.InstanceName);
  vec = new int[max];
  for (int i=0; i<max; i++)
    vec[i]=x.vec[i];
}

parameter_value_array::parameter_value_array()
{
  section=0;
  max=1;
  line=0;
  InstanceName=new char[6];
  strcpy(InstanceName,"None\0");
  vec = new int[max];
}

parameter_value_array::parameter_value_array(int x)
{
  section=0;
  max=x;
  line=0;
  InstanceName=new char[6];
  strcpy(InstanceName,"None\0");
  vec = new int[max];
}

parameter_value_array::parameter_value_array(int size, char* name)
{
  section=0;
  max=size;
  InstanceName=new char[strlen(name)+2];
  strcpy(InstanceName,name);
  vec = new int[max];
}

int& parameter_value_array::operator[](int p)
{
  line = p;
  if (sim.current_ent() != NULL && (strcmp(InstanceName,"None")!=0))
    MEM_READ(InstanceName,line,0);
  return vec[p];
}

parameter_value_array& parameter_value_array::operator=(int& x)
{
  vec[line] = x;
  if (strcmp(InstanceName,"None")!=0)
    MEM_UPDATE(InstanceName,line,vec[line],0);
  return *this;
}

parameter_value_array& parameter_value_array::operator=(parameter_value_array& x)
{
  max=x.max;line=x.line;section=x.section;
  if (x.InstanceName!=NULL) {
  if (strlen(x.InstanceName)>strlen(InstanceName)) { 
    delete [] InstanceName; 
    InstanceName=new char[strlen(x.InstanceName)+2];
  }
  strcpy(InstanceName,x.InstanceName);

}
  delete [] vec;
  vec = new int[max];
  for (int i=0;i<x.max;i++)
    vec[i]=x.vec[i];
  return *this;
}

void parameter_value_array::Update(int index, int x, int stage)
{
  line=index;
  vec[line] = x;
  MEM_UPDATE(InstanceName,line,vec[line],0);
}

parameter_value_array::~parameter_value_array()
{
  delete [] InstanceName;
  delete [] vec;
}

void PutValue(char *buff, const parameter_value_array* val) { PutValue(buff, *val); }
void PutValue( char *buff, const parameter_value_array& val)
{
  for(int i=0;i<val.max;i++) {
    PutValue(buff,val.vec[i]);
  }
}

cache_memory::cache_memory(const cache_memory &x)
{
  section=x.section;
  max=x.max;
  line=x.line;
  InstanceName=new char[strlen(x.InstanceName)+2];
  strcpy(InstanceName,x.InstanceName);
  vec = new ca_line_struct[max];
  for (int i=0; i<max; i++)
    vec[i]=x.vec[i];
}

cache_memory::cache_memory()
{
  section=0;
  max=1;
  line=0;
  InstanceName=new char[6];
  strcpy(InstanceName,"None\0");
  vec = new ca_line_struct[max];
}

cache_memory::cache_memory(int x)
{
  section=0;
  max=x;
  line=0;
  InstanceName=new char[6];
  strcpy(InstanceName,"None\0");
  vec = new ca_line_struct[max];
}

cache_memory::cache_memory(int size, char* name)
{
  section=0;
  max=size;
  InstanceName=new char[strlen(name)+2];
  strcpy(InstanceName,name);
  vec = new ca_line_struct[max];
}

ca_line_struct& cache_memory::operator[](int p)
{
  line = p;
  if (sim.current_ent() != NULL && (strcmp(InstanceName,"None")!=0))
    MEM_READ(InstanceName,line,0);
  return vec[p];
}

cache_memory& cache_memory::operator=(ca_line_struct& x)
{
  vec[line] = x;
  if (strcmp(InstanceName,"None")!=0)
    MEM_UPDATE(InstanceName,line,vec[line],0);
  return *this;
}

cache_memory& cache_memory::operator=(cache_memory& x)
{
  max=x.max;line=x.line;section=x.section;
  if (x.InstanceName!=NULL) {
  if (strlen(x.InstanceName)>strlen(InstanceName)) { 
    delete [] InstanceName; 
    InstanceName=new char[strlen(x.InstanceName)+2];
  }
  strcpy(InstanceName,x.InstanceName);

}
  delete [] vec;
  vec = new ca_line_struct[max];
  for (int i=0;i<x.max;i++)
    vec[i]=x.vec[i];
  return *this;
}

void cache_memory::Update(int index, ca_line_struct x, int stage)
{
  line=index;
  vec[line] = x;
  MEM_UPDATE(InstanceName,line,vec[line],0);
}

cache_memory::~cache_memory()
{
  delete [] InstanceName;
  delete [] vec;
}

void PutValue(char *buff, const cache_memory* val) { PutValue(buff, *val); }
void PutValue( char *buff, const cache_memory& val)
{
  for(int i=0;i<val.max;i++) {
    PutValue(buff,val.vec[i]);
  }
}

memory_trace::memory_trace(const memory_trace &x)
{
  section=x.section;
  max=x.max;
  line=x.line;
  InstanceName=new char[strlen(x.InstanceName)+2];
  strcpy(InstanceName,x.InstanceName);
  vec = new mem_trace_struct[max];
  for (int i=0; i<max; i++)
    vec[i]=x.vec[i];
}

memory_trace::memory_trace()
{
  section=0;
  max=1;
  line=0;
  InstanceName=new char[6];
  strcpy(InstanceName,"None\0");
  vec = new mem_trace_struct[max];
}

memory_trace::memory_trace(int x)
{
  section=0;
  max=x;
  line=0;
  InstanceName=new char[6];
  strcpy(InstanceName,"None\0");
  vec = new mem_trace_struct[max];
}

memory_trace::memory_trace(int size, char* name)
{
  section=0;
  max=size;
  InstanceName=new char[strlen(name)+2];
  strcpy(InstanceName,name);
  vec = new mem_trace_struct[max];
}

mem_trace_struct& memory_trace::operator[](int p)
{
  line = p;
  if (sim.current_ent() != NULL && (strcmp(InstanceName,"None")!=0))
    MEM_READ(InstanceName,line,0);
  return vec[p];
}

memory_trace& memory_trace::operator=(mem_trace_struct& x)
{
  vec[line] = x;
  if (strcmp(InstanceName,"None")!=0)
    MEM_UPDATE(InstanceName,line,vec[line],0);
  return *this;
}

memory_trace& memory_trace::operator=(memory_trace& x)
{
  max=x.max;line=x.line;section=x.section;
  if (x.InstanceName!=NULL) {
  if (strlen(x.InstanceName)>strlen(InstanceName)) { 
    delete [] InstanceName; 
    InstanceName=new char[strlen(x.InstanceName)+2];
  }
  strcpy(InstanceName,x.InstanceName);

}
  delete [] vec;
  vec = new mem_trace_struct[max];
  for (int i=0;i<x.max;i++)
    vec[i]=x.vec[i];
  return *this;
}

void memory_trace::Update(int index, mem_trace_struct x, int stage)
{
  line=index;
  vec[line] = x;
  MEM_UPDATE(InstanceName,line,vec[line],0);
}

memory_trace::~memory_trace()
{
  delete [] InstanceName;
  delete [] vec;
}

void PutValue(char *buff, const memory_trace* val) { PutValue(buff, *val); }
void PutValue( char *buff, const memory_trace& val)
{
  for(int i=0;i<val.max;i++) {
    PutValue(buff,val.vec[i]);
  }
}




int main(int argc, char *argv[])
{
  signal(SIGALRM, SIG_IGN);
  signal(SIGPROF, SIG_IGN);
  signal(SIGWINCH, SIG_IGN);
  if (argc>1) freopen(argv[1], "w", stdout);
  char buffer[80];
  int i;
  sim.set_trc_level(0);

  // Variables declarations for parameters
  TimerData ATimerData;
  mipsData AmipsData;
  p_cacheData Ap_cacheData;
  s_cacheData As_cacheData;
  mp_busData Amp_busData;
  MemoryData AMemoryData;

  // Read in global parameters
  FILE *FP;
  FP = fopen( "dashnode.params", "r");
  if (FP==NULL) {
    printf("Couldn't open dashnode.params.\n");
    exit(1);
  }

//Initialise static parameters from the .params file.
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%d",&Timer::Max_cycles);}
  else printf("Couldn't read Max_cycles\n");
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%d",&mips::MIPS_delay);}
  else printf("Couldn't read MIPS_delay\n");
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%d",&p_cache::P_delay);}
  else printf("Couldn't read P_delay\n");
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%d",&p_cache::P_size);}
  else printf("Couldn't read P_size\n");
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%d",&s_cache::S_delay);}
  else printf("Couldn't read S_delay\n");
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%d",&s_cache::S_size);}
  else printf("Couldn't read S_size\n");
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%d",&Memory::M_delay);}
  else printf("Couldn't read M_delay\n");
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%d",&Memory::M_size);}
  else printf("Couldn't read M_size\n");

  // Calls to create all entities

  // Create entity TIMER.
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%d",&ATimerData.Cycles);}
  else printf("Couldn't read Cycles\n");

  registering("TIMER", "TIMER", "Timer");
 new Timer("TIMER",ATimerData.Cycles
);

  // Create entity MIPS.
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%d",&AmipsData.mem_trace_size);}
  else printf("Couldn't read mem_trace\n");
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%d",&AmipsData.TC);}
  else printf("Couldn't read TC\n");

  registering("MIPS", "MIPS", "mips");
 new mips("MIPS",AmipsData.mem_trace_size
,AmipsData.TC
);

  // Create entity P_CACHE.
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%c",&Ap_cacheData.status0);}
  else printf("Couldn't read status0\n");
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%c",&Ap_cacheData.status1);}
  else printf("Couldn't read status1\n");
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%d",&Ap_cacheData.cache_size);}
  else printf("Couldn't read cache\n");

  registering("P_CACHE", "P_CACHE", "p_cache");
 new p_cache("P_CACHE",Ap_cacheData.status0
,Ap_cacheData.status1
,Ap_cacheData.cache_size
);

  // Create entity S_CACHE.
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%c",&As_cacheData.status0);}
  else printf("Couldn't read status0\n");
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%c",&As_cacheData.status1);}
  else printf("Couldn't read status1\n");
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%d",&As_cacheData.cache_size);}
  else printf("Couldn't read cache\n");

  registering("S_CACHE", "S_CACHE", "s_cache");
 new s_cache("S_CACHE",As_cacheData.status0
,As_cacheData.status1
,As_cacheData.cache_size
);

  // Create entity MP_BUS.

  registering("MP_BUS", "MP_BUS", "mp_bus");
 new mp_bus("MP_BUS");

  // Create entity MEMORY.
  if (fgets(buffer,80,FP)!=NULL){
    sscanf(buffer,"%c",&AMemoryData.action);}
  else printf("Couldn't read action\n");

  registering("MEMORY", "MEMORY", "Memory");
 new Memory("MEMORY",AMemoryData.action
);
// Icon crest created, no declaration of behaviour.
// Icon title created, no declaration of behaviour.
// Icon parameters created, no declaration of behaviour.
  sim.link_ports("S_CACHE","to_mp_bus","MP_BUS","from_mips1");
  sim.link_ports("MIPS","to_p_cache","P_CACHE","from_mips");
  sim.link_ports("P_CACHE","to_mips","MIPS","from_p_cache");
  sim.link_ports("P_CACHE","to_s_cache","S_CACHE","from_p_cache");
  sim.link_ports("S_CACHE","to_p_cache","P_CACHE","from_s_cache");
  sim.link_ports("MP_BUS","to_mips1","S_CACHE","from_mp_bus");
  sim.link_ports("MP_BUS","to_c_memory","MEMORY","from_mp_bus");
  sim.link_ports("MEMORY","to_mp_bus","MP_BUS","from_c_memory");

  fclose( FP );

  sim.run("/Users/RNI/HASE/Models/dash/V1.4/results/Dashnode");
  return 0;
}
