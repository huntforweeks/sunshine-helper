/************************************************************************/
/* regress.h                                                            */
/*                                                                      */
/* Numerical Calculations.                                              */
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

#ifndef __regress_h
#define __regress_h

#if defined (__cplusplus)
extern "C" {
#endif

#include <numeric.h>


#define FIT_NOT_POSSIBLE -30


/* prototypes */

double gauss             (double x, double mu, double sigma);
double gauss_distorted   (double x, double a0, double a1, double a2, double a3);

int regression        (double *x, double *y, int n, 
		       double *alpha, double *beta,
		       double *sigma_a, double *sigma_b,
		       double *correlation);
double weight_regression (double *x, double *y, double *sigma, int n,
                          double *alpha, double *beta,
                          double *sigma_a, double *sigma_b);
int gaussfit             (double *x, double *y, long number,
			  double *mu, double *sigma, double *area);
int gaussfit_distorted   (double *x, double *y, long number,
			  double *a0, double *a1, double *a2, double *a3);
int boltzmannfit         (double *x, double *y, long number, 
			  double *a, double *b);
int exponentialfit       (double *x, double *y, long number, 
			  double *a, double *b);
int parabolafit          (double *x, double *y, long number, 
			  double *a0, double *a1, double *a2);
int cubicfit             (double *x, double *y, long number, 
			  double *a0, double *a1, double *a2, double *a3);
int hyperbolafit         (double *x, double *y, long number, 
			  double *a, double *b);
int inv_parabolafit      (double *x, double *y, long number, 
			  double *a0, double *a1, double *a2);


#if defined (__cplusplus)
}
#endif

#endif

