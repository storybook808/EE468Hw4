/* 
 * This is an implementation of merge sort assuming the
 * data is an array a[] of length N, and N is a power of 2.
 *
 * The main() program uses a function called merge() which
 * merges subarrays in sorted order.  Rewrite the program
 * so that it uses threads to do the merging of the
 * subarrays.  In particular, each call to merge() creates
 * a thread.  
 *
 * The merging is done in a for-loop.  Each
 * pass through the for-loop causes subarrays of the same size
 * to be merged together.  The mergings within a pass should 
 * be done in parallel using one thread per merge. However,
 * the main program must wait for all the threads to complete
 * before doing another pass of the for-loop.  Thus, mergings
 * of subarrays of size 1 can be done in parallel, then
 * mergings of subarrays of size 2 can be done in parallel and
 * so forth.
 *
 * Use the pthread API
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

//private definitions
#define N 	   16
#define THREAD_MAX 50
//private structures
typedef struct
{
	int i;
	int arrsize;
}Parameter;
//private global variables
int a[N];     /* Array to sort */
int temp[N];  /* Temporary storage */
//private prototypes
void genvalues(int n); /* Initializes array a[] */
void prnvalues(int n); /* Prints array a[] */
void *merge(void *context); /* Merges subarrays */

main() 
{
	int arrsize;    /* Size of subarrays to merge */
	int numarr;     /* Number of subarrays */
	int newarrsize;  /* New subarray size */
	int newnumarr;   /* New number of subarrays */
	int i;

	//variables added for this assignment
	int       thread_count;
	Parameter A[THREAD_MAX];
	pthread_t thread_index[THREAD_MAX]; //index to manage other threads

	//initialize a[] with random values
	genvalues(N);
	printf("Initial values:\n");
	//display the values
	prnvalues(N);
	//initialize values
	numarr = 2;
	arrsize = 1;
	while(numarr > 1)
	{
		//reset thread count
		thread_count = 0;
		//merge subarrays to double subarray size
		arrsize= 2*arrsize;
		numarr = N/arrsize;
		for(i = 0; i < N; i += arrsize)
		{
			//transfer parameters into a structure
			A[thread_count].i = i;
			A[thread_count].arrsize = arrsize;
			//create a new thread to process the merge function
			if(pthread_create(&thread_index[thread_count], NULL, \
			   (void *)&merge, (void *)&A[thread_count]))
			{
				//failed to create a new thread
				printf("error: creating thread\n");
				//quit the program
				return;
			}
			else
			{
				printf("Thread %d created\n", thread_count + 1);
				//increment the number of threads created
				thread_count = thread_count + 1;
			}
		}
		//wait for threads to finish their assignments
		for(i = 0; i < thread_count; i++)
		{
			printf("Waiting for thread %d\n", i + 1);
			if(pthread_join(thread_index[i], NULL))
			{
				//failed to join the threads
				printf("error: joining thread\n");
				//quit the program
				return;
			}
		}
	}
	printf("\nOutput:\n");
	//display the sorted values
	prnvalues(N);
}

/*
 * Merges subarrays (a[first], ..., a[midpoint-1])
 *    and (a[midpoint],..., a[last-1]) into
 *    subarray temp[i],..., temp[last-1] in increasing order.
 *    Then it copies temp[first],..., temp[last-1] back into
 *    a[i],..., a[last-1].
 */
void *merge(void *context)
{
	int leftptr;
	int rightptr;
	int midpoint;
	int last;
	int k;
	int delay;
	
	//pass parameter address to structure in this scope
	Parameter *A =  context;
	/* 
	 * Do not delete the next three lines.  They cause the function to
	 * delay by a amount that is proportional to the subarray it is merging
	 */
	if(A->arrsize > 0) delay = A->arrsize;
	else delay = 1;
	usleep(delay*250000);

	midpoint= A->i + (A->arrsize/2);
	last = A->i + A->arrsize;
	leftptr = A->i;
	rightptr = midpoint; 

	for(k=A->i; k<last; k++)
	{
		if (leftptr >= midpoint) temp[k] = a[rightptr++];
		else if (rightptr >= last) temp[k] = a[leftptr++];
		else if (a[leftptr] < a[rightptr]) temp[k] = a[leftptr++];
		else if (a[leftptr] >= a[rightptr]) temp[k] = a[rightptr++];
		else printf("There's a bug \n");
	}

	for(k=A->i; k<last; k++) a[k] = temp[k];
	return;
}



/*
 * Initializes array a[] with random values.
 */
void genvalues(int n)
{
	int k;
	int i;
	int current; 

	k = 2*n;
	current = 0;
	for(i=0; i<n; i++)
	{
		current = (current*73 + 19)%k;
		a[i] = current;
	}
}

/*
 * Prints the values in the array a[]
 */
void prnvalues(int n)
{
	int i;
	for(i=0; i<n; i++)
	{
		printf(" %d ", a[i]);
		if((i+1)%10 == 0) printf("\n");
	}
	printf("\n");
}


