/* 
	Viraj Sabhaya
	UTA ID : 1001828871

	NOTE: ALL CAPITALIZE COMMENTS ARE THE CHANGES I MADE :)
*/

#include "bitmap.h"

#include <getopt.h>
#include <stdlib.h>
#include <sys/time.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

int iteration_to_color( int i, int max );
int iterations_at_point( double x, double y, int max );
void* compute_image(void* argg);

//CREATED A STRUCTURE OF ALL THE PARAMETERS OF COMPUTE_IMAGE TO ASSIGN THE ARGS TO EACH THREAD
//INCLUDES ALL THE PARAMETERS FROM COMPUTE_IMAGE
struct arguments_of_threads{
	struct bitmap *bm;
	double xmin;
	double xmax;
	double ymin;
	double ymax;
	double xmin;
	double xmax;
	int max;
};

void show_help()
{
	printf("Use: mandel [options]\n");
	printf("Where options are:\n");
	printf("-m <max>        The maximum number of iterations per point. (default=1000)\n");
	printf("-x <coord>      X coordinate of image center point. (default=0)\n");
	printf("-y <coord>      Y coordinate of image center point. (default=0)\n");
	printf("-s <scale>      Scale of the image in Mandlebrot coordinates. (default=4)\n");
	printf("-W <pixels>     Width of the image in pixels. (default=500)\n");
	printf("-H <pixels>     Height of the image in pixels. (default=500)\n");
	printf("-o <file>   	Set output file. (default=mandel.bmp)\n");
	printf("-n <#threads>   Number of threads. (default=1)");	//THE THREAD HELP LINE
	printf("-h              Show this help text.\n");
	printf("\nSome examples are:\n");
	printf("mandel -x -0.5 -y -0.5 -s 0.2\n");
	printf("mandel -x -.38 -y -.665 -s .05 -m 100\n");
	printf("mandel -x 0.286932 -y 0.014287 -s .0005 -m 1000\n\n");
}

int main( int argc, char *argv[] )
{
	char c;
	// These are the default configuration values used
	// if no command line arguments are given.

	const char *outfile = "mandel.bmp";
	double xcenter = 0;
	double ycenter = 0;
	double scale = 4;
	int    image_width = 500;
	int    image_height = 500;
	int    max = 1000;

	//DEFAULT VALUE OF THE THREAD ASSIGNED TO 1.
	int    NumOfThreads = 1;

	//DECLARATION OF THE TIME VARIABLES
	struct timeval begin_time;
  	struct timeval end_time;

	//STARING OF THE INITIAL TIME AND THE PROCESS BEGINS...
	gettimeofday( &begin_time, NULL );

	// For each command line argument given,
	// override the appropriate configuration value.

	while((c = getopt(argc,argv,"x:y:s:W:H:m:o:n:h"))!=-1) 
	{
		switch(c) {
			case 'x':
				xcenter = atof(optarg);
				break;
			case 'y':
				ycenter = atof(optarg);
				break;
			case 's':
				scale = atof(optarg);
				break;
			case 'W':
				image_width = atoi(optarg);
				break;
			case 'H':
				image_height = atoi(optarg);
				break;
			case 'm':
				max = atoi(optarg);
				break;
			case 'o':
				outfile = optarg;
				break;
			case 'n':
				NumOfThreads = atoi(optarg);
				break;
			case 'h':
				show_help();
				exit(1);
				break;
		}
	}

	// Display the configuration of the image.
	printf("mandel: x=%lf y=%lf scale=%lf max=%d outfile=%s NumThreads=%d\n", xcenter, ycenter, scale, max, outfile, NumOfThreads);

	// Create a bitmap of the appropriate size.
	struct bitmap *bm = bitmap_create(image_width,image_height);

	// Fill it with a dark blue, for debugging
	bitmap_reset(bm,MAKE_RGBA(0,0,255,0));

	//REMOVED THE COMPUTE_IMAGE FUNCTION BECAUSE IT IS CALLED UNDER THE PTHREAD_CREATE IN THE FOR LOOP BELOW... 
	// Compute the Mandelbrot image

	//INITIALIZE THE ARRAY OF THREADS.
	pthread_t ThreadsArray[NumOfThreads];

	//WHEREAS THIS INITIALIZE THE THREAD PARAMETERS INTO THE THREADS.
	struct arguments_of_threads argg[NumOfThreads];
	
	//DELARING INT I OUTSIDE FOR LOOP BECAUSE OF SEGMENATATION FAULT
	int i;
	//FOR LOOP FOR ASSIGNING THE VALUES INTO THE PARAMETERS AND IT ALSO CREATES THE THREAD WITH IT.
	for(int i = 0;i < NumOfThreads;i++)
	{
		argg[i].bm = bm;
		argg[i].xmin = xcenter - scale;
		argg[i].ymin = ycenter - scale;
		argg[i].xmax = xcenter + scale;
		argg[i].ymax = ycenter + scale;
		argg[i].max = max;
		
		//THIS USES THE PTHREAD API AND IS RESPONSIBLE FOR CREATING THREADS.
		pthread_create(&ThreadsArray[i], NULL, compute_image, (void*) &argg[i]);
	}

	//DELARING INT J OUTSIDE FOR LOOP BECAUSE OF SEGMENATATION FAULT
	int j;
	//THIS FOR LOOP BASICALLY ITERATES AND JOINS ALL THE THREAD RESULTS.
	for(j = 0; j < NumOfThreads; j++ )
	{ 
		pthread_join( &ThreadsArray[j], NULL );
	}

	// Save the image in the stated file.
	if(!bitmap_save(bm,outfile)) {
		fprintf(stderr,"mandel: couldn't write to %s: %s\n",outfile,strerror(errno));
		return 1;
	}

	gettimeofday( &end_time, NULL );

	long time_to_execute = ( end_time.tv_sec * 1000000 + end_time.tv_usec ) -
							( begin_time.tv_sec * 1000000 + begin_time.tv_usec );

	printf("This code took %ld microseconds to execute\n", time_to_execute);

	return 0;
}

/*
Compute an entire Mandelbrot image, writing each point to the given bitmap.
Scale the image to the range (xmin-xmax,ymin-ymax), limiting iterations to "max"
*/

void* compute_image(void* argg)		//REMOVED THE FOLLOWING PARAMETERS : // struct bitmap *bm, double xmin, double xmax, double ymin, double ymax, int max, int threads
{
	int i,j;
	
	struct arguments_of_threads * NewArgg = (struct arguments_of_threads*) argg;

	int width = bitmap_width(NewArgg->bm);
	int height = bitmap_height(NewArgg->bm);

	// For every pixel in the image...

	for(j=0;j<height;j++) {

		for(i=0;i<width;i++) {

			// Determine the point in x,y space for that pixel.
			double x = (NewArgg->xmin) + i*((NewArgg->xmax)-(NewArgg->xmin))/width;
			double y = (NewArgg->ymin) + j*((NewArgg->ymax)-(NewArgg->ymin))/height;

			// Compute the iterations at that point.
			int iters = iterations_at_point(x,y,(NewArgg->max));

			// Set the pixel in the bitmap.
			bitmap_set((NewArgg->bm),i,j,iters);
		}
	}
}

/*
Return the number of iterations at point x, y
in the Mandelbrot space, up to a maximum of max.
*/

int iterations_at_point( double x, double y, int max )
{
	double x0 = x;
	double y0 = y;

	int iter = 0;

	while( (x*x + y*y <= 4) && iter < max ) {

		double xt = x*x - y*y + x0;
		double yt = 2*x*y + y0;

		x = xt;
		y = yt;

		iter++;
	}

	return iteration_to_color(iter,max);
}

/*
Convert a iteration number to an RGBA color.
Here, we just scale to gray with a maximum of imax.
Modify this function to make more interesting colors.
*/

int iteration_to_color( int i, int max )
{
	int gray = 255*i/max;
	return MAKE_RGBA(gray,gray,gray,0);
}




