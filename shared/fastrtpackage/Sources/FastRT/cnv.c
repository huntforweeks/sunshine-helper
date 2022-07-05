/*************************************************************************/
/* conv.c                                                                */
/*                                                                       */
/* Data convolution.                                                     */
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


/* @31c@ */
/* @c31@ */

/****************************************************************************/
/* In order to use the functions provided by the numeric library,  @31_10c@ */
/* #include <numeric.h> in your source code and link with numeric.a.        */
/*                                                                          */
/* @strong{Example:}                                                        */
/* Example for a source file:                                               */
/* @example                                                                 */
/*                                                                          */
/*   ...                                                                    */
/*   #include <numeric.h>                                                   */
/*   ...                                                                    */
/*                                                                          */
/* @end example                                                             */
/*                                                                          */
/* Linking of the executable, using the GNU compiler gcc:                   */
/* @example                                                                 */
/*                                                                          */
/*   gcc -o test test.c -lnumeric                                           */
/*                                                                          */
/* @end example                                                   @c31_10@  */
/****************************************************************************/


/****************************************************************************/
/* The NUMERIC library provides various numeric functions.         @31_20c@ */
/*                                                                 @c31_20@ */
/****************************************************************************/



#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#include "numeric.h"

/**************************************************************************/
/* convolute spec[] with conv[]; output is given in spec_conv[].          */
/* x_spec and x_conv must be equidistant (both same distance).            */
/**************************************************************************/

int convolute (double *x_spec, double *y_spec, int spec_num,
	       double *x_conv, double *y_conv, int conv_num,
	       double **x_spec_conv, double **y_spec_conv, int *spec_conv_num)
{

  double spec_delta=0, conv_delta=0;
  double sum=0;
  int i=0, index=0, spec_index=0;
  int mid=0;

  
  /* check for equidistant values */

  if (spec_num > 1)
    spec_delta = x_spec[1] - x_spec[0];

  for (i=2; i<spec_num; i++)  
    if (!double_equal(x_spec[i]-x_spec[i-1], spec_delta)) 
      return SPEC_NOT_EQUIDISTANT;

  if (conv_num > 1)
    conv_delta = x_conv[1] - x_conv[0];

  for (i=2; i<conv_num; i++)  
    if (!double_equal(x_conv[i]-x_conv[i-1], conv_delta)) 
      return CONV_NOT_EQUIDISTANT;

  if (!double_equal(conv_delta, spec_delta))  
    return (SPEC_CONV_DIFFERENT);




  /* look for center wavelength of convolution function */
  mid=0;
  while (x_conv[mid++] != 0.0)
    if (mid == conv_num) 
      return CONV_NOT_CENTERED;

  mid--;


  /* number of values */
  *spec_conv_num = spec_num;

  /* allocate memory for convoluted function */
  *x_spec_conv = (double *) calloc (*spec_conv_num, sizeof(double));
  *y_spec_conv = (double *) calloc (*spec_conv_num, sizeof(double));



  /* do convolution */

  for (i=0; i<spec_num; i++)  {
    sum=0.0;
    for (index=0; index<conv_num; index++)  {
      spec_index = i - mid + index;
      if (spec_index >= 0 && spec_index < spec_num)  {
	(*y_spec_conv)[i] += y_conv[index] * y_spec[spec_index];
	sum+=y_conv[index];
      }
    }

    if (sum != 0.0)
      (*y_spec_conv)[i] /= sum;
    else
      (*y_spec_conv)[i] = 0;
      
    (*x_spec_conv)[i] = x_spec[i];
  }


  return 0;
}



/***********************************************************************************/
/* Function: int_convolute                                                @31_30i@ */
/* Description:                                                                    */
/*  convolute spc[] with conv[]; output is set in spec_conv[].                     */
/*  x_conv[] must be given in equidistant steps.                                   */
/*  spc[] will be interpolated to the stepwidth of x_conv[]                        */
/*                                                                                 */
/* Parameters:                                                                     */
/*  double *x_spc:         x values of the data points, i=0...spc_num-1            */
/*  double *y_spc:         y values of the data points, i=0...spc_num-1            */
/*  int spc_num:           number of data points                                   */
/*  double *x_conv:        x values of the convolution function, i=0...conv_num-1  */
/*  double *y_conv:        y values of the convolution function, i=0...conv_num-1  */
/*  int conv_num:          number of convolution function data points              */
/*  double **y_spec_conv:  convoluted spectrum on the original grid x_spc,         */
/*                         i=0...spc_num-1                                         */
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

int int_convolute (double *x_spc,  double *y_spc,  int spc_num,
		   double *x_conv, double *y_conv, int conv_num,
		   double **y_spec_conv)
{

  double interval=0, ratio=0;
  double *x_spec=NULL, *y_spec=NULL; 
  double *x_spec_tmp=NULL, *y_spec_tmp=NULL;
  int spec_tmp_num=0, spec_num=0;

  int i=0, index=0, spec_index=0, status=0;
  int mid=0;

  double *a0=NULL, *a1=NULL, *a2=NULL, *a3=NULL;

  double sum=0, stepwidth=0;

  /* check for equidistant values */
  if (conv_num > 1)
    stepwidth = x_conv[1] - x_conv[0];

  for (i=2; i<conv_num; i++)  
    if (!double_equal(x_conv[i]-x_conv[i-1], stepwidth)) 
      return CONV_NOT_EQUIDISTANT;

  /* interpolate spectrum to stepwidth of convolution function */

  /* calculate LINEAR interpolation coefficients */
  status = linear_coeffc (x_spc, y_spc, spc_num, &a0,  &a1,  &a2,  &a3);
  if (status!=0)  {
    fprintf (stderr, " ... int_convolute(): error calculating interpolation coefficients\n");
    return status;
  }
  
  /* calculate number of interpolated values */
  interval = x_spc[spc_num-1] - x_spc[0];
  ratio    = interval/stepwidth;
  
  spec_num = (int) ceil(ratio) + 1;

  /* allocate memory for interpolated spectrum */
  x_spec  = calloc (spec_num, sizeof(double));
  y_spec  = calloc (spec_num, sizeof(double));

  
  /* calculate interpolated values */
  for (i=0; i<spec_num; i++)  {
    x_spec[i] = x_spc[0] + (double) i * stepwidth;

    status = calc_splined_value (x_spec[i], &(y_spec[i]), 
				 x_spc, spc_num, 
				 a0, a1, a2, a3);

    if (status!=0)  {
      if (i==spec_num-1)     /* if last data point */
	y_spec[i] = y_spec[i-1];
      else  {
	fprintf (stderr, " ... int_convolute(): error interpolating spectrum at %g\n", x_spec[i]);
	return status;
      }
    }
  }
   
  /* free interpolation coefficients */
  free(a0);
  free(a1);
  free(a2);
  free(a3);
  


  /* look for center wavelength of convolution function */
  mid=0;
  while (x_conv[mid++] != 0)
    if (mid == conv_num) 
      return CONV_NOT_CENTERED;

  mid--;


  /* number of values */
  spec_tmp_num = spec_num;

  /* allocate memory for convoluted function */
  x_spec_tmp = (double *) calloc (spec_tmp_num, sizeof(double));
  y_spec_tmp = (double *) calloc (spec_tmp_num, sizeof(double));



  /* do convolution */

  for (i=0; i<spec_num; i++)  {
    sum=0.0;
    for (index=0; index<conv_num; index++)  {
      spec_index = i - mid + index;
      if (spec_index >= 0 && spec_index < spec_num)  {
	y_spec_tmp[i] += y_conv[index] * y_spec[spec_index];
	sum+=y_conv[index];
      }
    }

    if (sum != 0.0)
      y_spec_tmp[i] /= sum;
    else
      y_spec_tmp[i] = 0;
      
    x_spec_tmp[i] = x_spec[i];
  }


  /* now again interpolate convoluted spectra to original steps */

  /* calculate LINEAR interpolation coefficients */
  status = linear_coeffc (x_spec_tmp, y_spec_tmp, spec_tmp_num, &a0,  &a1,  &a2,  &a3);
  if (status!=0)  {
    fprintf (stderr, " ... int_convolute(): error calculating interpolation coefficients\n");
    return status;
  }
  

  /* allocate memory for convoluted function */
  *y_spec_conv = (double *) calloc (spc_num, sizeof(double));

  for (i=0; i<spc_num; i++)  {

    status = calc_splined_value (x_spc[i], &((*y_spec_conv)[i]), 
				 x_spec_tmp, spec_tmp_num, 
				 a0, a1, a2, a3);
    
    if (status!=0)  {
      fprintf (stderr, " ... int_convolute(): error interpolating spectrum\n");
      return status;
    }
  }
    
  free(a0);
  free(a1);
  free(a2);
  free(a3);
  free(x_spec);
  free(y_spec);
  free(x_spec_tmp);
  free(y_spec_tmp);
  
  return 0;
}
