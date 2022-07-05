/************************************************************************/
/* integrat.c                                                           */
/*                                                                      */
/* Different integration methods.                                       */
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



/* prototypes of private functions */

double integrate_spline_intervall (double a0, double a1, double a2, double a3, 
				   double a, double b, double x_left);




/***************************************************/
/* integrate a function defined at discrete points */
/* x values must be sorted in ascending order      */
/* ( !!integrate does no checks!! )                */
/***************************************************/

double integrate (double *x_int, double *y_int, int number)
{
  int i=0;
  double sum=0;


  for (i=0; i<number-1; i++)  
    sum += ((y_int[i] + y_int[i+1]) / 2.0 * (x_int[i+1]-x_int[i]));
  
  return sum;
}





/*******************************************************************/
/* Integrate between two x-values a,b that lie between two         */
/* adjacent data points. a or b may coincide with the data points. */
/* Private function, used by integrate_spline()                    */
/*******************************************************************/

double integrate_spline_intervall (double a0, double a1, double a2, double a3, 
				   double a, double b, double x_left)
{
  double l0 = a;
  double l1 = (a-x_left)*(a-x_left);
  double l2 = l1*(a-x_left);
  double l3 = l2*(a-x_left);

  double r0 = b;
  double r1 = (b-x_left)*(b-x_left);
  double r2 = r1*(b-x_left);
  double r3 = r2*(b-x_left);

  double result=0;


  /* if zero interval */
  if (double_equal(a,b))  
    return 0;

    
  result = (a0*r0 + a1*r1/2.0 + a2*r2/3.0 + a3*r3/4.0) - 
    (a0*l0 + a1*l1/2.0 + a2*l2/3.0 + a3*l3/4.0);
  
  return result;
}
 



/******************************************************************/
/* Calculate integral \int y(x) dx  numerically between the       */
/* limits a and b by interpolating the data points (x[i], y[i])   */
/* with natural cubic splines.                                    */
/******************************************************************/

int integrate_spline (double *x, double *y, int number,
		      double a, double b, double *integral)
{
  double *a0=NULL, *a1=NULL, *a2=NULL, *a3=NULL;
  double temp=0;
  int status=0;
  int exchange=0;
  int i=0, i1=0, i2=0;

  *integral=0;
  
  /* check if nonzero interval */
  if (double_equal(a,b))  {
    *integral=0;
    return 0;
  }

  /* sort a,b */
  if (b<a)  {
    exchange=1;    /* set exchange flag to TRUE */
    temp=b;
    b=a;
    a=temp;
  }
  
  /* check worst case of 'OUT OF RANGE' */
  if (a>x[number-1] || b<x[0])
    return LIMITS_OUT_OF_RANGE;


  /* check for position of a */    
  if (double_equal(a, x[0]))                /* if a == left data limit        */
    i1=1;
  else  {
    if (double_equal(a,x[number-1]))  {     /* if a == right data limit       */
      *integral=0;
      return 0;
    }
    else  {
      if (a<x[0])                           /* if a left of left data limit   */
	return LIMITS_OUT_OF_RANGE;
      else  {                               /* if a between data limits       */
	while (x[i1]<a)  
	  i1++;
      }
    }
  }


  /* check for position of b */    
  if (double_equal(b, x[number-1]))         /* if b == right data limit       */
    i2=number-2;
  else  {
    if (double_equal(b,x[0]))  {            /* if b == left data limit        */
      *integral=0;
      return 0;
    }      
    else  {
      if (b>x[number-1])                    /* if a right of right data limit */
	return LIMITS_OUT_OF_RANGE;
      else  {                               /* if b between data limits       */
	while (x[i2]<b)  
	  i2++;
	i2--;
      }
    }
  }


  /* calculate spline coefficients */
  status = spline_coeffc (x, y, number, &a0, &a1, &a2, &a3);

  if (status!=0) 
    return SPLINE_NOT_POSSIBLE;


  if (i2<i1-1)  {
    free(a0);
    free(a1);
    free(a2);
    free(a3);
    return FATAL_INTEGRATION_ERROR;
  }


  /* do integration */
  if (i2==i1-1) 
    *integral = integrate_spline_intervall (a0[i2], a1[i2], a2[i2], a3[i2], a, b, x[i2]);
  else  {
    *integral = integrate_spline_intervall (a0[i1-1], a1[i1-1], a2[i1-1], a3[i1-1], 
					    a, x[i1], x[i1-1]);

    for (i=i1; i<i2; i++)  
      *integral += integrate_spline_intervall (a0[i], a1[i], a2[i], a3[i], 
					       x[i], x[i+1], x[i]);
    
    *integral += integrate_spline_intervall (a0[i2], a1[i2], a2[i2], a3[i2], 
					     x[i2], b, x[i2]);
  }


  if (exchange)  
    *integral = -(*integral);



  free(a0);
  free(a1);
  free(a2);
  free(a3);

  return 0;
}





/******************************************************************/
/* Calculate integral \int y(x) dx  numerically between the       */
/* limits a and b by interpolating the data points (x[i], y[i])   */
/* linearely.                                                     */
/*                                                                */
/* !!!! ATTENTION: NOT TESTED !!!!                                */      
/*                                                                */
/******************************************************************/

int integrate_linear (double *x, double *y, int number,
		      double a, double b, double *integral)
{
  double *a0=NULL, *a1=NULL, *a2=NULL, *a3=NULL;
  double temp=0;
  int status=0;
  int exchange=0;
  int i=0, i1=0, i2=0;

  *integral=0;
  
  /* check if nonzero interval */
  if (double_equal(a,b))  {
    *integral=0;
    return 0;
  }

  /* sort a,b */
  if (b<a)  {
    exchange=1;    /* set exchange flag to TRUE */
    temp=b;
    b=a;
    a=temp;
  }
  
  /* check worst case of 'OUT OF RANGE' */
  if (a>x[number-1] || b<x[0])
    return LIMITS_OUT_OF_RANGE;


  /* check for position of a */    
  if (double_equal(a, x[0]))                /* if a == left data limit        */
    i1=1;
  else  {
    if (double_equal(a,x[number-1]))  {     /* if a == right data limit       */
      *integral=0;
      return 0;
    }
    else  {
      if (a<x[0])                           /* if a left of left data limit   */
	return LIMITS_OUT_OF_RANGE;
      else  {                               /* if a between data limits       */
	while (x[i1]<a)  
	  i1++;
      }
    }
  }


  /* check for position of b */    
  if (double_equal(b, x[number-1]))         /* if b == right data limit       */
    i2=number-2;
  else  {
    if (double_equal(b,x[0]))  {            /* if b == left data limit        */
      *integral=0;
      return 0;
    }      
    else  {
      if (b>x[number-1])                    /* if a right of right data limit */
	return LIMITS_OUT_OF_RANGE;
      else  {                               /* if b between data limits       */
	while (x[i2]<b)  
	  i2++;
	i2--;
      }
    }
  }


  /* calculate spline coefficients */
  status = linear_coeffc (x, y, number, &a0, &a1, &a2, &a3);

  if (status!=0) 
    return SPLINE_NOT_POSSIBLE;


  if (i2<i1-1)  {
    free(a0);
    free(a1);
    free(a2);
    free(a3);
    return FATAL_INTEGRATION_ERROR;
  }


  /* do integration */
  if (i2==i1-1) 
    *integral = integrate_spline_intervall (a0[i2], a1[i2], a2[i2], a3[i2], a, b, x[i2]);
  else  {
    *integral = integrate_spline_intervall (a0[i1-1], a1[i1-1], a2[i1-1], a3[i1-1], 
					    a, x[i1], x[i1-1]);

    for (i=i1; i<i2; i++)  
      *integral += integrate_spline_intervall (a0[i], a1[i], a2[i], a3[i], 
					       x[i], x[i+1], x[i]);
    
    *integral += integrate_spline_intervall (a0[i2], a1[i2], a2[i2], a3[i2], 
					     x[i2], b, x[i2]);
  }


  if (exchange)  
    *integral = -(*integral);



  free(a0);
  free(a1);
  free(a2);
  free(a3);

  return 0;
}
