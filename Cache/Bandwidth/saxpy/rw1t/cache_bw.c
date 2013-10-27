#include <time.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <omp.h>
#include <malloc.h>

#define DB_LVL 1
#define DB(n,info) {if(n!=0) printf("DB INFO:: %s\n",info);}
#define SAVE_DATA(...) {FILE *fp=fopen("raw.dat", "a"); \
						fprintf(fp, __VA_ARGS__); \
						fclose(fp);}
#define DT double 


#define SEPERATOR "-----------------------------------------------------"



/**
	synchronization
**/
#define SYNC {__asm__ __volatile__ ("cpuid");}

/**
	timer
**/
double timer()
{
	long t_val = 0;
	struct timespec ts;
	clock_gettime(CLOCK_REALTIME,&ts);
	t_val = ts.tv_sec * 1e+9 + ts.tv_nsec;
	
	return (double)t_val;	
}


/**
	fill an array with random number
	@params A: pointer to the memory space
	@params n: the number of elements
	@params maxv: the maximum value
**/
void fill(DT * A, long n, DT maxv){
	long i;
    for (i = 0; i < n; i++)
    {
        A[i] = ((DT) maxv * (rand() / (RAND_MAX + 1.0f)));
    }
    return ;
}


/**
	benchmark entry
**/

int main(int argc, char** argv)
{
	/* variable declaration */
	DB(DB_LVL, "declaration");
	DT * memA_t0;
	DT * memB_t0;
	DT * memO_t0;  
	int reps, size;
	int samples;
	int tid;
	int i, p, r, bytes, elems;	
	int bytes_min, bytes_max;	
	int elems_min, elems_max;
	double func_overhead;
	double t_start, t_end;	
	double t_min, c_min;	
	DB(DB_LVL, SEPERATOR);
	
	/* initialization */
	DB(DB_LVL, "intialization");
	samples = 3;	
	bytes_min = 1024, bytes_max = 1024*32; /* [1KB, 8MB] */
	elems_min = bytes_min/sizeof(DT), elems_max = bytes_max/sizeof(DT); /* the number of elements */
	reps = 	40000;
	double alpha = 0.5;

	DB(DB_LVL, SEPERATOR);
	
	/* omp environment */
	omp_set_num_threads(1);

	/* iteration */
	DB(DB_LVL, "measurement");	
	for(elems=elems_min, bytes=bytes_min; elems<=elems_max; elems=elems+elems_min, bytes=bytes+bytes_min)
	{
		memA_t0 = (DT *)_mm_malloc(bytes, 64);
		memB_t0 = (DT *)_mm_malloc(bytes, 64);
		//mem_t1 = (DT *)_mm_malloc(bytes_max, 64);
		memO_t0 = (DT *)_mm_malloc(bytes, 64);
		//memO_t1 = (DT *)_mm_malloc(bytes_max, 64);
		/* initialization a local space */
		fill(memA_t0, elems, 10.0);
		fill(memB_t0, elems, 10.0);
		//fill(mem_t1, elems, 5.0);
		fill(memO_t0, elems, 0.0);
		//fill(memO_t1, elems, 0.0);
		
		/* measurement */
		t_min = 0.0f;
		c_min = 0.0f;
		DT ret_t0 = 0.0;		
		DT ret_t1 = 0.0;
		for(p=0; p<samples; p++)
		{			
			#pragma omp parallel shared(ret_t0, ret_t1, reps, elems) \
								private(tid)
			{

				//tid = omp_get_thread_num();
				//#pragma omp barrier 				
				//if((tid==0)&&(p==(samples-1)))
				{
					t_start = timer();						
				}			
				
				//if(tid==0)
				{
					for(r=0; r<reps; r++)
					{
						for(i=0; i<elems; i=i+1)
						{
							//ret_t0 += mem_t0[i];
							memO_t0[i] = alpha * memA_t0[i] + memB_t0[i];							
						}				
						memO_t0[0] = memO_t0[0] + 0.001; // to avoid overflow and optimizations
					}								
				}		
				//#pragma omp barrier 				
				//if((tid==0)&&(p==(samples-1)))
				{
					t_end = timer();			
				}
			}
		}	
		t_min = (t_end - t_start)/reps;		
		printf("%lf\n", memO_t0[0]);
		SAVE_DATA("%d\t%lf\n", bytes/1024, bytes/t_min)
		printf("cbw: %lf\t\n", bytes/t_min);
		if(memA_t0!=NULL) _mm_free(memA_t0);
		if(memB_t0!=NULL) _mm_free(memB_t0);
		//if(mem_t1!=NULL) _mm_free(mem_t1);
		if(memO_t0!=NULL) _mm_free(memO_t0);
		//if(memO_t1!=NULL) _mm_free(memO_t1);
	}		
	DB(DB_LVL, SEPERATOR);			
	
	/* post-process */
	DB(DB_LVL, "post-process");

	DB(DB_LVL, SEPERATOR);
}
