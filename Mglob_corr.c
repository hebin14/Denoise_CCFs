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
    
    time_t time0,time1,it;
    int n1,i1,ltau,ntau,itau,nccfs,nsubcorr,lgcorr,loverlap; 
    sf_file in,out,other,out2;
    float d1,*tr1=NULL,*tr2=NULL;
    float Tau_max,**global_corr=NULL,**gcorr=NULL,*sumgcorr=NULL;
    float memoryinGB,memorytop=2.0,gcorr_len,gcorr_overlap;
    time0=time(NULL);
    
    sf_init(argc,argv);
    in = sf_input("in");
    
    other = sf_input("other");
    out = sf_output("out");
    
    if (SF_FLOAT != sf_gettype(in)) sf_error("Need float input");
    if (!sf_histint(in,"n1",&n1)) sf_error("No n1= in input.");
    if (!sf_histfloat(in,"d1",&d1)) sf_error("No d1= in input.");
    
    tr1 = sf_floatalloc(n1);tr2 = sf_floatalloc(n1);
    sf_floatread(tr1,n1,in);sf_floatread(tr2,n1,other);
   
    if (!sf_getfloat("Tau_max",&Tau_max)) Tau_max=240.0;
    //  /* sgement correlation length */
    if (!sf_getfloat("gcorr_len",&gcorr_len)) gcorr_len=3600.;
    if (!sf_getfloat("gcorr_overlap",&gcorr_overlap)) gcorr_overlap=gcorr_len/2.0;
    if (!sf_getint("nccfs",&nccfs)) nccfs=3600;
    
    ltau = SF_NINT(Tau_max/d1);
    ntau = 2 * ltau + 1;
    memoryinGB=2.*4*nccfs*ntau/1024./1024./1024.;
    if(memoryinGB>memorytop)sf_error("memory cost=%f larger than limited=%f",memoryinGB,memorytop);
    global_corr = sf_floatalloc2(nccfs,ntau);   
    memset(global_corr[0],0.,sizeof(float)*ntau*nccfs);
    lgcorr=SF_NINT(gcorr_len/d1);loverlap=SF_NINT(gcorr_overlap/d1);
    nsubcorr=SF_NINT((n1-lgcorr-ntau)/(lgcorr-loverlap))+1;
    if(nsubcorr<1){
        nsubcorr=1;lgcorr=n1;loverlap=0;
        sf_warning("segment correlation length longer than 1day, use one day instead!!!!");   
    }
    gcorr=sf_floatalloc2(nsubcorr,ntau);sumgcorr=sf_floatalloc(ntau);
    memset(gcorr[0],0.,sizeof(float)*ntau*nsubcorr);memset(sumgcorr,0.,sizeof(float)*ntau); 
    sf_warning("n11=%d,ltau=%d,lgcorr=%d,loverlap=%d,d1=%f",nsubcorr,ltau,lgcorr,loverlap,d1);  
    subgcorr(nsubcorr, ltau, lgcorr,loverlap, d1, tr1, tr2, gcorr);
    for(itau=0;itau<ntau;itau++)for(i1=0;i1<nsubcorr;i1++)sumgcorr[itau]+=gcorr[itau][i1];
    for(itau=0;itau<ntau;itau++)for(i1=0;i1<nccfs;i1++)global_corr[itau][i1]=sumgcorr[itau];

    sf_putint(out,"n1",nccfs);
    sf_putint(out,"n2",ntau);
    sf_putfloat(out,"d1",d1*SF_NINT(n1/nccfs));
    sf_putfloat(out,"d2",d1);
    sf_putfloat(out,"o2",-(ltau*d1));
    sf_putfloat(out,"o1",0);
    sf_floatwrite(global_corr[0],ntau*nccfs,out);

    free2float(global_corr);free1float(sumgcorr);free2float(gcorr);
    free1float(tr1);free1float(tr2);
    time1=time(NULL);

    //sf_warning("T0=%d,T1=%d,Tdiff=%d",time0,time1,time1-time0);
    exit(0);
}


