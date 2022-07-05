/************************************************************************/
/* fastrt.c                                                             */
/*                                                                      */
/* Simulation of clear sky surface irradiance at user specified UV      */
/* wavelengths                                                          */
/*                                                                      */
/* Author: Ola Engelsen,                                                */
/*        Norwegian Institute for Air Research (NILU)                   */
/*        N-9296 Tromso                                                 */
/*        Norway                                                        */
/*        Email: ola.engelsen@nilu.no                                   */
/*                                                                      */
/*----------------------------------------------------------------------*/
/* Copyright (C) 2005 Ola Engelsen                                      */
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

#include "fastrt_.h"
#include "ascii.h"
#include "numeric.h"

#define DELTA_SZA 3.
#define DELTA_O3 20.
#define DELTA_ALT 3.
#define FWHM_DEFAULT 0.6
#define SOLAR_FLUX_RESOLUTION 0.05
#define ALBEDO_RESOLUTION 10.
#define CLOUD_THICKNESS 5.

static void print_usage()
{
  fprintf (stderr, "\n");
  fprintf (stderr, "\nComputes surface irradiances ");
  fprintf (stderr, "in mW/(m^2 nm)\n");
  fprintf (stderr, "Usage: fastrt [-havbcutozpqlfrwgesxd]\n");
  fprintf (stderr, " -h prints this message.\n");
  fprintf (stderr, " -a solar zenith angle ([0,90] degrees) REQUIRED\n");
  fprintf (stderr, " -v visibility ([5,350]km) (no effect if Angstrom beta is set or clouds present)\n");
  fprintf (stderr, " -b Aerosol Angstrom beta ([0,0.4]) (0.02 default)(no effect if clouds present)\n");
  fprintf (stderr, " -c radiation is trapped by broken clouds (set no parameter)\n");
  fprintf (stderr, " -u Cloud liquid water column (g m-2)  ([0,5000]) (0 default)\n");
  fprintf (stderr, " -t Cloud optical depth at 360 nm ([0,1083], no effect if cloud liquid water content is set)\n");
  fprintf (stderr, " -o ozone column ([100,600] DU) REQUIRED\n");
  fprintf (stderr, " -z surface altitude ([0,6] KM) (0.0km default\n");
  fprintf (stderr, " -p surface albedo ([0,1]) (0.0 default, q and l option ignored\n");
  fprintf (stderr, " -q surface ID (snow5cm=0;snow2cm=1;sand=2;lawn=3;cdry=4;cwet=5;gras=6;beet=7;oat=8;loam=9;rye=10;stub=11;snow_new=12;snow_old=13;sand_dry=14;limestone=15;meadow=16;field_dry=17) (l option ignored)\n");
  fprintf (stderr, " -l file containing the spectral surface albedo (OPTIONAL)\n");
  fprintf (stderr, "    1st column: ascending wavelengths in nanometers [290,400] at %f intervals\n", ALBEDO_RESOLUTION);
  fprintf (stderr, "    2nd column: surface albedo\n");
  fprintf (stderr, " -f FWHM in nanometers ([0.05,55] nm)(0.6nm default) of triangular spectral response function \n");
  fprintf (stderr, " -r file containing the spectral response function (OPTIONAL)\n");
  fprintf (stderr, "    1st column: wavelengths in nm, absolute or relative to the central\n");
  fprintf (stderr, "    2nd column: spectral response\n");
  fprintf (stderr, "\n");
  fprintf (stderr, "and one of the following required options:\n");
  fprintf (stderr, "\n");
  fprintf (stderr, "OPTION 1\n");
  fprintf (stderr, " -w wavelength ([290,405] nm) \n");
  fprintf (stderr, "\n");
  fprintf (stderr, "OPTION 2\n");
  fprintf (stderr, " -g start_wavelength ([290,405] nm)\n");
  fprintf (stderr, " -e end_wavelength ([290,405] nm)\n");
  fprintf (stderr, " -s step_wavelength (nm)\n");
  fprintf (stderr, "\n");
  fprintf (stderr, "OPTION 3\n");
  fprintf (stderr, "-x lambdafile (file with a column of wavelengths in nm)\n");
  fprintf (stderr, " -d day of year (average sun-earth distance default)\n");
  fprintf (stderr, "\n");
  fprintf (stderr, "Examples:\n");
  fprintf (stderr, "./fastrt -a 57.5 -o 315. -w 300.15 -d 80 -f 0.6 -z 0.73\n");
  fprintf (stderr, "./fastrt -a 57.5 -v 29. -t 10.7 -o 430. -z 0.7 -q 0 -g 290.5 -e 399.5 -s 0.5 -f 0.6 \n");
  fprintf (stderr, "./fastrt -a 57.5 -b 0.2 -o 430. -z 0.0 -g 305 -e 306 -s 0.05 -f 0.05 \n");
  fprintf (stderr, "./fastrt -a 57.5 -o 315. -x lambdafile\n");
  fprintf (stderr, "\n");
  fprintf (stderr, "Output:\n");
  fprintf (stderr, "wavelength(nm) irradiance (mW/(m^2 nm))\n");
  fprintf (stderr, "\n");
}

void free_splinecoef_results(int status, void* a0, void* a1, void* a2, void* a3)
{
  if (status == 0)
  {
    free(a0);
    free(a1);
    free(a2);
    free(a3);
  }
}

int make_slitfunction(double fwhm, double **sr_lambda, double **sr, int *sr_nlambda)
{
  double resolution=SOLAR_FLUX_RESOLUTION;
  double lambda_range, sr_step;
  int i, j, n_points=0;
  
  lambda_range = 2*fwhm;
  
  n_points     = (int) (lambda_range/resolution+0.5) + 1;
  sr_step    = 1./(n_points/2);
  
  *sr_lambda = (double *) calloc(n_points, sizeof(double));
  *sr = (double *) calloc(n_points, sizeof(double));
  
  j=-1;
  for (i=0;i<n_points/2;i++) {
    j++;
    (*sr_lambda)[j] = -lambda_range/2. + i*resolution;
    (*sr)[j]   = (double) i * sr_step;
  }
  for (i=0;i<=n_points/2;i++) {
    j++;
    (*sr_lambda)[j] = (double) i * resolution;
    (*sr)[j]   = (double)(n_points/2 - i) * sr_step;
  }
  *sr_nlambda=n_points;
  return 0;
}

int read_slitfunction(char *filename, double **sr_lambda, double **sr, int *rows)
     /* reads in the slit function from a file */
{
  int status=0;
  int max_columns=0, min_columns=0;
  double **data=NULL;
  
  /* read files */
  status = ASCII_file2double (filename, rows,
   &max_columns, &min_columns, &data);
  if (status!=0) {
    fprintf (stderr, "ERROR: cannot read slitfunction file\n");
    exit(0);
  }
  
  if (max_columns!=min_columns) {
    fprintf (stderr, " !! ATTENTION !! Inconsistent number of columns\n");
    fprintf (stderr, "     min = %d, max =%d\n", min_columns, max_columns);
    exit(0);
  }
  if (min_columns<2) {
    fprintf (stderr, " ... ending, too few columns\n");
    exit(0);
  }
  
  *sr_lambda = ASCII_column (data, *rows, 0);
  *sr = ASCII_column (data, *rows, 1);
  
  ASCII_free_double (data, *rows);
  return 0;
}

int check_spectral_response_function(double *sr_lambda, double *sr, int sr_nlambda)
{
  int mid, i;
  double conv_delta=0.0, sum, sum0, central_lambda;
  
  /* check for equidistant relative wavelengths in slitfunction */
  if (sr_nlambda >=3) {
    conv_delta = sr_lambda[1] - sr_lambda[0];
    
    for (i=2; i<sr_nlambda; i++)
      if (!double_equal(sr_lambda[i]-sr_lambda[i-1], conv_delta)) {
       fprintf (stderr, " ... wavelengths in slitfunction are not equidistant!\n");
       fprintf (stderr, " ... FWHM must be multiple of %f nm\n", SOLAR_FLUX_RESOLUTION);
       exit(0);
     }
   }
   else {
    fprintf (stderr, "... ERROR: less than 3 slitfunction elements.\n");
    fprintf (stderr, "    For a Kronecker delta slitfunction generate a \n");
    fprintf (stderr, "    triangular slitfunction of %f nm FWHM\n", SOLAR_FLUX_RESOLUTION);
    exit(0);
  }
  
  if (sr_lambda[0] > 250) { /* absolute wavelengths provided, convert to relative wavelengths */
    /*find central wavelength */
  sum=0.;
  sum0=0.;
  for (i=0; i<sr_nlambda; i++){
    sum +=sr[i]*sr_lambda[i];
    sum0+=sr[i];
  }
  central_lambda=SOLAR_FLUX_RESOLUTION * (int)(sum/sum0/SOLAR_FLUX_RESOLUTION+0.5);
    /* convert to relative wavelengths */
  for (i=0; i<sr_nlambda; i++){
    sr_lambda[i]=sr_lambda[i]-central_lambda;
  }
}
  /* look for center wavelength of convolution function */
  /*  mid=0;
      while (sr_lambda[mid++] != 0.0)
      if (mid == sr_nlambda) {
      fprintf (stderr, " ... no center defined in slitfunction\n");
      exit(0);
      }
      mid--;*/

return 0;
}

/* #include "do_spectra.c"*/
double *do_spectra(char *filename, double *lambda, int n_lambda,
 double *sr_lambda, double *sr, int sr_nlambda, double *solirr)
     /* reads data of adjacent data from files and interpolates to the
    desired wavelengths */
{
  int rows_lambda=0, rows_data=0;
  int status=0;
  int i=0, k=0, m=0, index;
  double *x=NULL, *y=NULL;
  double *a0=NULL, *a1=NULL, *a2=NULL, *a3=NULL;
  
  int max_columns=0, min_columns=0;
  double **rawlambda=NULL, **data=NULL;
  double irr=0., sr_sum=0., lam;
  
  double ynew=0, *global_irradiance=NULL;
  
  
  global_irradiance = calloc (n_lambda, sizeof(double));
  
  /* read wavelength file for the transmittance file*/
  status = ASCII_file2double ("./TransmittancesCloudH2O0.000/rawlambdafile", &rows_lambda,
   &max_columns, &min_columns, &rawlambda);
  
  if (status!=0) {
    fprintf (stderr, "ERROR: cannot read rawlambdafile\n");
    exit(0);
  }
  
  if (max_columns!=min_columns) {
    fprintf (stderr, " !! ATTENTION !! Inconsistent number of columns\n");
    fprintf (stderr, "     min = %d, max =%d\n", min_columns, max_columns);
    exit(0);
  }
  
  if (min_columns != 1) {
    fprintf (stderr, " Error, wavelength file does not contain a single column\n");
    exit(0);
  }
  
  /* read transmittance file */
  status = ASCII_file2double (filename, &rows_data,
   &max_columns, &min_columns, &data);
  
  if (status!=0) {
    /* run error loop */
    for (i=0; i<n_lambda; i++)  {
      global_irradiance[i] = NaN;
    }
    return global_irradiance;
  }
  
  if (max_columns!=min_columns) {
    fprintf (stderr, " !! ATTENTION !! Inconsistent number of columns\n");
    fprintf (stderr, "     min = %d, max =%d\n", min_columns, max_columns);
    exit(0);
  }
  
  if (min_columns<1) {
    fprintf (stderr, " ... ending, too few columns\n");
    exit(0);
  }
  
  if (rows_lambda != rows_data) {
    fprintf (stderr, " ... Error, the rawlambdafile and a datafile\n");
    fprintf (stderr, "are mutually incompatible, unequal number of rows\n");
    fprintf (stderr, "rows_lambda = %d\n", rows_lambda);
    fprintf (stderr, "rows_data = %d\n", rows_data);
    exit(0);
  }
  
  /* interpolate to output wavelengths */
  x = ASCII_column (rawlambda, rows_lambda, 0);
  for (k=0; k<1; k++) {
    y = ASCII_column (data, rows_data, k);
    
    /* calculate interpolating spline coefficients */
    /* fprintf (stderr, " ... spline interpolation\n");*/
    status = spline_coeffc (x, y, rows_data, &a0, &a1, &a2, &a3);
    if (status!=0)  {
      fprintf (stderr, "sorry cannot do spline interpolation\n");
      fprintf (stderr, "spline_coeffc() returned status %d\n", status);
      fprintf (stderr, "Wavelength beyond prespecified range?\n");
      exit(status);
    }

    
    /* convolve with slitfunction stored in sr. Relative wavelengths stored in sr_lambda */
    for (i=0; i<n_lambda; i++)  {
      irr = 0.;
      sr_sum=0.;
      for (m=0;m<sr_nlambda;m++) {
       lam = lambda[i] + sr_lambda[m];
       status = calc_splined_value (lam, &ynew, x, rows_data, a0, a1, a2, a3);
       index = (int)((lam - 280.)/ SOLAR_FLUX_RESOLUTION + 0.5);
       
       irr += ynew * sr[m] * solirr[index];
       sr_sum += sr[m];
     }
     if (sr_sum != 0.0)
       irr /= sr_sum;
     else
       irr = 0.;
     
      /* copy data to result array */
     if (status==0) {
       global_irradiance[i] = irr;
     }
     else {
       global_irradiance[i] = NaN;
     }
   }
   free_splinecoef_results(status, a0, a1, a2, a3);
   free(y);
 }
 free(x);
 ASCII_free_double (data, rows_data);
 ASCII_free_double (rawlambda, rows_lambda);
 
 return global_irradiance;
}


int compute_aerosol_scaling(double sza, double beta, double *lambda, int n_lambda, double ***factor)
     /* compute multiplication factor for aerosol loading, set to unity if clouds are present */
{
  char dummyfilename[FILENAME_MAX+200]="";
  int rows_data=0;
  int status=0, dummy;
  int z=0, i=0, rows_index=0, alt, sza_rounded;
  int max_columns=0, min_columns=0;
  double **data=NULL;
  double beta0=0.02; /* coefficients were computed using beta=beta-beta0 translation */
  int lambda_start=290, lambda_step=10;

  /* allocate memory for double array */
  if ( (status = ASCII_calloc_double (factor, n_lambda, 3)) != 0 )
    return status;

  for (z=0; z<3; z++){
    sza_rounded=(int)((sza/DELTA_SZA)+0.5)*DELTA_SZA;
    alt=z*DELTA_ALT;
    dummy = sprintf(dummyfilename,
      "%s%d%s%d", "./TransmittancesCloudH2O0.000_coeffs_beta/sza", sza_rounded,
      "alt", alt);
    
    /* read coefficient file */
    status = ASCII_file2double (dummyfilename, &rows_data,
      &max_columns, &min_columns, &data);
    if (status!=0) {
      /* run error loop*/
      for (i=0; i<n_lambda; i++)  {
       for (z=0; z<3; z++){
         (*factor)[i][z] = NaN;
       }
     }
     return status;
   }
   
   if (max_columns!=min_columns) {
    fprintf (stderr, " !! ATTENTION !! Inconsistent number of columns\n");
    fprintf (stderr, "     min = %d, max =%d\n", min_columns, max_columns);
    exit(0);
  }
  
  if (min_columns!=2) {
    fprintf (stderr, " ... ending, incorrect number of columns\n");
    exit(0);
  }
  
  for (i=0; i<n_lambda; i++) {
    rows_index=(int)((lambda[i]-lambda_start)/lambda_step+0.5);
      (*factor)[i][z]=1.+ data[rows_index][0]*(beta-beta0) + data[rows_index][1]*(beta-beta0)*(beta-beta0); /* polynomial coefficients were determined using a beta=beta-beta0 translation */
  }
  
  ASCII_free_double (data, rows_data);
}
return status;
}


int compute_atmospheric_reflectance(double o3, double beta,
  double cloudH2O, double *x_cloudH2O, int subscr_cloudH2O_max,
  double *lambda, int n_lambda, double ***AtmReflArray)
     /* compute multiplication factor for multiple bounces of light at the surface-atmosphere boundary */

     /* improve sensitivity with ozone and aerosols */
{
  char dummyfilename[FILENAME_MAX+200]="";
  int rows_data=0;
  int status=0, status_c=0, status_v=0, dummy;
  double *a0=NULL, *a1=NULL, *a2=NULL, *a3=NULL;
  int z=0, i=0, j, alt, subscr_cloudH2O,
  rows_index=0, rows_index_min, rows_index_max, rows_index_nb;
  int max_columns=0, min_columns=0;
  double **data, **ozonefactor=NULL, **betafactor=NULL,
  y_cloudH2O[4], ynew=0., **tmp[4], *x_wl=NULL, *y_wl=NULL;
  int lambda_start=290, lambda_step=10;
  double beta0=0.02; /* coefficients were computed using beta=beta-beta0 translation */
  double o30=300.; /* coefficients were computed using o3=o3-o30 translation */

  rows_index_min=(int)((lambda[0]-lambda_start)/lambda_step+0.5);
  rows_index_max=(int)((lambda[n_lambda-1]-lambda_start)/lambda_step+0.5);
  rows_index_nb=rows_index_max-rows_index_min+1;
  if (rows_index_nb < 3){
    if (rows_index_min > 0){rows_index_min--;rows_index_nb++;}
    if (rows_index_max < 11){rows_index_max++;rows_index_nb++;}
  }
  x_wl = calloc (rows_index_nb, sizeof(double));
  y_wl = calloc (rows_index_nb, sizeof(double));

  /*  if ((cloudH2O-cloudH2O_low) < (cloudH2O_high-cloudH2O)){
      cloudH2O=cloudH2O_low;
      }
      else {
      cloudH2O=cloudH2O_high;
      }*/


  /* compute atmospheric reflectance for base case */
  /* allocate memory for double array */
  if ( (status = ASCII_calloc_double (AtmReflArray, n_lambda, 3)) != 0 )
    return status;
  
  for (z=0; z<3; z++) {
    alt=z*DELTA_ALT;
    for (subscr_cloudH2O=0;subscr_cloudH2O<=subscr_cloudH2O_max;subscr_cloudH2O++){
      dummy = sprintf(dummyfilename,
        "%s%5.3f%s%d", "./AtmosphericReflectivitiesCloudH2O", x_cloudH2O[subscr_cloudH2O],
        "/alt", alt);
      /*    dummy = sprintf(dummyfilename,
        "%s%d", "./AtmosphericReflectivitiesCloudH2O0.000/alt", alt);*/
      
      /* read coefficient file */
      status = ASCII_file2double (dummyfilename, &rows_data,
        &max_columns, &min_columns, &tmp[subscr_cloudH2O]);
      if (status!=0) {
    /* run error loop*/
       for (i=0; i<n_lambda; i++)  {
         for (z=0; z<3; z++){
           (*AtmReflArray)[i][z] = NaN;
         }
       }
       return status;
     }
     
     if (max_columns!=min_columns) {
       fprintf (stderr, " !! ATTENTION !! Inconsistent number of columns\n");
       fprintf (stderr, "     min = %d, max =%d\n", min_columns, max_columns);
       exit(0);
     }
     
     if (min_columns!=1) {
      fprintf (stderr, " ... ending, incorrect number of columns\n");
      exit(0);
    }
  }
  rows_index=-1;
  for (i=rows_index_min; i<=rows_index_max; i++) {
    rows_index++;
    for (subscr_cloudH2O=0;subscr_cloudH2O<=subscr_cloudH2O_max;subscr_cloudH2O++){
     y_cloudH2O[subscr_cloudH2O]=tmp[subscr_cloudH2O][i][0];
   }
   if (subscr_cloudH2O_max==0){
     ynew=y_cloudH2O[0];
   }
   else {
     status_c = spline_coeffc (x_cloudH2O, y_cloudH2O, subscr_cloudH2O_max+1, &a0, &a1, &a2, &a3);
     status_v = calc_splined_value (cloudH2O, &ynew, x_cloudH2O, subscr_cloudH2O_max+1, a0, a1, a2, a3);
     free_splinecoef_results(status_c, a0, a1, a2, a3);
   }
   x_wl[rows_index]=lambda_start+i*lambda_step;
   y_wl[rows_index]=ynew;
 }
  status_c = spline_coeffc (x_wl, y_wl, rows_index+1, &a0, &a1, &a2, &a3);
  for (j=0; j<n_lambda; j++) {
    status_v = calc_splined_value (lambda[j], &ynew, x_wl, rows_index+1, a0, a1, a2, a3);
    (*AtmReflArray)[j][z]=ynew;
  }
      for (subscr_cloudH2O=0;subscr_cloudH2O<=subscr_cloudH2O_max;subscr_cloudH2O++){
          ASCII_free_double(tmp[subscr_cloudH2O], rows_data);
      }

  free_splinecoef_results(status_c, a0, a1, a2, a3);
}
    free(x_wl);
    free(y_wl);

  /* compute scaling factor for ozone content */
  /* allocate memory for double array */
if ( (status = ASCII_calloc_double (&ozonefactor, n_lambda, 3)) != 0 ){
  return status;
}

for (z=0; z<3; z++){
  alt=z*DELTA_ALT;
  dummy = sprintf(dummyfilename,
    "%s%d", "./AtmosphericReflectivitiesCloudH2O0.000_coeffs_ozone/alt", alt);
    /* read coefficient file */
  status = ASCII_file2double (dummyfilename, &rows_data,
    &max_columns, &min_columns, &data);
  if (status!=0) {
    /* run error loop*/
    for (i=0; i<n_lambda; i++)  {
     for (z=0; z<3; z++){
       ozonefactor[i][z] = NaN;
     }
   }
   return status;
 }
 
 if (max_columns!=min_columns) {
  fprintf (stderr, " !! ATTENTION !! Inconsistent number of columns\n");
  fprintf (stderr, "     min = %d, max =%d\n", min_columns, max_columns);
  exit(0);
}

if (min_columns!=2) {
  fprintf (stderr, " ... ending, incorrect number of columns\n");
  exit(0);
}

for (i=0; i<n_lambda; i++) {
  rows_index=(int)((lambda[i]-lambda_start)/lambda_step+0.5);
  ozonefactor[i][z]=1.+ data[rows_index][0]*(o3-o30) + data[rows_index][1]*(o3-o30)*(o3-o30);
}
ASCII_free_double (data, rows_data);
}

  /* compute scaling factor for aerosol loading */
  /* allocate memory for double array */
if ( (status = ASCII_calloc_double (&betafactor, n_lambda, 3)) != 0 )
  return status;

  if (cloudH2O != 0.000){ /* ignore aerosols if clouds are present */
for (z=0; z<3; z++){
  for (i=0; i<n_lambda; i++) {
   betafactor[i][z]=1.;
 }
}
}
else {
  for (z=0; z<3; z++){
    alt=z*DELTA_ALT;
    dummy = sprintf(dummyfilename,
      "%s%d", "./AtmosphericReflectivitiesCloudH2O0.000_coeffs_beta/alt", alt);
      /* read coefficient file */
    status = ASCII_file2double (dummyfilename, &rows_data,
      &max_columns, &min_columns, &data);
    if (status!=0) {
    /* run error loop*/
     for (i=0; i<n_lambda; i++)  {
       for (z=0; z<3; z++){
         betafactor[i][z] = NaN;
       }
     }
     return status;
   }
   
   if (max_columns!=min_columns) {
     fprintf (stderr, " !! ATTENTION !! Inconsistent number of columns\n");
     fprintf (stderr, "     min = %d, max =%d\n", min_columns, max_columns);
     exit(0);
   }
   
   if (min_columns!=2) {
     fprintf (stderr, " ... ending, incorrect number of columns\n");
     exit(0);
   }
   
   for (i=0; i<n_lambda; i++) {
     rows_index=(int)((lambda[i]-lambda_start)/lambda_step+0.5);
     betafactor[i][z]=1.+ data[rows_index][0]*(beta-beta0) + data[rows_index][1]*(beta-beta0)*(beta-beta0);
   }
   ASCII_free_double (data, rows_data);
 }
}

    /* consider integrating all for loops to save operations */
for (z=0; z<3; z++){
  for (i=0; i<n_lambda; i++) {
    (*AtmReflArray)[i][z]*=betafactor[i][z]*ozonefactor[i][z];
    /* fprintf (stderr, "function: z %d i %d AtmReflArray %f betafactor %f ozonefactor %f\n",
       i, z, (*AtmReflArray)[i][z], betafactor[i][z], ozonefactor[i][z]); */
  }
}
ASCII_free_double(betafactor, n_lambda);
ASCII_free_double(ozonefactor, n_lambda);
return status;
}


double *newton_co(int np, double *x, double *y)

     /*
      * Newton interpolation
      *
      *   Reference: Cheney and Kincaid, Numerical Mathematics and
      *              Computing, 1980, Brooks/Cole Publishing Company
      *              Monterey, California.
      */
{
  double *a=NULL;
  int i, j;
  a = calloc (np, sizeof(double));
  for (i = 0; i < np; i++){
    a[i] = y[i];
  }
  
  for (j = 1; j <= np - 1; j++){
    for (i = np; i >= j + 1; i--){
      a[i-1] = (a[i-1] - a[i-2]) / (x[i-1] - x[i-j-1]);
    }
  }
  return a;
}

double eval (int n, double *x, double *a, double t)
     /*
      * Newton interpolation
      *
      *   Reference: Cheney and Kincaid, Numerical Mathematics and
      *              Computing, 1980, Brooks/Cole Publishing Company
      *              Monterey, California.
      */
{
  double ev;
  int i;
  
  ev = a[n-1];
  for (i = n - 2; i >= 0; i--) {
    ev = ev * ( t - x[i] ) + a[i];
  }
  return ev;
}


int run_fastrt_(int argc, char **argv, double *doserates_out)
     /* reads input parameters, accesses irradiance data which represent
    conditions closest to the specified ones, interpolates the available
    data, and writes result to standard output */
{
  double global_irradiance, *int_grid_data[4][4][3],
  x_o3[4], y_o3[4], x_sza[4], y_sza[4],
  x_alt[3], y_alt[3], x_cloudH2O[4]={0.0,0.0,0.0,0.0}, y_cloudH2O[4], ynew=0.;
  int i, j, k, z, subscr_o3, subscr_sza, subscr_alt,
  subscr_cloudH2O, subscr_cloudH2O_max=0,
  n_lambda=0, n_alt=3, start_alt=0;
  double szagrid[4], ozonegrid[4], altgrid[3];
  
  char xfilename[FILENAME_MAX+200]="",
  srfilename[FILENAME_MAX+200]="",
  filename[4][4][3][FILENAME_MAX+200];
  int status=0, status_c=0, status_v=0;
  
  double *a0=NULL, *a1=NULL, *a2=NULL, *a3=NULL, *a=NULL;
  int dummy, c=0, surfaceno=0, index=0;


  int sza_flag=0, ozone_flag=0, alt_flag=0,
  albedo_flag=0, albedo_file_flag=0, albedo_type_flag=0,
  beta_flag=0, broken_cloud_flag=0, cloudH2O_flag=0,
  start_lambda_flag=0, end_lambda_flag=0, day_flag=0,
  step_lambda_flag=0, x_flag=0, fwhm_flag=0, sr_flag=0;
  
  double alt=0.0, start_lambda=0.0, end_lambda=0.0, step_lambda=0.0;

  double sza=0.0, o3=0.0, beta=0.0,
  cloudOD = 0.0,  cloudH2O = 0.0, cloudH2O_low = 0.0, cloudH2O_high = 0.0,
  visibility, fwhm=0.0, day_corr, angle,
  pi=3.14159265358979323846264338327, day=0.,
  alb=0, *albedo=NULL, **AtmReflArray=NULL, **AerosolScalingArray=NULL,
  AtmAlbFactor=1., *lambda=NULL, *sr_lambda=NULL, *sr=NULL;
  int sr_nlambda=0;
  char surfacealbedo[FILENAME_MAX+200]="";
  
  double **albedo_measured_lambda=NULL, **albedo_measured=NULL;
  int rows=0;
  int max_columns=0, min_columns=0;
  double **data=NULL, *tmp[4], tau550;

/*  double cloud_H2O_array[9] = {0.0, 0.04, 0.06, 0.10, 0.14, 0.26, 0.41, 0.58, 1.0}; */
  double cloud_H2O_array[9] = {0.000, 0.005, 0.014, 0.029, 0.057, 0.109, 0.217, 0.460, 1.000};

  double surface_albedo[18][14] = {
/* 290 - 420 nm, 10 nm intervals, 12 + 6 = 18 surface types  */
/* (1) U. Feister and R. Grewe, Spectral albedo measurements in the
   UV and visible region over different types of surfaces, Photochemistry
   and Photobiology, 62, 736-744, 1995.   */
/* (2) M. Blumthaler and W. Ambach, Solar UVB-Albedo of various surfaces, Photochemistry
   and Photobiology, 48, 1, pp. 85-88, 1988 */
    {0.755, 0.764, 0.765, 0.769, 0.775, 0.785, 0.791, 0.796, 0.802, 0.807, 0.810, 0.818, 0.825, 0.826}, /* sno5 (1) */
    {0.615, 0.623, 0.629, 0.632, 0.640, 0.645, 0.656, 0.661, 0.665, 0.669, 0.670, 0.672, 0.673, 0.677}, /* sno2 (1) */
    {0.126, 0.138, 0.148, 0.160, 0.171, 0.182, 0.193, 0.200, 0.209, 0.221, 0.229, 0.239, 0.246, 0.254}, /* sand (1) */
    {0.021, 0.023, 0.024, 0.026, 0.027, 0.029, 0.031, 0.032, 0.033, 0.035, 0.037, 0.039, 0.041, 0.045}, /* lawn (1) */
    {0.095, 0.096, 0.098, 0.105, 0.110, 0.118, 0.123, 0.131, 0.136, 0.141, 0.150, 0.161, 0.172, 0.179}, /* cdry (1) */
    {0.072, 0.077, 0.078, 0.083, 0.087, 0.092, 0.097, 0.101, 0.105, 0.110, 0.117, 0.127, 0.137, 0.144}, /* cwet (1) */
    {0.016, 0.016, 0.017, 0.017, 0.017, 0.018, 0.018, 0.018, 0.019, 0.019, 0.020, 0.022, 0.024, 0.027}, /* gras (1) */
    {0.018, 0.019, 0.019, 0.020, 0.021, 0.021, 0.022, 0.022, 0.023, 0.024, 0.025, 0.027, 0.025, 0.027}, /* beet (1) */
    {0.017, 0.017, 0.017, 0.018, 0.018, 0.018, 0.018, 0.018, 0.019, 0.019, 0.019, 0.020, 0.022, 0.024}, /* oat (1) */
    {0.039, 0.041, 0.044, 0.048, 0.052, 0.055, 0.058, 0.062, 0.066, 0.070, 0.075, 0.080, 0.085, 0.091}, /* loam (1) */
    {0.015, 0.016, 0.018, 0.018, 0.019, 0.019, 0.019, 0.020, 0.020, 0.020, 0.021, 0.023, 0.026, 0.030}, /* rye (1) */
    {0.019, 0.019, 0.019, 0.020, 0.021, 0.021, 0.022, 0.023, 0.024, 0.025, 0.026, 0.029, 0.032, 0.035},  /* stub (1) */
    {0.900, 0.900, 0.906, 0.891, 0.890, 0.901, 0.905, 0.906, 0.915, 0.913, 0.918, 0.920, 0.925, 0.927},  /* snow new (2) */
    {0.811, 0.811, 0.835, 0.837, 0.838, 0.842, 0.849, 0.854, 0.865, 0.868, 0.862, 0.876, 0.872, 0.879},  /* snow old (2) */
    {0.060, 0.060, 0.076, 0.085, 0.092, 0.099, 0.103, 0.106, 0.110, 0.113, 0.105, 0.108, 0.129, 0.133},  /* sand dry (2) */
    {0.137, 0.137, 0.133, 0.141, 0.144, 0.147, 0.150, 0.154, 0.159, 0.164, 0.170, 0.178, 0.184, 0.190},  /* limestone (2) */
    {0.011, 0.011, 0.011, 0.011, 0.011, 0.010, 0.010, 0.012, 0.011, 0.010, 0.012, 0.012, 0.015, 0.019},  /* meadow (2) */
    {0.018, 0.018, 0.031, 0.035, 0.037, 0.037, 0.041, 0.045, 0.046, 0.049, 0.055, 0.049, 0.057, 0.065}  /* field dry (2) */
  };

  #include "solirr.c"


/* accept command line options */

  while ((c=getopt (argc, argv, "a:v:b:cu:t:o:z:p:q:l:f:r:w:g:e:s:x:d:h")) != EOF) {
    switch(c) {
      case 'a':
      sza_flag=1;
      sza = atof(optarg);
      if (sza < 0.) {
       fprintf (stderr, "error: solar zenith angle less than 0 degrees\n");
       exit(0);
     }
     if (sza > 90.) {
       fprintf (stderr, "warning: solar zenith angle greater than 90 degrees\n");
     }
     break;
     case 'v':
     beta_flag=1;
     visibility = atof(optarg);
     if (visibility < 5.) {
       fprintf (stderr, "warning: visibility less than 5 km\n");
       exit(0);
     }
     if (visibility > 350.) {
       fprintf (stderr, "warning: visibility more than 350 km\n");
       exit(0);
     }
      /* parametrization from Iqbal M., An Introduction to Solar Radiation, Academic, San Diego, CA, 1983 */
     tau550 = (3.912/visibility-0.01162)*(0.02472*(visibility-5.)+1.132);
     beta=tau550*pow(0.55,1.3);
      /*      fprintf (stdout, "Aerosol optical depth at 550nm: %6.2f\n", tau550);
          fprintf (stdout, "beta: %6.2f\n", beta);  */
     break;
     case 'b':
     beta_flag=1;
     beta = atof(optarg);
     if (beta < 0.) {
       fprintf (stderr, "error: Aerosol beta less than 0\n");
       exit(0);
     }
     if (beta > 0.4) {
       fprintf (stderr, "warning: Aerosol beta greater than 0.4\n");
       exit(0);
     }
     break;
     case 'c':
     broken_cloud_flag=1;
     break;
     case 't':
     cloudH2O_flag=1;
     cloudOD = atof(optarg);
     if (cloudOD < 0.) {
       fprintf (stderr, "error: cloud optical depth less than 0\n");
       exit(0);
     }
     if (cloudOD >= 1083.) {
       fprintf (stderr, "error: cloud optical depth greater than 1083\n");
       exit(0);
     }
      cloudH2O=cloudOD/1083.; /* convert cloud optical depth to cloud liquid water content in a 5km thick cloud (g m-2)*/
     i=0;
     while (cloud_H2O_array[i] < cloudH2O){
       i++;
     }
     if (cloud_H2O_array[i] == cloudH2O){
       x_cloudH2O[0]=cloudH2O;
       subscr_cloudH2O_max=0;
       cloudH2O_high=cloudH2O;
       cloudH2O_low=cloudH2O;
     }
     else{
       subscr_cloudH2O=-1;
       for(j=0;j<4;j++){
         k=i-2+j;
      if ((k>=0) && (k<=8)){ /*8 is the max index of cloud_H2O_array */
         subscr_cloudH2O++;
         x_cloudH2O[subscr_cloudH2O]=cloud_H2O_array[k];
       }
     }
     subscr_cloudH2O_max=subscr_cloudH2O;
     cloudH2O_high=cloud_H2O_array[i];
     cloudH2O_low=cloud_H2O_array[i-1];
   }
   break;
   case 'u':
   cloudH2O_flag=1;
      cloudH2O = atof(optarg)/CLOUD_THICKNESS/1000.; /* convert cloud liquid water column to cloud liquid water content to 2 decimals*/
   if (cloudH2O < 0.) {
     fprintf (stderr, "error: cloud liquid water content less than 0\n");
     exit(0);
   }
   if (cloudH2O > 1.) {
     fprintf (stderr, "error: cloud liquid water content %f in the assumed %f km thick cloud is greater than or equal to 1.\n", cloudH2O, CLOUD_THICKNESS);
     exit(0);
   }
   i=0;
   while (cloud_H2O_array[i] < cloudH2O){
     i++;
   }
   if (cloud_H2O_array[i] == cloudH2O){
     x_cloudH2O[0]=cloudH2O;
     subscr_cloudH2O_max=0;
     cloudH2O_high=cloudH2O;
     cloudH2O_low=cloudH2O;
   }
   else{
     subscr_cloudH2O=-1;
     for(j=0;j<4;j++){
       k=i-2+j;
      if ((k>=0) && (k<=8)){ /*8 is the max index of cloud_H2O_array */
       subscr_cloudH2O++;
       x_cloudH2O[subscr_cloudH2O]=cloud_H2O_array[k];
     }
   }
   subscr_cloudH2O_max=subscr_cloudH2O;
   cloudH2O_high=cloud_H2O_array[i];
   cloudH2O_low=cloud_H2O_array[i-1];
 }
 break;
 case 'o':
 ozone_flag=1;
 o3 = atof(optarg);
 if (o3 < 100.) {
   fprintf (stderr, "warning: ozone column less than 100 DU\n");
 }
 if (o3 > 600) {
   fprintf (stderr, "warning: ozone column greater than 600 DU\n");
 }
 break;
 case 'z':
 alt_flag=1;
 alt = atof(optarg);
 if (alt < 0.) {
   fprintf (stderr, "warning: surface altitude less than 0 km\n");
 }
 if (alt > 6.) {
   fprintf (stderr, "warning: surface altitude greater than 6 km\n");
 }
 break;
 case 'p':
 albedo_flag=1;
 alb = atof(optarg);
 if (alb < 0.) {
   fprintf (stderr, "error: surface albedo less than 0\n");
 }
 if (alb > 1.) {
   fprintf (stderr, "warning: surface albedo greater than 1\n");
 }
 break;
 case 'q':
 albedo_type_flag=1;
 surfaceno = atoi(optarg);
 if (surfaceno < 0) {
   fprintf (stderr, "error: surface # less than 0\n");
   return (-1);
 }
 if (surfaceno > 17) {
   fprintf (stderr, "error: surface # greater than 17\n");
   return (-1);
 }
 break;
 case 'l':
 albedo_file_flag=1;
 strcpy(surfacealbedo, optarg);
 break;
 case 'f':
 fwhm_flag=1;
 fwhm = atof(optarg);
 if (fwhm <0.05 || fwhm > 55.) {
   fprintf (stderr,"warning: FWHM not within range [0.05,55] nm\n");
 }
 break;
 case 'r':
 sr_flag=1;
 strcpy(srfilename, optarg);
 break;
 case 'w':
 n_lambda = 1;
 lambda=(double *) calloc (n_lambda, sizeof(double));
 *lambda = atof(optarg);
 if ((*lambda < 290.) || (*lambda > 405.)) {
   fprintf (stderr, "warning: wavelength outside [290,405] nm\n");
 }
 break;
 case 'g':
 start_lambda_flag=1;
 start_lambda = atof(optarg);
 if (start_lambda < 290.) {
   fprintf (stderr, "warning: start wavelength less than 290 nm\n");
 }
 break;
 case 'e':
 end_lambda_flag=1;
 end_lambda = atof(optarg);
 if (end_lambda > 405.) {
   fprintf (stderr, "warning: end wavelength greater than 405 nm\n");
 }
 break;
 case 's':
 step_lambda_flag=1;
 step_lambda = atof(optarg);
 break;
 case 'x':
 x_flag=1;
 strcpy(xfilename, optarg);
 break;
 case 'd':
 day_flag = 1;
 day = atof(optarg);
 break;
 case 'h':
 print_usage();
 return (-1);
 break;
 default:
 print_usage();
 return (-1);
}
}

#ifdef linux
optind = 0;
#else
optind = 1;
optreset = 1;
#endif
  /* check validity of wavelengths */
if (!sza_flag || !ozone_flag) {
  fprintf (stderr, "solar zenith angle or ozone column is inadequately specified");
  print_usage();
  return (-1);

}
if (!(fwhm_flag || sr_flag)) {
    /* Round off to nearest multiple of SOLAR_FLUX_RESOLUTION */
  fwhm = (double) (((int) (FWHM_DEFAULT/SOLAR_FLUX_RESOLUTION + 0.5)) * SOLAR_FLUX_RESOLUTION);
  status = make_slitfunction(fwhm, &sr_lambda, &sr, &sr_nlambda);
}
if (fwhm_flag) {
    /* Round off to nearest multiple of SOLAR_FLUX_RESOLUTION */
  fwhm = (double) (((int) (fwhm/SOLAR_FLUX_RESOLUTION + 0.5)) * SOLAR_FLUX_RESOLUTION);
  status = make_slitfunction(fwhm, &sr_lambda, &sr, &sr_nlambda);
}
if (sr_flag) {
    /* 'fprintf (stderr, " ... reading slitfunction from file %s ...\n", srfilename); */
  status = read_slitfunction(srfilename, &sr_lambda, &sr, &sr_nlambda);
  status = check_spectral_response_function(sr_lambda, sr, sr_nlambda);
}

if (start_lambda_flag && end_lambda_flag && step_lambda_flag) {
  n_lambda = (int) ((end_lambda-start_lambda)/step_lambda) + 1;
  lambda=(double *) calloc (n_lambda, sizeof(double));
  for (i = 0; i < n_lambda; i++) {
    lambda[i] = start_lambda + i * step_lambda;
  }
}

if (x_flag) {
    /*    fprintf (stderr, " ... reading wavelengths from file %s ...\n", xfilename); */
  
    /* read file with user x values */
  status = read_1c_file (xfilename, &lambda, &n_lambda);
  if ((lambda[0] < 290.) || (lambda[n_lambda-1] > 405.)) {
    fprintf (stderr, "warning: wavelength beyond [290,405] nm\n");
  }
  if (status!=0)  {
    fprintf (stderr, "error reading file %s\n", xfilename);
    return (-1);
  }
}

if (lambda == NULL) {
  fprintf (stderr, "output wavelengths inadequately specified");
  print_usage();
  return (-1);
}

if (day_flag == 0) {
  day_corr=1.;
}
else {
    /* correct for deviations from average sun-earth distance
       From J. Lenoble, "Atmospheric Radiative Transfer", 1993, A. Deepak Publishing */
  angle = 2.0 * pi * (double) (day-1) / 365.0;
  day_corr = 1.000110 + 0.034221 * cos(angle) + 0.001280 * sin(angle)
  + 0.000719 * cos(2*angle) + 0.000077 * sin(2*angle);
}

albedo=(double *) calloc (n_lambda, sizeof(double));

if (albedo_flag) {
  for (k=0;k<n_lambda;k++){
    albedo[k]=alb;
  }
}
else if (albedo_type_flag) {
  for (i = 0; i < n_lambda; i++) {
    index = (int) ((lambda[i]-290.)/ALBEDO_RESOLUTION + 0.5);
    albedo[i] = surface_albedo[surfaceno][index];
  }
}
else if (albedo_file_flag){
  status = ASCII_file2double (surfacealbedo, &rows,
    &max_columns, &min_columns, &data);
  if (status!=0) {
    fprintf (stderr, "ERROR: cannot read albedo file\n");
    exit(0);
  }
  
  if (max_columns!=min_columns) {
    fprintf (stderr, " !! ATTENTION !! Inconsistent number of columns\n");
    fprintf (stderr, "     min = %d, max =%d\n", min_columns, max_columns);
    exit(0);
  }
  if (min_columns<2) {
    fprintf (stderr, " ... ending, too few columns\n");
    exit(0);
  }
  
  *albedo_measured_lambda = ASCII_column (data, rows, 0);
  *albedo_measured = ASCII_column (data, rows, 1);
  
  ASCII_free_double (data, rows);
  
    /* interpolate tabulated data to output wavelengths */
  i=0;
  for (k=0; k<n_lambda; k++){
    while ((lambda[k] < (*albedo_measured_lambda)[i]) && (i < rows)) {
     i++;
   }
   albedo[k]= (*albedo_measured)[i] +
   (lambda[k]-(*albedo_measured_lambda)[i])*
   ((*albedo_measured)[i+1]-(*albedo_measured)[i])/((*albedo_measured_lambda)[i]-(*albedo_measured_lambda)[i]);
 }
}

/* find closest precomputed tabulated data entries*/
for (i=0; i<4; i++){
  szagrid[i] = (double)((int)(sza/DELTA_SZA)-1+i)*DELTA_SZA;
  ozonegrid[i] = (double)((int)(o3/DELTA_O3)-1+i)*DELTA_O3;
}
for (z=0; z<n_alt; z++){
  altgrid[z] = (double)(z*DELTA_ALT);
  if (alt == altgrid[z]) {
    n_alt=1;
    start_alt = z;
  }
}

for (i=0; i<4; i++){
  for (j=0; j<4; j++){
    for (z=start_alt; z<start_alt+n_alt; z++){
     if (broken_cloud_flag){
    /* read fringe spectra files and interpolate to output wavelengths */
       dummy = sprintf(filename[i][j][z],
         "%s%d%s%d%s%d", "./TransmittancesCloudH2O0.000/sza", ((int)fabs(szagrid[i])),
         "ozone", ((int)ozonegrid[j]),
         "alt", ((int)altgrid[z]));
       int_grid_data[i][j][z] = do_spectra(filename[i][j][z], lambda, n_lambda,
         sr_lambda, sr, sr_nlambda, solirr);
     }
     else {
       dummy = sprintf(filename[i][j][z],
         "%s%5.3f%s%d%s%d%s%d", "./TransmittancesCloudH2O", x_cloudH2O[0],
         "/sza", ((int)fabs(szagrid[i])),
         "ozone", ((int)ozonegrid[j]),
         "alt", ((int)altgrid[z]));
      /*      fprintf (stderr, "%s\n", filename[i][j][z]);*/
       
      /* read fringe spectra files and interpolate to output wavelengths */
       int_grid_data[i][j][z] = do_spectra(filename[i][j][z], lambda, n_lambda,
         sr_lambda, sr, sr_nlambda, solirr);
       
      /* do spline interpolation of cloud tabular entries */
       if (cloudH2O != x_cloudH2O[0]){
         tmp[0]=int_grid_data[i][j][z];
         for (subscr_cloudH2O=1;subscr_cloudH2O<=subscr_cloudH2O_max;subscr_cloudH2O++){
           dummy = sprintf(filename[i][j][z],
             "%s%5.3f%s%d%s%d%s%d", "./TransmittancesCloudH2O", x_cloudH2O[subscr_cloudH2O],
             "/sza", ((int)fabs(szagrid[i])),
             "ozone", ((int)ozonegrid[j]),
             "alt", ((int)altgrid[z]));
           
          /* read fringe spectra files and interpolate to output wavelengths */
           tmp[subscr_cloudH2O] = do_spectra(filename[i][j][z], lambda, n_lambda,
            sr_lambda, sr, sr_nlambda, solirr);
         }
         for (k = 0; k < n_lambda; k++) {
          /*        int_grid_data[i][j][z][k] = int_grid_data[i][j][z][k] +
                (cloudH2O-cloudH2O_low) *
                (tmp[k]-int_grid_data[i][j][z][k])/(cloudH2O_high-cloudH2O_low);*/
           for (subscr_cloudH2O=0;subscr_cloudH2O<=subscr_cloudH2O_max;subscr_cloudH2O++){
            y_cloudH2O[subscr_cloudH2O]=log(tmp[subscr_cloudH2O][k]);
          }
          status_c = spline_coeffc (x_cloudH2O, y_cloudH2O, subscr_cloudH2O_max+1, &a0, &a1, &a2, &a3);
          status_v = calc_splined_value (cloudH2O, &ynew, x_cloudH2O, subscr_cloudH2O_max+1, a0, a1, a2, a3);
          int_grid_data[i][j][z][k] = exp(ynew);
          /*          fprintf (stderr, "%f\n", int_grid_data[i][j][z][k]);*/
          free_splinecoef_results(status_c, a0, a1, a2, a3);

        }
      }
    }
  }
}
}

free(sr_lambda);
free(sr);

  /* compute multiplication factor for aerosol loading */
if ((beta != 0.02) && (cloudH2O_flag !=1)) {
  status = compute_aerosol_scaling(sza, beta, lambda, n_lambda, &AerosolScalingArray);
  if (status!=0) {
    fprintf (stderr, "ERROR: computation of aerosol effect failed\n");
    exit(0);
  }
}

  /* compute multiplication factor for multiple bounces of light at the surface-atmosphere boundary */
if (albedo_flag || albedo_type_flag || albedo_file_flag){
  status = compute_atmospheric_reflectance(o3, beta, cloudH2O, x_cloudH2O, subscr_cloudH2O_max, lambda, n_lambda, &AtmReflArray);
  if (status!=0) {
    fprintf (stderr, "ERROR: computation of albedo effect failed\n");
    exit(0);
  }
}

for (k = 0; k < n_lambda; k++) {
  subscr_alt=-1;
  for (z=start_alt; z<start_alt+n_alt; z++){
    subscr_sza=-1;
    for (i=0; i<4; i++){
     subscr_o3=-1;
     for (j=0; j<4; j++){
       if (int_grid_data[i][j][z][k] != NaN) {
         subscr_o3++;
         x_o3[subscr_o3]=ozonegrid[j];
         y_o3[subscr_o3]=int_grid_data[i][j][z][k];
       }
     }

    /* interpolated to correct ozone column */
     status_c = spline_coeffc (x_o3, y_o3, subscr_o3+1, &a0, &a1, &a2, &a3);
     status_v = calc_splined_value (o3, &ynew, x_o3, subscr_o3+1, a0, a1, a2, a3);
     free_splinecoef_results(status_c, a0, a1, a2, a3);

     if ((status_c==0) && (status_v==0)) {
       subscr_sza++;
       x_sza[subscr_sza] = szagrid[i];
       y_sza[subscr_sza] = ynew;
     }
   }
   
      /* interpolate to correct solar zenith angle */
   status_c = spline_coeffc (x_sza, y_sza, subscr_sza+1, &a0, &a1, &a2, &a3);
   status_v = calc_splined_value (sza, &ynew, x_sza, subscr_sza+1, a0, a1, a2, a3);
   free_splinecoef_results(status_c, a0, a1, a2, a3);


      /* correct for multiple bounces between surface and atmosphere and aerosols
       scaling factors must be multiplied with transmittance at beta=0.02 (default)*/

      /* compute multiplication factor for aerosol loading */
   if ((beta != 0.02) && (cloudH2O_flag !=1)) {
    /*    fprintf (stderr, "k %d z %d AerosolScalingArray %f\n", k, z, AerosolScalingArray[k][z] );*/
     ynew*=AerosolScalingArray[k][z];
   }
   
      /* compute multiplication factor for multiple bounces of light at the surface-atmosphere boundary */
   if (albedo_flag || albedo_type_flag || albedo_file_flag){
     AtmAlbFactor = 1./(1-AtmReflArray[k][z]*albedo[k]);
     ynew*=AtmAlbFactor;
    /*    fprintf (stderr, "main: k %d z %d AtmReflArray %f albedo %f AtmAlbFactor %f\n",
        k, z, AtmReflArray[k][z], albedo[k], AtmAlbFactor); */
   }
   
   if ((status_c==0) && (status_v==0)) {
     subscr_alt++;
     x_alt[subscr_alt] = altgrid[z];
     y_alt[subscr_alt] = ynew;
   }
   else {
     fprintf (stderr, "sorry cannot do spline interpolation\n");
     fprintf (stderr, "spline_coeffc() returned status_c status_v %d %d \n", status_c, status_v);
     fprintf (stderr, "solar zenith angle or ozone column is beyond prespecified range?\n");
     print_usage();
     exit(status);
   }
 }
    
 if (n_alt > 1) {
  a = newton_co (subscr_alt+1, x_alt, y_alt);
      /* interpolate to correct surface altitude */
  ynew = eval (subscr_alt+1, x_alt, a, alt);
  free(a);
}

    /* copy data to result array */
if (status==0) {
  global_irradiance = ynew * day_corr;
}
else {
  fprintf (stderr, "sorry cannot do spline interpolation\n");
  fprintf (stderr, "calc_splined_value() returned status %d\n", status);
  fprintf (stderr, "Ozone content, solar zenith angle, altitude, \n");
  fprintf (stderr, "or wavelength are beyond prespecified range?\n");
  print_usage();
  exit(status);
}
//    fprintf (stdout, "%6.2f %10.4e\n", lambda[k], global_irradiance);
doserates_out[k] = global_irradiance;
}
    if ((beta != 0.02) && (cloudH2O_flag !=1))
    {
        ASCII_free_double(AerosolScalingArray, n_lambda);
    }
    // free all used memory
    ASCII_free_double(AtmReflArray, n_lambda);

    if (n_lambda > 0)
    {
        free(lambda);
        free(albedo);
    }
    
for (i=0; i<4; i++){
  for (j=0; j<4; j++){
    for (z=start_alt; z<start_alt+n_alt; z++){
      free(int_grid_data[i][j][z]);
    }
  }
}


return (0);
}
