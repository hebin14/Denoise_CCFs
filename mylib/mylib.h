#include <rsf.h>
void *alloc1 (int n1, int size);
void *realloc1 (void *v, int n1, int size);
void free1 (void *p);
void **alloc2 (int n1, int n2, int size);
void free2 (void **p);
void ***alloc3 (int n1, int n2, int n3, int size);
void free3 (void ***p);
void ****alloc4 (int n1, int n2, int n3, int n4, int size);
void free4 (void ****p);
void *****alloc5 (int n1, int n2, int n3, int n4, int n5, int size);
void free5 (void *****p);
void ******alloc6 (int n1, int n2, int n3, int n4, int n5, int n6, 
                  int size);
void free6 (void ******p);
int *alloc1int (int n1);
int *realloc1int (int *v, int n1);
void free1int (int *p);
int **alloc2int (int n1, int n2);
void free2int (int **p);
int ***alloc3int (int n1, int n2, int n3);
void free3int (int ***p);
float *alloc1float (int n1);
float *realloc1float (float *v, int n1);
void free1float (float *p);
float **alloc2float (int n1, int n2);
void free2float (float **p);
float ***alloc3float (int n1, int n2, int n3);
void free3float (float ***p);
float ****alloc4float (int n1, int n2, int n3, int n4);
void free4float (float ****p);
float *****alloc5float (int n1, int n2, int n3, int n4, int n5);
void free5float (float *****p);
float ******alloc6float (int n1, int n2, int n3, int n4, int n5, 
                         int n6);
void free6float (float ******p);
int ****alloc4int (int n1, int n2, int n3, int n4);
void free4int (int ****p);
int *****alloc5int (int n1, int n2, int n3, int n4, int n5);
void free5int (int *****p);
unsigned char *****alloc5uchar(int n1,int n2,int n3,int n4,
	int n5);
void free5uchar(unsigned char *****p);
unsigned short *****alloc5ushort(int n1,int n2,int n3,int n4,
        int n5);
void free5ushort(unsigned short *****p);
unsigned short ******alloc6ushort(int n1,int n2,int n3,int n4,
        int n5,int n6);
void free6ushort(unsigned short ******p);
double *alloc1double (int n1);
double *realloc1double (double *v, int n1);
void free1double (double *p);
double **alloc2double (int n1, int n2);
void free2double (double **p);
double ***alloc3double (int n1, int n2, int n3);
void free3double (double ***p);
sf_complex *alloc1complex (int n1);
sf_complex *realloc1complex (sf_complex *v, int n1);
void free1complex (sf_complex *p);
sf_complex **alloc2complex (int n1, int n2);
void free2complex (sf_complex **p);
sf_complex ***alloc3complex (int n1, int n2, int n3);
void free3complex (sf_complex ***p);

sf_double_complex *alloc1dcomplex (int n1);
sf_double_complex *realloc1dcomplex (sf_double_complex *v, int n1);
void free1dcomplex (sf_double_complex *p);
sf_double_complex **alloc2dcomplex (int n1, int n2);
void free2dcomplex (sf_double_complex **p);
sf_double_complex ***alloc3dcomplex (int n1, int n2, int n3);
void free3dcomplex (sf_double_complex ***p);
void subricker(int order,int nt,float dt,float f0,float time_delay,float*fricker);
void sublcorr(int nt, int ltau, int nw_gaussian,int nccfs,float dt,float*trace1,float*trace2,float**local_corr);
void subnormalize(int n, float*trace);
void subtaper_bounday(int n, float perc, float*taper);
void subgcorr(int n11, int ltau,int lgcorr,int overlap,float dt,float*trace1,float*trace2,float**global_corr);