/**
	@ measure the time of moving (<)32KB data into registers
	& calculate the transfer rates
	@ 27/10/2013 by Jianbin Fang from Delft University of Technology
	@ Reference: BenchITv6
**/

#include <stdio.h>
#include <malloc.h>
#include "../../../common/common.h"

void run(void * buffer, double * time){
	return ;
}

int main(int argc, char** argv){
	int size = 1024; // 1KB buffer
	void * buffer = _mm_malloc(size, ALIGN);
	
	// warm up (to move data into caches)
	
	// run test 

	// print results

	if(buffer!=NULL) _mm_free(buffer);	
	return 1;
}
