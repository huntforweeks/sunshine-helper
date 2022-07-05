/************************************************************************/
/* linear.h                                                             */
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

#ifndef __linear_h
#define __linear_h

#if defined (__cplusplus)
extern "C" {
#endif

#include <numeric.h>


/* prototypes */
int linear_coeffc (double *x, double *y, int number, 
		   double **a0, double **a1, double **a2, double **a3);

#if defined (__cplusplus)
}
#endif

#endif
