//
//  Header.h
//  
//
//  Created by Hunter Weeks on 7/2/22.
//

#ifndef fastrt__h
#define fastrt__h


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ascii.h"
#include "numeric.h"
#include <getopt.h>

#define PROGRAM "FASTRT"

#define DELTA_SZA 3.
#define DELTA_O3 20.
#define DELTA_ALT 3.
#define FWHM_DEFAULT 0.6
#define SOLAR_FLUX_RESOLUTION 0.05
#define ALBEDO_RESOLUTION 10.
#define CLOUD_THICKNESS 5.

static void print_usage();


int make_slitfunction(double fwhm, double **sr_lambda, double **sr, int *sr_nlambda);

int read_slitfunction(char *filename, double **sr_lambda, double **sr, int *rows);

int check_spectral_response_function(double *sr_lambda, double *sr, int sr_nlambda);

/* #include "do_spectra.c"*/
double *do_spectra(char *filename, double *lambda, int n_lambda,
                   double *sr_lambda, double *sr, int sr_nlambda, double *solirr);


int compute_aerosol_scaling(double sza, double beta, double *lambda, int n_lambda, double ***factor);


int compute_atmospheric_reflectance(double o3, double beta,
                    double cloudH2O, double *x_cloudH2O, int subscr_cloudH2O_max,
                                    double *lambda, int n_lambda, double ***AtmReflArray);


double *newton_co(int np, double *x, double *y);


int run_fastrt_(int argc, char **argv, double *doserates);

#endif /* fastrt__h */
