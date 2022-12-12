/************************************************************************/
/* numeric.h                                                            */
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

#ifndef __numeric_h
#define __numeric_h

#define NaN -9999.99

#if defined (__cplusplus)
extern "C" {
#endif

#include "equation.h"    /* solve linear equation systems             */
#include "function.h"    /* some analytical and statistical functions */
#include "spl.h"         /* interpolating and approximating splines   */
#include "linear.h"      /* linear interpolation                      */
#include "cnv.h"         /* convolution                               */
#include "regress.h"     /* some linear and nonlinear regression      */
#include "integrat.h"    /* numerical integration                     */

#if defined (__cplusplus)
}
#endif

#endif









