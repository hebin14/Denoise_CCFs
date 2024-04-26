#include "iolib.h"
int julian_day(Date date){
  int id,im,iy;
  int num_days=0;
  for(id=1;id<=date.date;id++) num_days++;
  for(im=1;im<date.month;im++){
    if(im==1||im==3||im==5||im==7||im==8||im==10||im==12) num_days += 31;
    else if(im==2&&(date.year%4==0)) num_days += 29;
    else if(im==2&&(date.year%4!=0)) num_days += 28;
    else num_days +=30;
  }
  return num_days;
}
