/************************************************************************/
/* function.c                                                           */
/*                                                                      */
/* Some useful (or not so useful) stuff.                                */
/*                                                                      */
/* Author: Bernhard Mayer,                                              */
/*        Fraunhofer Institute for Atmospheric Environmental Research,  */
/*        82467 Garmisch-Partenkirchen,                                 */
/*        Germany                                                       */
/*                                                                      */
/*----------------------------------------------------------------------*/
/* Copyright (C) 1995 Bernhard Mayer                                    */
/*                                                                      */
/* This program is free software; you can redistribute it and/or modify */
/* it under the terms of the GNU General Public License as published by */
/* the Free Software Foundation; either version 1, or (at your option)  */
/* any later version.                                                   */
/*                                                                      */
/* This program is distributed in the hope that it will be useful,      */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of       */
/* MERCHANTABILITY of FITNESS FOR A PARTICULAR PURPOSE. See the         */
/* GNU General Public License for more details.                         */
/*                                                                      */
/* To obtain a copy of the GNU General Public License write to the      */
/* Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,   */
/* USA.                                                                 */
/*----------------------------------------------------------------------*/
/************************************************************************/



#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "numeric.h"



/* local definitions */
#define DOUBLE_RELATIVE_ERROR 1e-10


/************************************************************************/
/* compare two float values; returns 0, if the relative difference is   */
/* bigger than DOUBLE_RELATIVE_ERROR                                    */
/************************************************************************/

int double_equal (double a, double b)  
{
  double diff=0, temp=0;

  /* return 1, if a==b */
  if ((diff=fabs(a-b)) == 0.0)
    return 1;


  /* if one of both == 0, the relative difference cannot be calculated */
  if (a==0 || b==0) 
    return 0;

  a = fabs(a);
  b = fabs(b);

  /* sort a,b */
  if (a>b)  {
    temp=a;
    a=b;
    b=temp;
  }

  if ( diff/a < DOUBLE_RELATIVE_ERROR)
    return 1;

  return 0;
}




/**************************/
/* sort two long integers */
/**************************/

void sort_long (long *x1, long *x2)
{
  long dummy=0;
  
  if (*x1>*x2)  {
    dummy = *x2;
    *x2 = *x1;
    *x1 = dummy;
  }
}


/********************************/
/* faculty of an integer number */
/********************************/

double fak (long n)
{
  double dummy=0;
  
  if (n<=0) 
    return 1;
  else  {
    dummy = (double) n * fak(n-1);
    return dummy;
  }
}


/***********************/
/* calculates n over m */
/***********************/

long over (long n, long m)  
{
  long dummy = (long) ( fak (n) / fak (m) / fak (n-m) + 0.5);
  return dummy;
}




/************************************************************/ 
/* average a data set of n points with a gaussian function  */
/* of base width "width"                                    */
/************************************************************/ 

void average (long width, long *y, long n)
{
  long i=0, j=0;
  long counter=0;
  double sum=0;
  double *weight = (double *) calloc (width+1, sizeof(double));
  double *new_y  = (double *) calloc (n      , sizeof(double));
  
  for (i=0; i<n; i++) 
    new_y [i] = 0;
  
  for (i=0; i<=width; i++)  
    weight [i] = (double) over (width, i);
  
  
  for (i=0; i<n; i++)  {
    sum=0;
    for (j=0; j<=width; j++)  {
      counter = i-width/2+j;
      if (counter >= 0 && counter < n)  {  
	new_y[i] += ( (double) y[counter] * weight[j]);
	sum += weight[j];     
      }
    }  
    new_y[i] /= sum;  
  }
  
  for (i=0; i<n; i++)  
    y[i] = (long) new_y[i];

  free(weight);
  free(new_y);
}


/***********************************/
/* calculate mean of n data points */
/***********************************/

double mean (double *x, int n)
{
  int i=0;
  double tmp=0;
  
  for (i=0; i<n; i++)
    tmp += x[i];
  
  tmp /= n;
  return tmp;
}


/********************************************/
/* calculate WEIGHTED mean of n data points */
/********************************************/

double weight_mean (double *x, double *sigma, int n)
{
  int i=0;
  double tmp=0;
  double numerator=0;
  
  for (i=0; i<n; i++)   {
    tmp += x[i] / (sigma[i]*sigma[i]);
    numerator += 1/(sigma[i]*sigma[i]);
  }
  
  tmp /= numerator;
  return tmp;
}


/*************************************************/
/* calculate standard deviation of n data points */
/*************************************************/

double standard_deviation (double *x, int n)
{
  int i=0;
  double tmp=0;
  
  double mu = mean (x, n);
  
  for (i=0; i<n; i++)
    tmp += ( (x[i] - mu) * (x[i] - mu) );
  
  tmp /= (float) (n-1);
  tmp = sqrt(tmp);
  
  return tmp;
}


/**********************************************************/
/* calculate weighted standard deviation of n data points */
/**********************************************************/

double weight_standard_deviation (double *x, double *sigma, int n)
{
  int i=0;
  double tmp=0;
  double numerator=0;
  
  double mu = weight_mean (x, sigma, n);
  
  for (i=0; i<n; i++)  {
    tmp += ( (x[i] - mu) * (x[i] - mu) / (sigma[i]*sigma[i]) );
    numerator += (1 / (sigma[i] * sigma[i]) );
  }
  
  tmp *= ( (float) n / (float) (n-1));
  tmp /= numerator;
  
  tmp = sqrt(tmp);
  
  return tmp;
}
