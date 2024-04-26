#include"stdio.h"
#include"hdf5.h"
#include"ASDF_read.h"
#include "ASDF_common.h"
#define MAXLEN 256
typedef struct{
  int year,month,date;
}Date;

typedef struct{
  char netwk[MAXLEN];
  char stanm[MAXLEN];
  char comp[MAXLEN];
  float stalon,stalat,staevel;
}Statinfo;
int subgetline(char* filename);
void subread_datalst(char filename[], char**filename_lst);
void get_statinfo(int num_stat,char filename[],Statinfo*statinfo);
void get_statinfo_from_station_name(char station_name[],Statinfo* statinfo);
void get_wavefrom_tags(Statinfo statinfo,char filename[],char fsmall_tag[],char fwavefrom_tag[]);
void get_wavefrom_attr_path(Statinfo statinfo,char filename[],char fsmall_tag[],char fattr_path[]);
int ASDF_read_integer_attr(hid_t file_id, const char *grp_name,
                       const char *attr_name, int*attr_value);
int ASDF_read_float_attr(hid_t file_id, const char *grp_name,
                       const char *attr_name, float*attr_value) ;
int ASDF_read_string_attr(hid_t file_id, const char *grp_name,
                       const char *attr_name, char*attr_value) ;

