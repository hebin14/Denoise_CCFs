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
    
    int n1,i1,ltau,ntau,itau,n11,n2_half,nselect,lgcorr,loverlap,n1stack; 
    sf_file in,out,other,stack;
    float d1,*tr1=NULL,*tr2=NULL;
    float Tau_max,**global_corr=NULL;
    int nbeg1,nend1,sbeg1,send1,nbeg2,nend2,sbeg2,send2,nbeg3,nend3;
    float gnrms=0.0,gsrms=0.0,lnrms=0.0,lsrms=0.0,gratio,lratio;
    float d2,o1,o2,win1,win2,win3,win4,win5,win6,dist,vsmin,vsmax,periodmax,tmin,tmax,t0min,t0max;
    float **gstack=NULL,epsilon,*fold=NULL,*final_stack=NULL,balance_factor,gcorr_len,gcorr_overlap;;
    int fold_ccf,num_dates;
    sf_init(argc,argv);
    in = sf_input("in");
    other = sf_input("other");
    stack = sf_input("stack");
    out = sf_output("out");
    
    if (SF_FLOAT != sf_gettype(in)) sf_error("Need float input");
    if (!sf_histint(in,"n1",&n1)) sf_error("No n1= in input.");
    if (!sf_histfloat(in,"d1",&d1)) sf_error("No d1= in input.");
    if (!sf_histint(stack,"n1",&n1stack)) sf_error("No n1= in stack.");
    if (!sf_histfloat(stack,"d2",&d2)) sf_error("No d2= in stack.");
    if (!sf_histfloat(stack,"o2",&o2)) sf_error("No o2= in stack.");
    if (!sf_getint("fold_ccf",&fold_ccf)) fold_ccf=0;
    if (!sf_getint("num_dates",&num_dates)) num_dates=30;
    if (!sf_getfloat("Tau_max",&Tau_max)) Tau_max=240.0;
    if (!sf_getfloat("dist",&dist)) dist=10;
    if (!sf_getfloat("vsmin",&vsmin)) vsmin=2.9;
    if (!sf_getfloat("vsmax",&vsmax)) vsmax=3.1;
    if (!sf_getfloat("periodmax",&periodmax)) periodmax=20.0;
    
    if (!sf_getfloat("gcorr_len",&gcorr_len)) gcorr_len=3600.;
    if (!sf_getfloat("gcorr_overlap",&gcorr_overlap)) gcorr_overlap=gcorr_len/2.0;
    tr1 = sf_floatalloc(n1);tr2 = sf_floatalloc(n1);
    sf_floatread(tr1,n1,in);sf_floatread(tr2,n1,other);
    lgcorr=SF_NINT(gcorr_len/d1);loverlap=SF_NINT(gcorr_overlap/d1);
    n11=SF_NINT((n1-lgcorr-ntau)/(lgcorr-loverlap))+1;
    if(n11<1){
        n11=1;lgcorr=n1;loverlap=0;
        sf_warning("segment correlation length longer than 1day, use one day instead!!!!");   
    }
    ltau = SF_NINT(Tau_max/d1);
    ntau = 2 * ltau + 1;
    global_corr = sf_floatalloc2(n11,ntau);
    memset(global_corr[0],0.,sizeof(float)*ntau*n11);
    subgcorr(n11, ltau,lgcorr,loverlap,d1,tr1,tr2,global_corr);
    if (!sf_getfloat("epsilon",&epsilon)) epsilon=1.0+1.0/(num_dates*n11);
    gstack = sf_floatalloc2(n1stack,ntau);
    sf_floatread(gstack[0],n1stack*ntau,stack);

    final_stack=sf_floatalloc(ntau);memset(final_stack,0.,sizeof(float)*ntau);
    nselect=0;
    t0min=o2;t0max=o2+ntau*d2;
    tmin=dist/vsmax;tmax=dist/vsmin;
    win3=-tmin+2*periodmax<0?-tmin+2*periodmax:0;
    win2=-tmax-2*periodmax>t0min?-tmax-2*periodmax:t0min+5*d2;
    win1=win2-4*periodmax>t0min?win2-4*periodmax:t0min+2*d2;
    
    win4=tmin-2*periodmax>0?tmin-2*periodmax:0;
    win5=tmax+2*periodmax<t0max?tmax+2*periodmax:t0max-5*d2;
    win6=win5+4*periodmax<t0max?win5+4*periodmax:t0max-2*d2;

    nbeg1=SF_NINT((win1-o2)/d2);nend1=SF_NINT((win2-o2)/d2);
    nbeg2=SF_NINT((win3-o2)/d2);nend2=SF_NINT((win4-o2)/d2);
    nbeg3=SF_NINT((win5-o2)/d2);nend3=SF_NINT((win6-o2)/d2);
    sbeg1=SF_NINT((win2-o2)/d2);send1=SF_NINT((win3-o2)/d2);
    sbeg2=SF_NINT((win4-o2)/d2);send2=SF_NINT((win5-o2)/d2);
    
    for(i1=0;i1<n11;i1++){
        gnrms=0.0,gsrms=0.0,lnrms=0.0,lsrms=0.0;
        for(itau=nbeg1;itau<=nend1;itau++){
            lnrms+=(gstack[itau][i1]-global_corr[itau][i1])*(gstack[itau][i1]-global_corr[itau][i1]);
            gnrms+=gstack[itau][i1]*gstack[itau][i1];
        }
        for(itau=nbeg2;itau<=nend2;itau++){
            lnrms+=(gstack[itau][i1]-global_corr[itau][i1])*(gstack[itau][i1]-global_corr[itau][i1]);
            gnrms+=gstack[itau][i1]*gstack[itau][i1];
        }
            for(itau=nbeg3;itau<=nend3;itau++){
            lnrms+=(gstack[itau][i1]-global_corr[itau][i1])*(gstack[itau][i1]-global_corr[itau][i1]);
            gnrms+=gstack[itau][i1]*gstack[itau][i1];
        }
        for(itau=sbeg1;itau<=send1;itau++){
            lsrms+=(gstack[itau][i1]-global_corr[itau][i1])*(gstack[itau][i1]-global_corr[itau][i1]);
            gsrms+=gstack[itau][i1]*gstack[itau][i1];
        }
        for(itau=sbeg2;itau<=send2;itau++){
            lsrms+=(gstack[itau][i1]-global_corr[itau][i1])*(gstack[itau][i1]-global_corr[itau][i1]);
            gsrms+=gstack[itau][i1]*gstack[itau][i1];
        }

        gnrms=sqrt(gnrms/(nend1+nend2+nend3-nbeg1-nbeg2-nbeg3+3));
        lnrms=sqrt(lnrms/(nend1+nend2+nend3-nbeg1-nbeg2-nbeg3+3));
        gsrms=sqrt(gsrms/(send1+send2-sbeg1-sbeg2+2));
        lsrms=sqrt(lsrms/(send1+send2-sbeg1-sbeg2+2));
        gratio=gsrms/(gnrms+1.0e-30);
        lratio=lsrms/(lnrms+1.0e-30);
        if(lratio/gratio<epsilon){
            for(itau=0;itau<ntau;itau++)final_stack[itau]+=global_corr[itau][i1];
            nselect+=1;
        }
        else
            sf_warning("\n il=%d with qk=%f",i1,lratio/gratio);
    }
    sf_warning("\n nselect=%d with epsilon=%f,with fold=%d,dist=%f,win1=%f,win2=%f,win3=%f,win4=%f,win5=%f,win6=%f\n"
        ,nselect,epsilon,fold_ccf,dist,win1,win2,win3,win4,win5,win6);
    
    if(fold_ccf){
        n2_half = ntau/2 + 1;
        fold=sf_floatalloc(n2_half);
        memset(fold,0.,sizeof(float)*n2_half);
        for(itau=ntau/2;itau<ntau;itau++)fold[itau-ntau/2] = final_stack[itau];
        for(itau=0;itau<ntau/2;itau++) fold[ntau/2-itau] += final_stack[itau];
    }
    if(fold_ccf){
        sf_putint(out,"n1",n2_half);
        sf_putfloat(out,"d1",d2);
        sf_putfloat(out,"o1",0);
        sf_putint(out,"n2",1);
        sf_putint(out,"o2",0);
        sf_putint(out,"d2",1);
        sf_floatwrite(fold,n2_half,out);
        free1float(fold);
    }else{
        sf_putint(out,"n1",ntau);
        sf_putfloat(out,"d1",d2);
        sf_putfloat(out,"o1",o2);
        sf_putint(out,"n2",1);
        sf_putint(out,"o2",0);
        sf_putint(out,"d2",1);
        sf_floatwrite(final_stack,ntau,out);
    }
    free2float(global_corr);free1float(final_stack);
    free1float(tr1);free1float(tr2);free2float(gstack);
    exit(0);
}


