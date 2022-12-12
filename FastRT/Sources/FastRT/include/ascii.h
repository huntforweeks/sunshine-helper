/************************************************************************/
/* ascii.h                                                              */
/*                                                                      */
/* Reading ASCII files.                                                 */
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

#ifndef __ascii_h
#define __ascii_h

#if defined (__cplusplus)
extern "C" {
#endif

/* errorcodes of ASCII_functions */

#define ASCIIFILE_NOT_FOUND      -1
#define ASCII_NO_MEMORY          -2
#define LESS_THAN_TWO_COLUMNS    -3
#define LESS_THAN_THREE_COLUMNS  -4


#define MAX_NUMBER_OF_COLUMNS  1024
#define MAX_LENGTH_OF_LINE     4096


/* prototypes */
int swift_package_file_access_shim(char *filename, char* resource_path_out);
int ASCII_checkfile     (char *filename, int *rows, 
			 int *min_columns, int *max_columns, int *max_length);
int ASCII_calloc_string (char ****string, int rows, int columns, int length);
int ASCII_free_string   (char ***string, int rows, int columns);
int ASCII_calloc_double (double ***value, int rows, int columns);
int ASCII_free_double   (double **value, int rows);
int ASCII_readfile      (char *filename, char ***array);
int ASCII_string2double (double **value, char *** string, int rows, int columns);
int ASCII_file2double   (char *filename, int *rows, 
			 int *max_columns, int *min_columns, double ***value);
int ASCII_calloc_float  (float ***value, int rows, int columns);
int ASCII_calloc_float_3D(float ****value, int rows, int columns, int length);
int ASCII_calloc_float_4D(float *****value, int rows, int columns, int length, int fourth_dimension);
int ASCII_free_float    (float **value, int rows);
int ASCII_free_float_3D (float ***value, int rows, int length);
int ASCII_file2float    (char *filename, int *rows, 
			 int *max_columns, int *min_columns, float ***value);
int ASCII_parse         (char *string, char *separator, char ***array, int *number);
int ASCII_parsestring   (char *string, char ***array, int *number);
double *ASCII_column    (double **value, int rows, int column);
float  *ASCII_column_float (float **value, int rows, int column);
int read_1c_file        (char *filename, 
			 double **first, int *n);
int read_2c_file        (char *filename, 
			 double **first, double **second, int *n);
int read_3c_file        (char *filename, 
			 double **first, double **second, double **third, 
			 int *n);
int read_5c_file        (char *filename, 
			 double **first, double **second, double **third, 
			 double **fourth, double **fifth, int *n);
char *substr            (char *buffer, char *string, int start, int length);


#if defined (__cplusplus)
}
#endif

#endif
