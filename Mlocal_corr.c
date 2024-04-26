/*** Bin He created on 2022-08-26 at UTD
/* measure the global and local cross-correlations */
/* the local coross-correlations are calculated with recursive Gaussian convolution
/* realized by solving the 1D heat equation with implicit finte difference method
/* reference: signal and image restoration using shock filters and snisotropic diffusion
/* contact me by hebin14@mails.ucas.ac.cn
***/
// Bin He updated 2022-12-30
// modifty the defination of corss-correlation same as python
#include <rsf.h>
#include <math.h>
#include"mylib.h"
int main (int argc, char* argv[])
{
    
    int n1,i1,nw_gaussian,ltau,ntau,itau,nccfs; 
    sf_file in,out,other,out2;
    float d1,*tr1=NULL,*tr2=NULL;
    float Tau_max,**local_corr=NULL;
    float memoryinGB,memorytop=2.0,gaussian_window;
    int both_corr;
    sf_init(argc,argv);
    in = sf_input("in");
    
    other = sf_input("other");
    out = sf_output("out");
    out2 = sf_output("out2");
    

    if (SF_FLOAT != sf_gettype(in)) sf_error("Need float input");
    if (!sf_histint(in,"n1",&n1)) sf_error("No n1= in input.");
    if (!sf_histfloat(in,"d1",&d1)) sf_error("No d1= in input.");
    
    tr1 = sf_floatalloc(n1);tr2 = sf_floatalloc(n1);
    sf_floatread(tr1,n1,in);sf_floatread(tr2,n1,other);
   
    if (!sf_getfloat("Tau_max",&Tau_max)) Tau_max=240.0;
     /* Gaussian window size */
    if (!sf_getfloat("gaussian_window",&gaussian_window)) gaussian_window=1800.;
    
    nw_gaussian=SF_NINT(gaussian_window/d1);
    /* Gaussian window size */
    if (!sf_getint("nccfs",&nccfs)) nccfs=3600;
    sf_warning("n1=%d,nccfs=%d,gaussian_window=%f,Taumax=%f",n1,nccfs,gaussian_window,Tau_max);
    ltau = SF_NINT(Tau_max/d1);
    ntau = 2 * ltau + 1;
    memoryinGB=2.*4*nccfs*ntau/1024./1024./1024.;
    if(memoryinGB>memorytop)sf_error("memory cost=%f larger than limited=%f",memoryinGB,memorytop);
    
    local_corr  = sf_floatalloc2(nccfs,ntau);
    memset(local_corr[0],0.,sizeof(float)*ntau*nccfs);
   
    sublcorr(n1, ltau, nw_gaussian,nccfs, d1, tr1, tr2, local_corr);
    sf_putint(out,"n1",nccfs);
    sf_putint(out,"n2",ntau);
    sf_putfloat(out,"d1",d1*SF_NINT(n1/nccfs));
    sf_putfloat(out,"d2",d1);
    sf_putfloat(out,"o2",-(ltau*d1));
    sf_putfloat(out,"o1",0);
    sf_floatwrite(local_corr[0],ntau*nccfs,out);

    free2float(local_corr);
    free1float(tr1);free1float(tr2);

    exit(0);
}


