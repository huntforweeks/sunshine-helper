/************************************************************************/
/* spl.h                                                                */
/*                                                                      */
/* Numerical Calculations, Spline routines.                             */
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

#ifndef __spl_h
#define __spl_h

#if defined (__cplusplus)
extern "C" {
#endif


#include <numeric.h>


#define NO_SPLINED_VALUES           -1
#define X_NOT_ASCENDING             -2
#define SPLINE_NOT_POSSIBLE         -3
#define TOO_FEW_DATA_POINTS         -4
#define DATA_NOT_SORTED             -5
#define NEGATIVE_WEIGHTING_FACTORS  -6
#define NO_EXTRAPOLATION            -7



/* prototypes */
int spline (double *x, double *y, int number, double start, double step,
	    int *newnumber, double **new_x, double **new_y);
int spline_coeffc (double *x, double *y, int number, 
		   double **a0, double **a1, double **a2, double **a3);
int appspl (double *x, double *y, double *w, int number, 
	    double start, double step, 
	    int *newnumber, double **new_x, double **new_y);
int appspl_coeffc (double *x, double *y, double *w, int number, 
		   double **a0, double **a1, double **a2, double **a3);
int calc_splined_value (double xnew, double *ynew, 
			double *x, int number, 
			double *a0, double *a1, double *a2, double *a3);
int linear_eqd (double *x, double *y, int number, double start, double step,
		int *newnumber, double **new_x, double **new_y);


#if defined (__cplusplus)
}
#endif

#endif
