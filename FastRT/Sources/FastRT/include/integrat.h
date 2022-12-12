/************************************************************************/
/* integrat.h                                                           */
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

#ifndef __integrat_h
#define __integrat_h

#if defined (__cplusplus)
extern "C" {
#endif

#include <numeric.h>

#define LIMITS_OUT_OF_RANGE      -1
#define FATAL_INTEGRATION_ERROR  -2


/* prototypes */

double integrate (double *x_int, double *y_int, int number);
int integrate_spline (double *x, double *y, int number,
		      double a, double b, double *integral);
int integrate_linear (double *x, double *y, int number,
		      double a, double b, double *integral);


#if defined (__cplusplus)
}
#endif

#endif
