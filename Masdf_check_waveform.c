/** 
  * extract one-day noise data for a give station name from asdf format to rsf
  * Bin He (hebin14@mails.ucas.ac.cn) created on 2022-12-17 at UTD 
**/ 

/** 1.  the noise data are download into Download/RAW_DATA using Jupyter notebook
        the RAW_DATA have been preprocessed by removing the instrument response
  * 2.  then the raw data are preprocessed with noisepy into Download/Preproc,
        with a asdf format for each day for all stations
        the preprocessing follows standard noise preprocessing precedures as Bensen et al. (2007)
*/

/* input : hdf5 file which contains one-day's noise data for all stations
   input : station_name, which is XI.SN56.BHZ (network.station_name.component)
   output: station_name.rsf
*/

#include <rsf.h>
#include <math.h>
#include"mylib.h"
#include "iolib.h"
#include <hdf5.h>

int main (int argc, char** argv)
{
    
    sf_file out; //output file
    Statinfo statinfo;
    char *station_name,*h5path,stnm[MAXLEN]="",*fsmall_tag;
    int npts,ns,station_exist,wavefrom_exist;
    char attribute_name[MAXLEN]="sampling_rate";
    int ifexist;
    sf_init (argc,argv);
    if (NULL != (h5path=sf_getstring("h5path")))
    if (NULL != (station_name=sf_getstring("station_name")))
    if (NULL != (fsmall_tag=sf_getstring("small_tag"))){
      char fattr_path[MAXLEN],fwaveform_tag[MAXLEN];
      hid_t tmp_id= H5Fopen(h5path, H5F_ACC_RDONLY, H5P_DEFAULT);
      get_statinfo_from_station_name(station_name,&statinfo);
      sprintf(stnm,"%s.%s",statinfo.netwk,statinfo.stanm);
        
      station_exist=ASDF_exists_in_path(tmp_id, "Waveforms", stnm);
      if(station_exist<=0){
        sf_warning("station=%s do not exist station_exist=%d",stnm,station_exist);
        ifexist=0;
      }else{
        get_wavefrom_attr_path(statinfo,h5path,&fsmall_tag[0],&fattr_path[0]);
        wavefrom_exist=H5Lexists(tmp_id,&fattr_path[0],H5P_DEFAULT);
        if(wavefrom_exist<=0){
          sf_warning("arrt_path=%s with wavefrom_exist=%d",fattr_path,wavefrom_exist);
          ifexist=0;
        }else{
          hid_t attr_id=H5Aopen_by_name(tmp_id,&fattr_path[0],&attribute_name[0], H5P_DEFAULT, H5P_DEFAULT);
          H5Aread(attr_id, H5T_NATIVE_INT, &ns);
          float dt = 1.0/ns;
          get_wavefrom_tags(statinfo,h5path,&fsmall_tag[0],&fwaveform_tag[0]);
          npts = ASDF_get_num_elements_from_path(tmp_id, &fwaveform_tag[0]);
          H5Aclose(attr_id);H5Fclose(tmp_id);
          sf_warning("...station=%s npts=%d with a sampling rate=%d,dt=%f...",stnm,npts,ns,dt);
          ifexist=1;

        }
      }
    }else{
      sf_warning("must fsmall_tag %s:",fsmall_tag);    
      sf_warning("must provide station_name %s:",station_name);
      sf_warning("must provide h5path %s:",h5path);
      ifexist=1;
    }
    printf("\n%d\n",ifexist);
    return  0;
}

