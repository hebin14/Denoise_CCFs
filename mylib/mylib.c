#include <rsf.h>
#include"mylib.h"
/**
 * @brief ricker function
 * 
 * @param order ==-1 first time derivative, ==0 ricker, ==1 time integral, ==2, Gaussian
 * @param nt time samples
 * @param dt time interval
 * @param f0 peak frequency
 * @param time_delay extra time delay, the default one is 1.5*sqrt(6.)/(SF_PI*f0)
 * @param fricker output(nt)
 */

void subricker(int order,int nt,float dt,float f0,float time_delay,float*fricker)


{ 
	int   i;
	float t0;
	float *x,*dx,*px,*ppx;
	float da,a,a2,t;
	t0=1.5*sqrt(6.)/(SF_PI*f0)+time_delay;
	x=(float*)malloc(sizeof(float)*nt);
	dx=(float*)malloc(sizeof(float)*nt);
	px=(float*)malloc(sizeof(float)*nt);
	ppx=(float*)malloc(sizeof(float)*nt);

	da=SF_PI*f0;
	for(i=0;i<nt;i++){
		t=i*dt;
		a=SF_PI*f0*(t-t0);
		a2=a*a;
		ppx[i]=-0.5/da/da*exp(-a2);
		px[i]=a/da*exp(-a2);
		x[i]=(1.-2.*a2)*exp(-a2);
		dx[i]=-4.*a*da*exp(-a2)-2.*a*da*(1.-2.*a2)*exp(-a2);
	}
    if(order==-1)
	    for(i=0;i<nt;i++)fricker[i]=dx[i];
    else if(order==1)
	    for(i=0;i<nt;i++)fricker[i]=px[i];
    else if(order==2)
	    for(i=0;i<nt;i++)fricker[i]=ppx[i];
    else
	    for(i=0;i<nt;i++)fricker[i]=x[i];
	free(ppx);
	free(px);
	free(x);
	free(dx);
 }


void sublcorr(int nt, int ltau, int nw_gaussian,int nccfs,float dt,float*trace1,float*trace2,float**local_corr){
 	int it,it_plus_itau,it_minus_itau,itau; /* time samples of the signal */
    /*BinHe updated 29230807 to make the lcorr matrix smaller*/
    int ntau,*index_itau=NULL,njump;
    float *prestack_cc=NULL,*prestack_shift=NULL,dummy_sum,memoryinGB,memorytop=2.0;
    int it_minus_itau_overtwo,it_minus_itau_overtwo1,it_minus_itau_overtwo2;/*for shifting the cc*/
   
    //-------------------precalculate the shifted prestack data-------------//
    //pay attentation that this section is dangerous if your memory is limited----//
    ntau = ltau * 2 + 1; /*siez of cc time lag*/
    index_itau=sf_intalloc(ntau);
    for(itau=0;itau<ntau;itau++)index_itau[itau] = itau - ltau; /*from -ltau to ltau*/
    memoryinGB=(2*4*nt+2*4*nccfs*ntau)/1024./1024./1024.;
    if(memoryinGB>memorytop)sf_error("memory cost=%f larger than limited=%f",memoryinGB,memorytop);
    njump=SF_NINT(nt/nccfs);

    //solving the heat equation
    float sigma = nw_gaussian * dt;
    // this Tmax is the maximum of the heat-equation,has no relation to the time of signal
    float Tmax = sigma * sigma / 2.0; 
    int NT_iterations = 5, it_iteration, ix, nx=nt;
    float dT_iterations = Tmax / NT_iterations, dx=dt;
    // here the dt is actually the dx in heat equation
    float lambda = dT_iterations / dx / dx, miu =  (1 + 2 * lambda - sqrt(1.0 + 4.0 * lambda))/2.0/lambda;
    float *u1=NULL, *u2=NULL,*u01=NULL,*u02=NULL;

    u1 = sf_floatalloc(nx); u2 = sf_floatalloc(nx);
    u01 = sf_floatalloc(nx); u02 = sf_floatalloc(nx);
    //the normized gaussian function for the product of two Gaussian functions
    sigma = sigma/sqrt(2.0);
    prestack_cc = sf_floatalloc(nt);
    prestack_shift = sf_floatalloc(nt);
    float*taper_tau=sf_floatalloc(ntau),*localmax=sf_floatalloc(nccfs),max1=0,max2=0;
    subtaper_bounday(ntau,0.1,taper_tau);
    for(itau = 0; itau < ntau; itau++ ){
        memset(prestack_shift, 0., sizeof(float) * nt);
	    memset(prestack_cc, 0., sizeof(float) * nt);
         for(it=0 ; it< nt; it++){
            it_plus_itau = it + index_itau[itau];
            if(it_plus_itau >=0 && it_plus_itau<nt){
                prestack_cc[it] = trace2[it] * trace1[it_plus_itau];
            }
        }
        /*shifting the cc to the center, shift_cc=integral(f(t-tau/2)*g(t+tau/2)*dt)*/
        /* calculet the even index firstly, then to interp for the odd ones*/
        if(index_itau[itau]%2==0){
            for(it=ltau; it< nt-ltau; it++){
                it_minus_itau_overtwo = it - index_itau[itau]/2;
                prestack_shift[it] = prestack_cc[it_minus_itau_overtwo];
            }
        }else{
            for(it=ltau; it< nt-ltau; it++){
                it_minus_itau_overtwo1 = it - (index_itau[itau]-1)/2;
                it_minus_itau_overtwo2 = it - (index_itau[itau]+1)/2;
                prestack_shift[it] = (prestack_cc[it_minus_itau_overtwo1]+prestack_cc[it_minus_itau_overtwo2])/2.0;/*central interp*/
            }
        }
        memset(u01,0.,sizeof(float)*nt);memset(u02,0.,sizeof(float)*nt);
        //initial conditions for the two Gaussian functions
        for(ix = 0; ix <nx; ix++) u01[ix] = prestack_shift[ix];
        for(it_iteration = 0; it_iteration < NT_iterations; it_iteration++){
            memset(u1,0.,sizeof(float)*nt);memset(u2,0.,sizeof(float)*nt);
            for( ix = 1; ix < nx; ix ++) u1[ix] = u01[ix] + miu * u1[ix-1];
            for( ix = nx-2; ix>=0; ix--) u2[ix] = u1[ix] + miu * u2[ix+1];
            for( ix = 0; ix<nx; ix++) u01[ix] = miu/lambda * u2[ix];
        }
        for(ix = 0; ix <nccfs*njump; ix++) local_corr[itau][ix/njump] +=  u01[ix];
    }
    for(ix=0;ix<nccfs;ix++){
        max1=0.0;
        for(itau = 0; itau < ntau; itau++ ) if(SF_ABS(local_corr[itau][ix])>max1)max1=SF_ABS(local_corr[itau][ix]);
        localmax[ix]=max1;
    }
    max2=0;
    for(ix=0;ix<nccfs;ix++)if(localmax[ix]>max2)max2=localmax[ix];
    for(ix=0;ix<nccfs;ix++)localmax[ix]=1.0/(localmax[ix]+max2*0.01);
    for(itau = 0; itau < ntau; itau++ )for(ix=0;ix<nccfs;ix++) local_corr[itau][ix]*=localmax[ix]*taper_tau[itau];
    
    free1float(taper_tau);free1float(localmax);
    free1float(u1);free1float(u2);free1float(u01);free1float(u02);
	free1int(index_itau);
	free1float(prestack_cc);free1float(prestack_shift);
}

void subgcorr(int n11, int ltau,int lgcorr, int loverlap,float dt,float*trace1,float*trace2,float**glob_corr){
    
 	int it,it_plus_itau,itau, ntau,iw,itbeg,itend,i1,*index_itau;
    float dummy_sum,max1=0,max2=0;
    float*taper_tau=NULL,*localmax=NULL;
    
    ntau = ltau * 2 + 1; /*siez of cc time lag*/
    memset(glob_corr[0],0.,sizeof(float)*n11*ntau);
    index_itau=sf_intalloc(ntau);
    for(itau=0;itau<ntau;itau++)index_itau[itau] = itau - ltau; /*from -ltau to ltau*/
    taper_tau=sf_floatalloc(ntau);
    subtaper_bounday(ntau,0.1,taper_tau);
    localmax=sf_floatalloc(n11);
    memset(localmax,0.,sizeof(float)*n11);
    for(iw=0;iw<n11;iw++){
        itbeg=iw*(lgcorr-loverlap)+ltau;
        itend=itbeg+lgcorr;
        max1=0.0;
        for(itau = 0; itau < ntau; itau++ ){
            dummy_sum = 0.0;
            for(it=itbeg ; it< itend; it++){
                it_plus_itau = it + index_itau[itau];
                dummy_sum+=trace2[it] * trace1[it_plus_itau];
            }
            glob_corr[itau][iw] = dummy_sum;
            if(SF_ABS(dummy_sum)>max1)max1=SF_ABS(dummy_sum);
        }
        localmax[iw]=max1;
    }
    for(iw=0;iw<n11;iw++)if(localmax[iw]>max2)max2=localmax[iw];
    for(iw=0;iw<n11;iw++)localmax[iw]=1.0/(localmax[iw]+max2*0.01);
    for(iw=0;iw<n11;iw++)for(itau = 0; itau < ntau; itau++ ) glob_corr[itau][iw]*=localmax[iw]*taper_tau[itau];
    free1float(taper_tau);free1int(index_itau);free1float(localmax);
}
void subnormalize(int n, float*trace){
    float max=0.0,tmpabs;
    int i;
    for(i=0;i<n;i++){
        tmpabs = SF_ABS(trace[i]);
        if(tmpabs>max)max=tmpabs;
    }
    for(i=0;i<n;i++) trace[i]/=(max+1.0e-30);
}

void subtaper_bounday(int n, float perc, float*taper){
    if(perc>0.25){ 
        sf_warning("do not taper too much of your data, set it to be perc=0.25(50%/2) at most!!!!");
        perc=0.25;
    }
    int ntaper=SF_NINT(n*perc);
    int i;
    for(i=0;i<n;i++)taper[i]=1.0;
    for(i=0;i<ntaper;i++)taper[i]=sin(i*1.0/ntaper*SF_PI/2.0);
    for(i=n-ntaper;i<n;i++)taper[i]=sin((n-1-i)*1.0/ntaper*SF_PI/2.0);
}

