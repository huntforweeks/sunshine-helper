/*************************************************************************/
/* linear.c                                                              */
/*                                                                       */
/* Linear interpolation with an interface compatible to spl.c            */
/* (interpolation may be done with calc_spline_values()                  */
/*                                                                       */
/* Author:                                                               */
/*   Bernhard Mayer                                                @au@  */
/*   Fraunhofer Institute for Atmospheric Environmental Research,  @ad@  */
/*   82467 Garmisch-Partenkirchen,                                 @ad@  */
/*   Germany                                                       @ad@  */
/*                                                                       */
/* ----------------------------------------------------------------------*/
/* Copyright (C) 1995 Bernhard Mayer                                     */
/*                                                                       */
/* This program is free software; you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by  */
/* the Free Software Foundation; either version 1, or (at your option)   */
/* any later version.                                                    */
/*                                                                       */
/* This program is distributed in the hope that it will be useful,       */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of        */
/* MERCHANTABILITY of FITNESS FOR A PARTICULAR PURPOSE. See the          */
/* GNU General Public License for more details.                          */
/*                                                                       */
/* To obtain a copy of the GNU General Public License write to the       */
/* Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139,    */
/* USA.                                                                  */
/*-----------------------------------------------------------------------*/
/*************************************************************************/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "numeric.h"

/***********************************************************************************/
/* Function: linear_coeffc                                                @31_30i@ */
/* Description:                                                                    */
/*  Calculate coefficients for linear interpolation;                               */
/*  memory for coefficients will be allocated automatically!                       */
/*  These function has been created for compatibility with the spline              */
/*  interpolation functions; for this reason four coefficients are calculated,     */
/*  but a2[] and a3[] are set to zero. The interpolation may be done with          */
/*  calc_spline_values().                                                          */
/*                                                                                 */
/* Parameters:                                                                     */
/*  double *x:    x values of the data points, i=0...number-1                      */
/*  double *y:    y values of the data points, i=0...number-1                      */
/*  int number:   number of datapoints                                             */
/*  double **a0:  array of coefficients, i=0...number-1                            */ 
/*  double **a1:  array of coefficients, i=0...number-1                            */ 
/*  double **a2:  array of coefficients, i=0...number-1, set to zero               */ 
/*  double **a3:  array of coefficients, i=0...number-1, set to zero               */ 
/*                                                                                 */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                         */
/*                                                                        @i31_30@ */
/***********************************************************************************/

int linear_coeffc (double *x, double *y, int number, 
		   double **a0, double **a1, double **a2, double **a3)
{
  int i=0;


  /* check if x ascending */
  for (i=0; i<number-1; i++)  {
    if (x[i]>=x[i+1])
      return X_NOT_ASCENDING;
  }

  
  /* allocate memory for coefficients */
  *a0 = (double *) calloc (number, sizeof(double));
  *a1 = (double *) calloc (number, sizeof(double));
  *a2 = (double *) calloc (number, sizeof(double));
  *a3 = (double *) calloc (number, sizeof(double));


  /* calculate coefficients */
  for (i=0; i<number-1; i++)  {
    (*a0)[i] = y[i];
    (*a1)[i] = (y[i+1]-y[i]) / (x[i+1]-x[i]);
  }


  return 0;   /* if o.k. */
}
