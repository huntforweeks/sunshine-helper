/************************************************************************/
/* function.h                                                           */
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

#ifndef __function_h
#define __function_h

#if defined (__cplusplus)
extern "C" {
#endif

#include <numeric.h>


/* prototypes */

int double_equal (double a, double b);
void sort_long (long *x1, long *x2);
double fak (long n);
long over (long n, long m);
void average (long width, long *y, long n);
double mean (double *x, int n);
double weight_mean (double *x, double *sigma, int n);
double standard_deviation (double *x, int n);
double weight_standard_deviation (double *x, double *sigma, int n);


#if defined (__cplusplus)
}
#endif

#endif


