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

#ifndef __table_h
#define __table_h

#if defined (__cplusplus)
extern "C" {
#endif

#include "numeric.h"
#include "ascii.h"

typedef struct {
  int      n_xx;
  int      n_yy;
  double*   xx;
  double*   yy;
  double**  table;
} TABLE;

/* Error codes */
#define NOT_A_VALID_TABLE             -30
#define REQUESTED_VALUE_OUT_OF_RANGE  -31
#define ERROR_SPLINING                -32

/* prototypes */

int read_table (char *filename, TABLE **table);
void free_table (TABLE *table);
int table_calculate ( TABLE *table, double calc_xx, double calc_yy,
		      double *column);

#if defined (__cplusplus)
}
#endif

#endif
