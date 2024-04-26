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
    
    int n1,n2,i,j,n2_half;
    sf_file in,out,other,out2;
    float d1,d2,o1,o2;
    float**lcorr=NULL,**similarity=NULL,*stack=NULL,*fold;
    int do_simistack,fold_ccf;
    float maxval;
    sf_init(argc,argv);
    
    in = sf_input("in");
    out = sf_output("out");
    if (!sf_getint("do_simistack",&do_simistack)) do_simistack=0;
    if (!sf_getint("fold_ccf",&fold_ccf)) fold_ccf=0;
    if(do_simistack) other = sf_input("other");
    sf_warning("do_simi_stack=%s,fold_ccf=%s",do_simistack?"true":"false",fold_ccf?"true":"false");
        
    if (SF_FLOAT != sf_gettype(in)) sf_error("Need float input");

    if (!sf_histint(in,"n1",&n1)) sf_error("No n1= in input.");
    if (!sf_histint(in,"n2",&n2)) sf_error("No n2= in input.");
    

    if (!sf_histfloat(in,"d1",&d1)) sf_error("No d1= in input.");
    if (!sf_histfloat(in,"d2",&d2)) sf_error("No d2= in input.");
    
    if (!sf_histfloat(in,"o2",&o2)) sf_error("No o2= in input.");
    if(fold_ccf){
        n2_half = n2/2 + 1;
        fold=sf_floatalloc(n2_half);
        memset(fold,0.,sizeof(float)*n2_half);
    }
    sf_warning("n1=%d,d1=%f,o1=%f",n2,d2,o2);
    lcorr = sf_floatalloc2(n1,n2);
    stack = sf_floatalloc(n2);
    if(do_simistack)similarity=sf_floatalloc2(n1,n2);
    
    sf_floatread(lcorr[0],n1*n2,in);
    if(do_simistack)sf_floatread(similarity[0],n1*n2,other);
    memset(stack,0.,sizeof(float)*n2);
    
    for(j=0;j<n2;j++)for(i=0;i<n1;i++){
        if(do_simistack) 
            stack[j]+=lcorr[j][i]*similarity[j][i];
        else 
            stack[j]+=lcorr[j][i];
    }
    maxval=0;
    for(j=0;j<n2;j++) if(SF_ABS(stack[j])>maxval) maxval=SF_ABS(stack[j]);
    for(j=0;j<n2;j++) stack[j]/=maxval+1.0e-20;
    if(fold_ccf){
        for(j=n2/2;j<n2;j++)fold[j-n2/2] = stack[j];
        for(j=0;j<n2/2;j++) fold[n2/2-j] += stack[j];
    }
    
    if(do_simistack)free2float(similarity);
    
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
        sf_putint(out,"n1",n2);
        sf_putfloat(out,"d1",d2);
        sf_putfloat(out,"o1",o2);
        sf_putint(out,"n2",1);
        sf_putint(out,"o2",0);
        sf_putint(out,"d2",1);
        sf_floatwrite(stack,n2,out);
        free2float(lcorr);
    }
    free1float(stack);

    exit(0);
}


