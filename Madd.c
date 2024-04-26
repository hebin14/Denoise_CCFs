/* Add, multiply, or divide  RSF datasets.

Takes: [< file0.rsf] file1.rsf file2.rsf ...

The various operations, if selected, occur in the following order:

(1) Take absolute value, abs=
(2) Add a scalar, add=
(3) Take the natural logarithm, log=
(4) Take the square root, sqrt=
(5) Multiply by a scalar, scale=
(6) Compute the base-e exponential, exp=
(7) Add, multiply, or divide the data sets, mode=

sfadd operates on integer, float, or complex data, but all the input
and output files must be of the same data type.

An alternative to sfadd is sfmath, which is more versatile, but may be
less efficient.
*/
/*
  Copyright (C) 2004 University of Texas at Austin
  
  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <math.h>
#include <string.h>

#include <unistd.h>

#include <rsf.h>

static void check_compat(sf_datatype type, 
			 size_t     nin, 
			 size_t     nopen,
			 sf_file*   ins,
			 const char** filename,
			 int        dim, 
			 const off_t *n);
static void add_float (int   collect, 
		       size_t nbuf, 
		       float* buf, 
		       const float* bufi, 
		       char   cmode, 
		       float  scale, 
		       float  add, 
		       int   abs_flag, 
		       int   log_flag, 
		       int   sqrt_flag, 
		       int   exp_flag);
static void add_int (int     collect, 
		     size_t   nbuf, 
		     int*     buf, 
		     const int*     bufi, 
		     char     cmode, 
		     float    scale, 
		     float    add, 
		     int     abs_flag, 
		     int     log_flag, 
		     int     sqrt_flag, 
		     int     exp_flag);
static void add_complex (int        collect, 
			 size_t      nbuf, 
			 sf_complex* buf, 
			 const sf_complex* bufi, 
			 char        cmode, 
			 float       scale, 
			 float       add, 
			 int        abs_flag, 
			 int        log_flag, 
			 int        sqrt_flag, 
			 int        exp_flag);

int countlines(char *filename)
{
  // count the number of lines in the file called filename                                    
  FILE *fp = fopen(filename,"r");
  int ch=0;
  int lines=0;

  if (fp == NULL)return 0;
  while ((ch = fgetc(fp)) != EOF)
  	if (ch == '\n')lines++;
  fclose(fp);
  return lines;
}
int main (int argc, char* argv[])
{
    int i, dim,ch;
    off_t n[SF_MAX_DIM], nsiz, *tell;
    size_t j, nin, nbuf, nopen, open_max;
    sf_file *ins, in, out;
    float *scale, *add;
    int *sqrt_flag, *abs_flag, *log_flag, *exp_flag, collect;
    char cmode, *buf, *bufi, *prog;
    const char *mode, filename[80];
		char *ccfile;
    sf_datatype type;
    FILE *fp = NULL;
    /* init RSF */
    sf_init (argc, argv);
    if (NULL != (ccfile=sf_getstring("ccfile"))){
			nin = countlines(ccfile);
			if(nin<1) exit(0);
		}
		nopen=nin;
		ins = (sf_file*) sf_alloc((size_t) nopen,sizeof(sf_file));
		fp = fopen(ccfile,"r");
		i=0;
  	while ((ch = fgetc(fp)) != EOF){
  		if (ch == '\n'){
				fscanf(fp, "%s", &filename);
				ins[i] = sf_input(filename);
				i++;
			}
		}
  	fclose(fp);
  
    out = sf_output ("out");
    sf_fileflush(out,ins[0]);
    
    nbuf = sf_bufsiz(ins[0]);
    buf  = sf_charalloc(nbuf);
    bufi = sf_charalloc(nbuf);
    
    /* default coefficients and flags */
    scale     = sf_floatalloc(nin);
    add       = sf_floatalloc(nin);  
    sqrt_flag = sf_intalloc(nin);
    abs_flag  = sf_intalloc(nin);
    log_flag  = sf_intalloc(nin);
    exp_flag  = sf_intalloc(nin);
    
    for (j = 0; j < nin; j++) {
			scale[j] = 1.;
			add[j]   = 0.;
			sqrt_flag[j] = abs_flag[j] = log_flag[j] = exp_flag[j] = 0;
    }

    (void) sf_getfloats("scale",scale,nin); 
    /* Scalar values to multiply each dataset with */

    (void) sf_getfloats("add",add,nin);
    /* Scalar values to add to each dataset */

    (void) sf_getints("sqrt",sqrt_flag,nin);
    /* If true take square root */

    (void) sf_getints("abs",abs_flag,nin);
    /* If true take absolute value */

    (void) sf_getints("log",log_flag,nin);
    /* If true take logarithm */

    (void) sf_getints("exp",exp_flag,nin);
    /* If true compute exponential */

    prog = sf_getprog();
    if (NULL != strstr(prog,"mul")) {
			mode = "mul";
    } else if (NULL != strstr(prog,"div")) {
			mode = "div";
    } else {
			mode = sf_getstring("mode");
       /* 'a' means add (default), 
	  	'p' or 'm' means multiply, 
	  	'd' means divide 
       */
    }

    cmode = (NULL==mode)? 'a':mode[0];

    /* verify file compatibility */
    dim = sf_largefiledims(ins[0],n); /* input files dimensions */
    for (nsiz=1, i=0; i < dim; i++) {
			nsiz *= n[i];
    }                           /* number of elements in input files */
    type = sf_gettype(ins[0]);
    //check_compat(type,nin,nopen,ins,filename,dim,n);
    /* end verify file compatibility */

    for (nbuf /= sf_esize(ins[0]); nsiz > 0; nsiz -= nbuf) {
			if (nbuf > nsiz) nbuf=nsiz;

			for (j=0; j < nin; j++) {
	    	in = ins[j];

	    	collect = (int) (j != 0);
	    	switch(type) {
					case SF_FLOAT:
		    		sf_floatread((float*) bufi,
				 			nbuf,
				 			in);	    
		    		add_float(collect, 
			      	nbuf,
			      	(float*) buf,
			      	(const float*) bufi, 
			      	cmode, 
			      	scale[j], 
			      	add[j], 
			      	abs_flag[j], 
			      	log_flag[j], 
			      	sqrt_flag[j], 
			      	exp_flag[j]);
		    		break;
					case SF_COMPLEX:		    
		    		sf_complexread((sf_complex*) bufi,
				  	 nbuf,
				   		in);
		    		add_complex(collect, 
							nbuf,
							(sf_complex*) buf,
							(const sf_complex*) bufi, 
							cmode, 
							scale[j], 
							add[j], 
							abs_flag[j], 
							log_flag[j], 
							sqrt_flag[j], 
							exp_flag[j]);
		    		break;
					case SF_INT:
		    		sf_intread((int*) bufi,
			       nbuf,
			       in);
		    		add_int(collect, 
			    		nbuf,
			    		(int*) buf,
			    		(const int*) bufi, 
			    		cmode, 
			    		scale[j], 
			    		add[j], 
			    		abs_flag[j], 
			    		log_flag[j], 
			    		sqrt_flag[j], 
			    		exp_flag[j]);
		    		break;
					default:
		    		sf_error("wrong type");
		    		break;
	    	}
			} /* j */

			switch(type) {
	    	case SF_FLOAT:
					sf_floatwrite((float*) buf,nbuf,out);
					break;
	    	case SF_COMPLEX:
					sf_complexwrite((sf_complex*) buf,nbuf,out);
					break;
	    	case SF_INT:
					sf_intwrite((int*) buf,nbuf,out);
					break;
	    	default:
					sf_error("wrong type");
				break;
			}	
	
    }
    

  exit(0);
}

static void add_float (int   collect,    /* if collect */
		       size_t nbuf,       /* buffer size */
		       float* buf,        /* output [nbuf] */
		       const float* bufi, /* input  [nbuf] */  
		       char   cmode,      /* operation */
		       float  scale,      /* scale factor */
		       float  add,        /* add factor */
		       int   abs_flag,   /* if abs */
		       int   log_flag,   /* if log */
		       int   sqrt_flag,  /* if sqrt */
		       int   exp_flag    /* if exp */)
/* Add floating point numbers */
{
    size_t j;
    float f;

    for (j=0; j < nbuf; j++) {
	f = bufi[j];
	if (abs_flag)    f = fabsf(f);
	f += add;
	if (log_flag)    f = logf(f);
	if (sqrt_flag)   f = sqrtf(f);
	if (1. != scale) f *= scale;
	if (exp_flag)    f = expf(f);
	if (collect) {
	    switch (cmode) {
		case 'p': /* product */
		case 'm': /* multiply */
		    buf[j] *= f;
		    break;
		case 'd': /* delete */
		    if (f != 0.) buf[j] /= f;
		    break;
		default:  /* add */
		    buf[j] += f;
		    break;
	    }
	} else {
	    buf[j] = f;
	}
    }
}

static void add_int (int   collect, 
		     size_t nbuf, 
		     int*   buf, 
		     const int*   bufi, 
		     char   cmode, 
		     float  scale, 
		     float  add, 
		     int   abs_flag, 
		     int   log_flag, 
		     int   sqrt_flag, 
		     int   exp_flag)
{
    size_t j;
    float f;

    for (j=0; j < nbuf; j++) {
	f = (float) bufi[j];
	if (abs_flag)    f = fabsf(f);
	f += add;
	if (log_flag)    f = logf(f);
	if (sqrt_flag)   f = sqrtf(f);
	if (1. != scale) f *= scale;
	if (exp_flag)    f = expf(f);
	if (collect) {
	    switch (cmode) {
		case 'p':
		case 'm':
		    buf[j] *= f;
		    break;
		case 'd':
		    if (f != 0.) buf[j] /= f;
		    break;
		default:
		    buf[j] += f;
		    break;
	    }
	} else {
	    buf[j] = f;
	}
    }
}

static void add_complex (int        collect, 
			 size_t      nbuf, 
			 sf_complex* buf, 
			 const sf_complex* bufi, 
			 char        cmode, 
			 float       scale, 
			 float       add, 
			 int        abs_flag, 
			 int        log_flag, 
			 int        sqrt_flag, 
			 int        exp_flag)
{
    size_t j;
    sf_complex c;
    
    for (j=0; j < nbuf; j++) {
	c = bufi[j];
	if (abs_flag)  {
#ifdef SF_HAS_COMPLEX_H
	    c = cabsf(c);
#else
	    c.r = cabsf(c);
	    c.i = 0.;
#endif
	}
#ifdef SF_HAS_COMPLEX_H
	c += add;
#else
	c.r += add;
#endif
	if (log_flag)    c = clogf(c);
	if (sqrt_flag)   c = csqrtf(c);
	if (1. != scale) {
#ifdef SF_HAS_COMPLEX_H
	    c *= scale;
#else
	    c = sf_crmul(c,scale);
#endif
	}
	if (exp_flag)    c = cexpf(c);
	if (collect) {
	    switch (cmode) {
		case 'p':
		case 'm':
#ifdef SF_HAS_COMPLEX_H
		    buf[j] *= c;
#else
		    buf[j] = sf_cmul(buf[j],c);
#endif
		    break;
		case 'd':
		    if (cabsf(c) != 0.) {
#ifdef SF_HAS_COMPLEX_H			
			buf[j] /= c;
#else
			buf[j] = sf_cdiv(buf[j],c);
#endif	
		    }
		    break;
		default:
#ifdef SF_HAS_COMPLEX_H	
		    buf[j] += c;
#else
		    buf[j] = sf_cadd(buf[j],c);
#endif
		    break;
	    }
	} else {
	    buf[j] = c;
	}
    }
}

/*------------------------------------------------------------*/
static void 
check_compat (sf_datatype type      /* data type */, 
	      size_t      nin       /* total number of files */,
	      size_t      nopen     /* number of open files */,
	      sf_file*    ins       /* input files [nopen] */,
	      const char **filename /* input file names [nin] */,
	      int         dim       /* file dimensionality */, 
	      const off_t*  n       /* dimensions [dim] */)
/* Check that the input files are compatible. 
   Issue error for type mismatch or size mismatch.
   Issue warning for grid parameters mismatch. */
{
    int ni, id;
    size_t i;
    float d, di, o, oi;
    char key[3];
    sf_file in;
    const float tol=1.e-5; /* tolerance for comparison */
    
    for (i=1; i < nin; i++) {
	in = (i >= nopen)? sf_input(filename[i]): ins[i];

	if (sf_gettype(in) != type) 
	    sf_error ("type mismatch: need %d",type);
	for (id=1; id <= dim; id++) {
	    (void) snprintf(key,3,"n%d",id%10u);
	    if (!sf_histint(in,key,&ni) || ni != n[id-1])
#if defined(__cplusplus) || defined(c_plusplus)
		sf_error("%s mismatch: need %ld",key,
			 (long int) n[id-1]);
#else
	        sf_error("%s mismatch: need %lld",key,
			 (long long int) n[id-1]);
#endif
	    (void) snprintf(key,3,"d%d",id%10u);
	    if (sf_histfloat(ins[0],key,&d)) {
		if (!sf_histfloat(in,key,&di) || 
		    (fabsf(di-d) > tol*fabsf(d)))
		    sf_warning("%s mismatch: need %g",key,d);
	    } else {
		d = 1.;
	    }
	    (void) snprintf(key,3,"o%d",id%10u);
	    if (sf_histfloat(ins[0],key,&o) && 
		(!sf_histfloat(in,key,&oi) || 
		 (fabsf(oi-o) > tol*fabsf(d))))
		sf_warning("%s mismatch: need %g",key,o);
	}

	if (i >= nopen) sf_fileclose(in);
    } /* i */
}
