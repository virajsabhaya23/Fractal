// The MIT License (MIT)
// 
// Copyright (c) 2016 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include <pthread.h>
#include <stdio.h>
#include <stdint.h>

#define NUM_THREADS 100

struct parameters
{
  int thread_id;
  int x;
  int y;
};

void * run_me( void * arg )
{
  int i;

  int x;
  int y;

  struct parameters * params = (struct parameters *) arg;

  x = params -> x;
  y = params -> y;

  int begin = params->thread_id * 10000 / NUM_THREADS;
  int end   = ( begin   + 10000 / NUM_THREADS ) - 1;

  for( i = begin; i < end; i++ )
  {
    x = x + y;
  }

  return NULL;
}

int main()
{
  pthread_t tid[NUM_THREADS];
  struct parameters params[NUM_THREADS];
 
  int i;
  
  for( i = 0; i < NUM_THREADS; i++ )
  { 
    params[i].x = 10;
    params[i].y = 100;
    params[i].thread_id = i;

    pthread_create( &tid[i], NULL, run_me, (void *) &params[i] );
  }

  for( i = 0; i < NUM_THREADS; i++ )
  { 
    pthread_join( tid[i], NULL );
  }
};
