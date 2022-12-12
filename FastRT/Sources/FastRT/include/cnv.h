/************************************************************************/
/* cnv.h                                                                */
/*                                                                      */
/* Numerical Calculations, Convolution Routines.                        */
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

#ifndef __cnv_h
#define __cnv_h

#if defined (__cplusplus)
extern "C" {
#endif

#include <numeric.h>


#define SPEC_NOT_EQUIDISTANT   (-10)
#define CONV_NOT_EQUIDISTANT   (-11)
#define CONV_NOT_CENTERED      (-12)
#define SPEC_CONV_DIFFERENT    (-13)


/* prototypes */

int convolute (double *x_spec, double *y_spec, int spec_num,
	       double *x_conv, double *y_conv, int conv_num,
	       double **x_spec_conv, double **y_spec_conv, int *spec_conv_num);

int int_convolute (double *x_spec, double *y_spec, int spec_num,
		   double *x_conv, double *y_conv, int conv_num,
		   double **y_spec_conv);

#if defined (__cplusplus)
}
#endif

#endif





