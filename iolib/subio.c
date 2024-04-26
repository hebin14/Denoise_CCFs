#include"iolib.h"
#include <string.h>
int subgetline(char* filename){
  int mod;
  int num_lines;
  char fline[MAXLEN];
  FILE*fp=NULL;
  mod=access(filename,0);
    if(mod==-1){
		  printf("\nfilename=%s not exit!!\n",filename);
		return 0;
	  }else{
      num_lines=0;
	    fp = fopen(filename,"r");
	    while (fgets(fline, MAXLEN, fp))num_lines++;
	    fclose(fp);
      return num_lines;
    }
}
void subread_datalst(char filename[], char**filename_lst){
    char fline[MAXLEN];
    int is=0;
	FILE*fp = fopen(filename,"r");
	while (fgets(fline, MAXLEN, fp)){
        //be careful about the "\n" at the end of each line
	    fline[strcspn(fline, "\n")] = 0;
		  sprintf(&filename_lst[is][0],"%s",fline);
	    is++;
    }
	fclose(fp);
}
void subsplit_string(char str[],char delim[],char str1[],char str2[]){
  char *ptr=NULL;
  strtok_r(str, delim, &ptr);
  sprintf(&str1[0],&str[0]);
  sprintf(&str2[0],ptr);
}
void get_statinfo(int num_stat,char filename[],Statinfo*statinfo){
  int istat;
  char fline[MAXLEN],fdummy0[MAXLEN],fdummy1[MAXLEN],fdummy2[MAXLEN];
  FILE*fp=fopen(filename,"r");
  fgets(fline, MAXLEN, fp);
  for(istat=0;istat<num_stat;istat++){
    fgets(fline, MAXLEN, fp);
    //slit one by one
    sprintf(&fdummy0[0],&fline[0]);
    subsplit_string(&fdummy0[0],",",&fdummy1[0],&fdummy2[0]);
    sprintf(statinfo[istat].netwk,&fdummy1[0]);

    sprintf(&fdummy0[0],&fdummy2[0]);
    subsplit_string(&fdummy0[0],",",&fdummy1[0],&fdummy2[0]);
    sprintf(statinfo[istat].stanm,&fdummy1[0]);

    sprintf(&fdummy0[0],&fdummy2[0]);
    subsplit_string(&fdummy0[0],",",&fdummy1[0],&fdummy2[0]);
    sprintf(statinfo[istat].comp,&fdummy1[0]);

    sscanf(fdummy2,"%f,%f,%f",&statinfo[istat].stalon,&statinfo[istat].stalat,&statinfo[istat].staevel);
    //printf("stainfo=%s,%s,%s,%f,%f,%f\n",statinfo[istat].netwk,statinfo[istat].stanm,statinfo[istat].comp,statinfo[istat].stalat,statinfo[istat].stalat,statinfo[istat].staevel);
  }
  fclose(fp);
}
void get_statinfo_from_station_name(char station_name[],Statinfo*statinfo){
  char fdummy0[MAXLEN],fdummy1[MAXLEN],fdummy2[MAXLEN];
  sprintf(&fdummy0[0],&station_name[0]);
  subsplit_string(&fdummy0[0],".",&fdummy1[0],&fdummy2[0]);
  sprintf(&statinfo->netwk[0],&fdummy1[0]);
  sprintf(&fdummy0[0],&fdummy2[0]);
  subsplit_string(&fdummy0[0],".",&fdummy1[0],&fdummy2[0]);
  sprintf(&statinfo->stanm[0],&fdummy1[0]);
  sprintf(&statinfo->comp[0],&fdummy2[0]);
  statinfo->staevel=0.0;
  statinfo->stalat=0.0;
  statinfo->stalon=0.0;  
}
void get_wavefrom_tags(Statinfo statinfo,char filename[],char fsmall_tag[], char ftag[]){
  char ftime[MAXLEN],fdummy0[MAXLEN],fdummy1[MAXLEN],fdummy2[MAXLEN];
  Date day1,day2;
  char fday1[MAXLEN],fday2[MAXLEN];
  sprintf(&fdummy0[0],&filename[0]);
  subsplit_string(&fdummy0[0], "/",&fdummy1[0],&fdummy2[0]);
  sprintf(&fdummy0[0],&fdummy2[0]);
  subsplit_string(&fdummy0[0], "/",&fdummy1[0],&fdummy2[0]);

  sprintf(&fdummy0[0],&fdummy2[0]);
  subsplit_string(&fdummy0[0],".",&fdummy1[0],&fdummy2[0]);

  sprintf(&ftime[0],&fdummy1[0]);

  sprintf(&fdummy0[0],&ftime[0]);
  subsplit_string(&fdummy0[0],"T",&fdummy1[0],&fdummy2[0]);

  sscanf(fdummy1,"%d_%d_%d",&day1.year,&day1.month,&day1.date); 
  sscanf(fdummy2,"%d_%d_%d",&day2.year,&day2.month,&day2.date);
  
  sprintf(&fdummy1[0],"%04d-%02d-%02dT00:00:00",day1.year,day1.month,day1.date);
  sprintf(&fdummy2[0],"%04d-%02d-%02dT00:00:00",day2.year,day2.month,day2.date);
  sprintf(&ftag[0],"/Waveforms/%s.%s/%s.%s..%s__%s__%s__%s",statinfo.netwk,statinfo.stanm,statinfo.netwk,statinfo.stanm,statinfo.comp,&fdummy1[0],&fdummy2[0],&fsmall_tag[0]);

}

void get_wavefrom_attr_path(Statinfo statinfo,char filename[],char fsmall_tag[],char fattr_path[]){
  char ftime[MAXLEN],fdummy0[MAXLEN],fdummy1[MAXLEN],fdummy2[MAXLEN];
  Date day1,day2;
  char fday1[MAXLEN],fday2[MAXLEN];
  sprintf(&fdummy0[0],&filename[0]);
  subsplit_string(&fdummy0[0], "/",&fdummy1[0],&fdummy2[0]);
  sprintf(&fdummy0[0],&fdummy2[0]);
  subsplit_string(&fdummy0[0], "/",&fdummy1[0],&fdummy2[0]);

  sprintf(&fdummy0[0],&fdummy2[0]);
  subsplit_string(&fdummy0[0],".",&fdummy1[0],&fdummy2[0]);

  sprintf(&ftime[0],&fdummy1[0]);

  sprintf(&fdummy0[0],&ftime[0]);
  subsplit_string(&fdummy0[0],"T",&fdummy1[0],&fdummy2[0]);

  sscanf(fdummy1,"%d_%d_%d",&day1.year,&day1.month,&day1.date); 
  sscanf(fdummy2,"%d_%d_%d",&day2.year,&day2.month,&day2.date);
  
  sprintf(&fdummy1[0],"%04d-%02d-%02dT00:00:00",day1.year,day1.month,day1.date);
  sprintf(&fdummy2[0],"%04d-%02d-%02dT00:00:00",day2.year,day2.month,day2.date);
  sprintf(&fattr_path[0],"/Waveforms/%s.%s/%s.%s..%s__%s__%s__%s",statinfo.netwk,statinfo.stanm,statinfo.netwk,statinfo.stanm,statinfo.comp,&fdummy1[0],&fdummy2[0],&fsmall_tag[0]);

}


int ASDF_read_integer_attr(hid_t file_id, const char *grp_name,
                       const char *attr_name, int*attr_value) {
    int success=0;
    hid_t attr_id = H5Aopen_by_name(file_id, grp_name,attr_name, H5P_DEFAULT, H5P_DEFAULT);
    success = H5Aread(attr_id, H5T_NATIVE_INT, attr_value);
    H5Aclose(attr_id);
    return success;
}

int ASDF_read_float_attr(hid_t file_id, const char *grp_name,
                       const char *attr_name, float*attr_value) {
    int success=0;
    hid_t attr_id = H5Aopen_by_name(file_id, grp_name,attr_name, H5P_DEFAULT, H5P_DEFAULT);
    success = H5Aread(attr_id, H5T_NATIVE_FLOAT, attr_value);
    H5Aclose(attr_id);
    return success;
}

int ASDF_read_string_attr(hid_t file_id, const char *grp_name,
                       const char *attr_name, char*attr_value) {
    int success=0;
    hid_t attr_id = H5Aopen_by_name(file_id, grp_name,attr_name, H5P_DEFAULT, H5P_DEFAULT);
    //success = H5Aread(attr_id, H5T_NATIVE_FLOAT, attr_value);
    H5Aclose(attr_id);
    return success;
}

