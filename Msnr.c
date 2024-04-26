/*** Bin He created on 2022-08-26 at UTD
/* measure the global and local cross-correlations */
/* the local coross-correlations are calculated with recursive Gaussian convolution
/* realized by solving the 1D heat equation with implicit finte difference method
/* reference: signal and image restoration using shock filters and snisotropic diffusion
/* contact me by hebin14@mails.ucas.ac.cn
***/
#include <rsf.h>
#include <math.h>
#include"mylib.h"
int main (int argc, char* argv[])
{
    
    int n1,n2,i,j;
    sf_file in,other; 
    float d1,d2,o1,o2;
    float*f,*g,corr,esignal,enoise,snr;
    bool get_corr;
    float sb,se,nb,ne;
    int ib,ie,snw,nnw;
    sf_init(argc,argv);
    
    in = sf_input("in");
    if (!sf_getbool("get_corr",&get_corr)) get_corr=false;
    sf_warning("get_corr=%s",get_corr?"true":"false");
    if (SF_FLOAT != sf_gettype(in)) sf_error("Need float input");
    if (!sf_histint(in,"n1",&n1)) sf_error("No n1= in input.");
    if (!sf_histfloat(in,"d1",&d1)) sf_error("No d1= in input.");
    if (!sf_histfloat(in,"o2",&o2)) sf_error("No o2= in input.");
    if (!sf_histfloat(in,"o1",&o1)) sf_error("No o1= in input.");
    if (!sf_getfloat("sb",&sb)) sf_error("No sb= in input.");
    if (!sf_getfloat("se",&se)) sf_error("No se= in input.");
    if (!sf_getfloat("nb",&nb)) sf_error("No nb= in input.");
    if (!sf_getfloat("ne",&ne)) sf_error("No ne= in input.");
    f=sf_floatalloc(n1);
    sf_floatread(f,n1,in);
    if(get_corr){
        other = sf_input("other");
        g=sf_floatalloc(n1);
        sf_floatread(g,n1,other);
        esignal=0.0;
        enoise=0.0;
        corr=0.0;
        for(i=0;i<n1;i++){
            esignal+=f[i]*f[i];
            enoise+=g[i]*g[i];
            corr+=f[i]*g[i];
        }
        corr = corr/sqrt(esignal*enoise+1.0e-30);
    }
    ib=SF_NINT((sb-o1)/d1);
    ie=SF_NINT((se-o1)/d1)-1;
    sf_warning("signal window: ib=%d,ie=%d,ib2=%d,ie2=%d",ib,ie,n1-ie,n1-ib);
    snw=ie-ib;
    if(ib<0) sf_error("signal range sb is too small!!!");
    if(ie>n1) sf_error("signal range se(ie=%d) is too large!!!",ie);
    if(snw<=0) sf_error("signal window to small, chech sb and se!!!");
    esignal=0.0;
    enoise=0.0;
    for(i=ib;i<ie;i++)
        esignal+=f[i]*f[i];
    for(i=n1-ie;i<n1-ib;i++)
        esignal+=f[i]*f[i];
    esignal/=snw;
    ib=SF_NINT((nb-o1)/d1);
    ie=SF_NINT((ne-o1)/d1)-1;
    sf_warning("signal window: ib=%d,ie=%d,ib2=%d,ie2=%d",ib,ie,n1-ie,n1-ib);
    nnw=ie-ib;
    if(ib<0) sf_error("noise range nb is too small!!!");
    if(ie>n1) sf_error("noise range ne(ie=%d) is too large!!!",ie);
    if(nnw<=0) sf_error("noise window to small, chech nb and ne!!!");
    for(i=ib;i<ie;i++)
        enoise+=f[i]*f[i];
    for(i=n1-ie;i<n1-ib;i++)
        enoise+=f[i]*f[i];
    enoise/=nnw;
    sf_warning("signal window: sb=%f,se=%f,with a lenth=%d,noise window: nb=%f,ne=%f,with a lenth=%d",sb,se,snw,nb,ne,nnw);
    snr = sqrt(esignal/(enoise+1.0e-30));
    if(get_corr){
        printf("%f %f",snr,corr);
        free1float(f);free1float(g);
    }
    else{
        printf("%f",snr);
        free1float(f);
    }
    exit(0);
}


