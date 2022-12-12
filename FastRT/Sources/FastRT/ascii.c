/*************************************************************************/
/* ascii.c                                                               */
/*                                                                       */
/* ascii.a - Parse ASCII files                                     @ti@  */
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


/* @30c@ */
/* @c30@ */
/****************************************************************************/
/* In order to use the functions provided by the ascii library,   @30_10c@  */
/* #include <ascii.h> in your source code and link with ascii.a.            */
/*                                                                          */
/* @strong{Example:}                                                        */
/* Example for a source file:                                               */
/* @example                                                                 */
/*                                                                          */
/*   ...                                                                    */
/*   #include <ascii.h>                                                     */
/*   ...                                                                    */
/*                                                                          */
/* @end example                                                             */
/*                                                                          */
/* Linking of the executable, using the GNU compiler gcc:                   */
/* @example                                                                 */
/*                                                                          */
/*   gcc -o test test.c -lascii                                             */
/*                                                                          */
/* @end example                                                   @c30_10@  */
/****************************************************************************/


/****************************************************************************/
/* The ASCII library provides functions for parsing ASCII files   @30_20c@  */
/* containing arrays of data.                                               */
/* An ASCII file is read line by line. Each line is split into fields;      */
/* a field is an arbitrary combination of characters which does             */
/* neither contain the line separator ('CARRIAGE RETURN') nor the field     */
/* separator ('SPACE').                                                     */
/*                                                                          */
/* In detail:                                                               */
/* @itemize @asis                                                           */
/*   @item Lines are separated by 'CARRIAGE RETURN'.                        */
/*   @item Tokens (or columns) are separated by one or more 'SPACE's.       */
/*   @item Empty lines are simply ignored.                                  */
/*   @item \% and # are comment symbols; text between a comment symbol      */
/*     and the next line separator is ignored                               */
/*   @item A comment symbol which is not at the beginning of a line is      */
/*     only recognized after a field separator, but not within a field      */ 
/*   @item The number of fields may differ from line to line.               */   
/* @end itemize                                                             */
/*                                                                          */
/* A simple example for an ASCII file, which would be recognized as a       */
/* valid one-column or two-column ASCII file:                               */
/* @example                                                                 */
/*                                                                          */
/*   % This is an example for the input ASCII file for sdose,               */
/*   % the time integration program                                         */
/*   11.0     13.0   % the two hours around noon                            */
/*   10.0     14.0                                                          */
/*    9.0     15.0                                                          */
/*                                                                          */
/*   # total dose                                                           */
/*   -1.0     24.0    % integrate over maximum available time interval      */
/*                                                                          */
/*   # the following line shows, that an extra column does not matter       */
/*   2  3.4  17                                                             */
/*                                                                          */
/* @end example                                                             */
/*                                                                          */ 
/* For most purposes, ASCII_file2double and ASCII_free_double provide a     */
/* convenient way for parsing files.                                        */
/* @strong{Example:}                                                        */
/* @example                                                                 */
/*                                                                          */
/*  #include <ascii.h>                                                      */
/*                                                                          */ 
/*  void main(int argc, char ** argv)                                       */
/*  @{                                                                      */
/*    int rows=0, max_columns=0, min_columns=0;                             */
/*    int i=0, status=0;                                                    */
/*    double **value=NULL;                                                  */
/*                                                                          */
/*    status = ASCII_file2double ("test.dat",                               */
/*  	                          &rows,                                    */
/*  			          &max_columns,                             */
/*  			          &min_columns,                             */
/*  			          &value);                                  */
/*		                                                            */
/*    for (i=0; i<rows; i++) @{                                             */
/*      ... do something for each row of the matrix                         */
/*   @}                                                                     */
/*                                                                          */
/*    ASCII_free_double (value, rows);                                      */
/* @}                                                                       */
/*                                                                          */
/* @end example                                                             */
/*                                                                          */
/* For special purposes (ASCII files with 1,2,3, or 5 columns) there are    */
/* additionally functions read_1c_file, ... which facilitate the            */
/* access even more.                                                        */
/*                                                                 @c30_20@ */
/****************************************************************************/
									    
#include <ctype.h>							    
#include <stdio.h>							    
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <float.h>
#include <math.h>

#include <CoreFoundation/CFBundle.h>
#include <CoreFoundation/CFUtilities.h>

#include "ascii.h"



/**********************************/
/* Error codes                    */
/**********************************/

#define ASCIIFILE_NOT_FOUND      -1 
#define ASCII_NO_MEMORY          -2 
#define LESS_THAN_TWO_COLUMNS    -3 
#define LESS_THAN_THREE_COLUMNS  -4 
#define LESS_THAN_FIVE_COLUMNS   -5 



/********************************************/
/* Limits                                   */
/********************************************/

/* Maximum number of columns in a file */
#define MAX_NUMBER_OF_COLUMNS  1024   

/* Maximum number of characters per line */
#define MAX_LENGTH_OF_LINE     4096



/***********************************/
/* Character to indicate a comment */
/***********************************/

#define ASCII_COMMENT_1          '%'
#define ASCII_COMMENT_2          '#'




/************************************/
/* Prototypes of internal functions */
/************************************/

static int ASCII_comment (char t);


/***********************************************************************************/
/* Function: ASCII_checkfile                                              @30_30i@ */
/* Description:                                                                    */
/*  Check an ASCII file: count rows, minimum number of columns,                    */
/*  maximum number of columns and the maximum length of a string;                  */
/*  empty rows and characters after one of the comment symbols                     */
/*  (either ASCII_COMMENT_1 or ASCII_COMMENT_2) are ignored.                       */ 
/*                                                                                 */
/* Parameters:                                                                     */
/*  char *filename:    name of the file which should be checked                    */
/*  int  *rows:        number of rows found                                        */
/*  int  *min_columns: minimum number of columns, set by function                  */
/*  int  *max_columns: maximum number of columns, set by function                  */
/*  int  *max_length:  maximum length of a field, set by function                  */
/*                                                                                 */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                         */
/*                                                                        @i30_30@ */
/***********************************************************************************/

void check_and_release_CFRef(CFTypeRef ref)
{
    if (ref != NULL)
        CFRelease(ref);
}

int swift_package_file_access_shim(char *filename, char* resource_path_out)
{
    CFBundleRef mainbundle = CFBundleGetMainBundle();
    CFURLRef FastRTBundleURL = CFBundleCopyResourceURL(mainbundle, CFSTR("FastRT_FastRT.bundle"), NULL, NULL);
//    check_and_release_CFRef(mainbundle);

    CFBundleRef FastRTBundle = CFBundleCreate(kCFAllocatorDefault, FastRTBundleURL );
    check_and_release_CFRef(FastRTBundleURL);
    
    CFURLRef ResourcesURL = CFBundleCopyResourceURL(FastRTBundle, CFSTR("Resources"), NULL, NULL);
    check_and_release_CFRef(FastRTBundle);
    
    int hasLeadingjunk = 0;
    if (strlen(filename) > 2 && filename[0] == '.' && filename[1] == '/')
        hasLeadingjunk = 2;
    
    CFStringRef cfFilename = CFStringCreateWithCString(kCFAllocatorDefault, filename + hasLeadingjunk, kCFStringEncodingASCII);
    CFURLRef resource_url = CFURLCreateCopyAppendingPathComponent(kCFAllocatorDefault, ResourcesURL, cfFilename, false);
    check_and_release_CFRef(ResourcesURL);
    check_and_release_CFRef(cfFilename);

    if (resource_url == NULL)
    {
        return ASCIIFILE_NOT_FOUND;
    }

    if (!CFURLGetFileSystemRepresentation(resource_url, true,
                                          resource_path_out, 1024))
    {
        check_and_release_CFRef(resource_url);
        return ASCIIFILE_NOT_FOUND;
    }
    check_and_release_CFRef(resource_url);
    return 0;
}

int ASCII_checkfile (char *filename, 
		     int *rows,
		     int *min_columns,
		     int *max_columns,
		     int *max_length)
   
{
  FILE *f=NULL;
  char line[MAX_LENGTH_OF_LINE+1]="";
  char *token=NULL;
  char *string=NULL;
  int temp1=0, temp2=0;
  int min_col=INT_MAX, max_col=0, max_len=0, r=0;
  

  /* reset parameters */
  *min_columns=0;
  *max_columns=0;
  *max_length=0;
  *rows=0;

  string = line;
    
    char resource_path[1024] = "";
    if (swift_package_file_access_shim(filename, resource_path) == ASCIIFILE_NOT_FOUND)
        return ASCIIFILE_NOT_FOUND;
  
  if ( (f = fopen(resource_path, "r")) == NULL)
    return ASCIIFILE_NOT_FOUND;

  /* count rows and columns */
  while ( fgets (string, MAX_LENGTH_OF_LINE, f) != NULL )  {

    if ( (token = strtok (string, " \t\n")) != NULL ) { /* if not an empty line */ 
      /* if not a comment     */
      if (!ASCII_comment(token[0]))  {

	r++;
	temp1++;

	/* check for maximal string length */
	temp2 = strlen(token);
	max_len = (temp2>max_len ? temp2 : max_len);

	while ( (token = strtok (NULL, " \t\n")) != NULL)  {

	  /* check for maximal string length */
	  temp2 = strlen(token);
	  max_len = (temp2>max_len ? temp2 : max_len);

	  if (ASCII_comment(token[0]))  /* if comment */
	    break;
	  temp1++;
	}

	min_col = (temp1<min_col ? temp1 : min_col);
	max_col = (temp1>max_col ? temp1 : max_col);
	temp1 = 0;
      }
    }

    string = line;
  }

  *min_columns = min_col;
  *max_columns = max_col;
  *max_length  = max_len;
  *rows        = r; 
  

  fclose (f);
  return 0;
}




/***********************************************************************************/
/* Function: ASCII_calloc_string                                          @30_30i@ */
/* Description: Allocate memory for a two-dimensional array of strings.            */
/*                                                                                 */
/* Parameters:                                                                     */
/*  char ****string:  Pointer to a two-dimensional array of strings; memory        */
/*                    for string is allocated automatically                        */
/*  int rows:         Number of rows, specified by the caller                      */
/*  int columns:      Number of columns, specified by the caller                   */
/*  int length:       Maximum length of a string, specified by the caller          */
/*                                                                                 */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int ASCII_calloc_string (char ****string,
			 int rows,
			 int columns,
			 int length)
{
  int i=0, j=0;

  if ((*string = (char ***) calloc (rows, sizeof (char **))) == NULL)
    return ASCII_NO_MEMORY;

  for (i=0; i<rows; i++)  {
    if (((*string)[i] = (char **) calloc (columns, sizeof (char *))) == NULL)
      return ASCII_NO_MEMORY;

    for (j=0; j<columns; j++)
      if (((*string)[i][j] = (char *) calloc (length+1, sizeof (char))) == NULL) 
	return ASCII_NO_MEMORY;
    
  }
  
  return 0;
}    




/***********************************************************************************/
/* Function: ASCII_free_string                                            @30_30i@ */
/* Description: Free memory, which has been allocated with ASCII_calloc_string.    */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int ASCII_free_string (char ***string, 
		       int rows,
		       int columns)

{
  int i=0, j=0;

  for (i=0; i<rows; i++)  {
    for (j=0; j<columns; j++)
      free (string[i][j]);

    free (string[i]);
  }

  free (string);

  return 0;
}    



/***********************************************************************************/
/* Function: ASCII_calloc_double                                          @30_30i@ */
/* Description: Allocate memory for a two-dimensional array of double.             */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int ASCII_calloc_double (double ***value,
			 int rows,
			 int columns)
{
  int i=0;
  
  if ( (*value = (double **) calloc ((size_t) rows, sizeof (double *))) == NULL )
    return ASCII_NO_MEMORY;

  for (i=0; i<rows; i++)  
    if ( ((*value)[i] = (double *) calloc ((size_t) columns, 
					   sizeof (double))) == NULL )
      return ASCII_NO_MEMORY;

  return 0;
}    



/***********************************************************************************/
/* Function: ASCII_calloc_float_3D                                        @30_30i@ */
/* Description: Allocate memory for a three-dimensional array of float.            */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author: Arve Kylling                                                   @i30_30@ */
/***********************************************************************************/

int ASCII_calloc_float_3D  (float ****value,
			 int rows,
			 int columns,
			 int length)
{
  int i=0, j=0;

  if ( (*value = (float ***) calloc (rows, sizeof (float **))) == NULL )  
    return ASCII_NO_MEMORY;

    
  for (i=0; i<rows; i++) {
    if ( ((*value)[i] = (float **) calloc (columns, sizeof (float *))) == NULL )
      return ASCII_NO_MEMORY;
    for (j=0; j<columns; j++)
      if (((*value)[i][j] = (float *) calloc (length, sizeof (float))) == NULL) 
	return ASCII_NO_MEMORY;
    
  }

  return 0;
}    

/***********************************************************************************/
/* Function: ASCII_calloc_float_4D                                        @30_30i@ */
/* Description: Allocate memory for a four-dimensional array of float.             */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author: Arve Kylling                                                   @i30_30@ */
/***********************************************************************************/

int ASCII_calloc_float_4D  (float *****value,
			 int rows,
			 int columns,
			 int length,
			 int fourth_dimension)
{
  int i=0, j=0, k=0;

  if ( (*value = (float ****) calloc (rows, sizeof (float ***))) == NULL )  
    return ASCII_NO_MEMORY;

    
  for (i=0; i<rows; i++) {
    if ( ((*value)[i] = (float ***) calloc (columns, sizeof (float **))) == NULL )
      return ASCII_NO_MEMORY;
    for (j=0; j<columns; j++) {
      if (((*value)[i][j] = (float **) calloc (length, sizeof (float *))) == NULL) 
	return ASCII_NO_MEMORY;
      for (k=0; k<length; k++)
	if (((*value)[i][j][k] = (float *) calloc (fourth_dimension, sizeof (float))) == NULL) 
	  return ASCII_NO_MEMORY;
    }    
  }

  return 0;
}    



/***********************************************************************************/
/* Function: ASCII_calloc_float                                           @30_30i@ */
/* Description: Allocate memory for a two-dimensional array of float.              */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int ASCII_calloc_float  (float ***value,
			 int rows,
			 int columns)
{
  int i=0;

  if ( (*value = (float **) calloc (rows, sizeof (float *))) == NULL )  
    return ASCII_NO_MEMORY;

    
  for (i=0; i<rows; i++) 
    if ( ((*value)[i] = (float *) calloc (columns, sizeof (float))) == NULL )
      return ASCII_NO_MEMORY;

  return 0;
}    



/***********************************************************************************/
/* Function: ASCII_free_double                                            @30_30i@ */
/* Description: Free memory, which has been allocated with ASCII_calloc_double.    */
/* Parameters:                                                                     */
/*  double **value:  Two-dimensional array of double                               */ 
/*  int rows:        Number of rows, specified by caller                           */
/*                                                                                 */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int ASCII_free_double (double **value, 
		       int rows)
{
  int i=0;

  for (i=0; i<rows; i++) 
    free (value[i]);

  free (value);

  return 0;
}    



/***********************************************************************************/
/* Function: ASCII_free_float                                             @30_30i@ */
/* Description: Free memory, which has been allocated with ASCII_calloc_float.     */
/* Parameters:                                                                     */
/*  float **value:   Two-dimensional array of float                                */ 
/*  int rows:        Number of rows, specified by caller                           */
/*                                                                                 */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int ASCII_free_float (float **value, 
		      int rows)
{
  int i=0;

  for (i=0; i<rows; i++) 
    free (value[i]);

  free (value);

  return 0;
}    


/***********************************************************************************/
/* Function: ASCII_free_float_3D                                          @30_30i@ */
/* Description: Free memory, which has been allocated with ASCII_calloc_float_3D.  */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author: Arve Kylling                                                   @i30_30@ */
/***********************************************************************************/

int ASCII_free_float_3D(float ***value, 
			int rows,
			int columns)
{
  int i=0, j=0;

  for (i=0; i<rows; i++) {
    for (j=0; j<columns; j++)
      free (value[i][j]);
    free (value[i]);
  }

  free (value);
  return 0;
}    





/***********************************************************************************/
/* Function: ASCII_readfile                                               @30_30i@ */
/* Description: Read an ASCII file into a two-dimensional array of strings;        */
/*		before calling ASCII_readfile, the file must be parsed with        */
/*		ASCII_checkfile, and  memory must be allocated with                */
/*              ASCII_calloc_string.                                               */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int ASCII_readfile (char *filename, 
		    char ***array)
{
  FILE *f=NULL;
  char line[MAX_LENGTH_OF_LINE+1]="";
  char *string=NULL;
  char *t=NULL;
  int row=0, column=0;

  
  string = line;
    
    char resource_path[1024] = "";
    if (swift_package_file_access_shim(filename, resource_path) == ASCIIFILE_NOT_FOUND)
        return ASCIIFILE_NOT_FOUND;
  
  if ( (f = fopen(resource_path, "r")) == NULL)
    return ASCIIFILE_NOT_FOUND;

  while ( fgets (string, MAX_LENGTH_OF_LINE, f) != NULL )  {

    column=0;

    if ( (t = strtok (string, " \t\n") ) != NULL)  {  /* if not an empty line */ 
      if (!ASCII_comment(t[0]))  { /* if not a comment */
	strcpy (array[row][column++], t);
	
	while ( (t = strtok (NULL, " \t\n") ) != NULL)  {
	  if (ASCII_comment(t[0]))     /* if comment */
	    break;
	  strcpy (array[row][column++], t);
	}

	row++;
      }
    }

    string = line;
  }


  fclose (f);
  return 0;
}



/***********************************************************************************/
/* Function: ASCII_string2double                                          @30_30i@ */
/* Description: Convert a two-dimensional array of strings to a two-dimensional    */
/*		array of double.                                                   */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int ASCII_string2double (double **value, 
			 char *** string,
			 int rows,
			 int columns)
{
  int i=0, j=0;
  char *dummy=NULL;

  for (i=0; i<rows; i++) 
    for (j=0; j<columns; j++)  {

      if (string[i][j][0] == 0)  
	value[i][j] = NAN;
      else  
	value[i][j] = strtod (string[i][j], &dummy);

    }

  return 0;
}    




/***********************************************************************************/
/* Function: ASCII_string2float                                           @30_30i@ */
/* Description: Convert a two-dimensional array of strings to a two-dimensional    */
/*		array of float.                                                    */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int ASCII_string2float (float **value, 
			char *** string,
			int rows,
			int columns)
{
  int i=0, j=0;
  char *dummy=NULL;

  for (i=0; i<rows; i++)  { 
    for (j=0; j<columns; j++)  {
      if (string[i][j][0] == 0)   {
	value[i][j] = (float) NAN;
      }
      else  {
	value[i][j] = (float) strtod (string[i][j], &dummy);
      }
    }
  }
  return 0;
}    







/***********************************************************************************/
/* Function: ASCII_file2double                                            @30_30i@ */
/* Description: Parse an ASCII file and store data in a twodimensional array       */
/*        value[row][column]; memory allocation for value is done automatically.   */
/*        rows is the number of (not empty) rows of the file, max_columns is the   */
/*        maximal number of columns of the file and min_columns is the minimal     */
/*        number of columns of all (not empty) lines; the dimension of the array   */
/*        value is rows * max_columns; strings that cannot be interpreted as       */
/*        floating point number are converted to 0; rows with less than            */
/*        max_columns columns are filled up with NAN; the allocated memory         */
/*        can be freed with ASCII_free_double (value, rows).                       */
/* Parameters:                                                                     */
/*  char *filename:     Name of the file which should be parsed                    */
/*  int  *rows:         Number of rows, set by function                            */
/*  int  *min_columns:  Minimum number of columns, set by function                 */
/*  int  *max_columns:  Maximum number of columns, set by function                 */
/*  double ***value:    Pointer to a two-dimensional array of double,              */
/*                      value [0 ... rows-1][0 ... max_columns-1].                 */
/*                      Memory is allocated automatically.                         */
/*                                                                                 */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int ASCII_file2double (char *filename,   
		       int *rows,        
		       int *max_columns, 
		       int *min_columns, 
		       double ***value)  
{
  char ***string = NULL;
  int status=0;
  int max_length=0;

  /* count rows and columns of ASCII file <filename> */
  if ( (status = ASCII_checkfile (filename, 
				  rows, 
				  min_columns, 
				  max_columns, 
				  &max_length)) != 0)
    return status;
  


  /* allocate memory for string array */
  if ( (status = ASCII_calloc_string (&string, 
				      *rows, 
				      *max_columns, 
				      max_length)) != 0 )
    return status;
  
  /* read ASCII file to string array */
  if ( (status = ASCII_readfile (filename, string)) != 0)
    return status;

  /* allocate memory for double array */
  if ( (status = ASCII_calloc_double (value, *rows, *max_columns)) != 0 )
    return status;

  /* convert string array to double array */
  if ( (status = ASCII_string2double (*value, string, *rows, *max_columns)) != 0)
    return status; 



  /* free memory of string array */
  ASCII_free_string (string, *rows, *max_columns);

  return 0;  /* everything ok */
} 



/***********************************************************************************/
/* Function: ASCII_file2float                                             @30_30i@ */
/* Description: Read an ASCII file and store data in a twodimensional array        */
/*        value[row][column]; memory allocation for value is done automatically.   */
/*        rows is the number of (not empty) rows of the file, max_columns is the   */
/*        maximal number of columns of the file and min_columns is the minimal     */
/*        number of columns of all (not empty) lines; the dimension of the array   */
/*        value is rows * max_columns; strings that cannot be interpreted as       */
/*        floating point number are converted to 0; rows with less than            */
/*        max_columns columns are filled up with NAN; the allocated memory         */
/*        can be freed with ASCII_free_float (value, rows).                        */
/*                                                                                 */
/* Parameters:                                                                     */
/*  char *filename:     Name of the file which should be parsed                    */
/*  int  *rows:         Number of rows, set by function                            */
/*  int  *min_columns:  Minimum number of columns, set by function                 */
/*  int  *max_columns:  Maximum number of columns, set by function                 */
/*  float ***value:     Pointer to a two-dimensional array of float,               */
/*                      value [0 ... rows-1][0 ... max_columns-1].                 */
/*                      Memory is allocated automatically.                         */
/*                                                                                 */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int ASCII_file2float (char *filename,
		      int *rows,
		      int *max_columns,
		      int *min_columns,
		      float ***value)
{
  char ***string = NULL;
  int status=0;
  int max_length=0;

  /* count rows and columns of ASCII file <filename> */
  if ( (status = ASCII_checkfile (filename, 
				  rows, 
				  min_columns, 
				  max_columns, 
				  &max_length)) != 0)
    return status;
  

  /* allocate memory for string array */
  if ( (status = ASCII_calloc_string (&string, 
				      *rows, 
				      *max_columns, 
				      max_length)) != 0 )
    return status;
  

  
  /* read ASCII file to string array */
  if ( (status = ASCII_readfile (filename, string)) != 0)
     return status;


  /* allocate memory for float array */
  if ( (status = ASCII_calloc_float (value, *rows, *max_columns)) != 0 )
    return status;

  /* convert string array to float array */
  if ( (status = ASCII_string2float (*value, string, *rows, *max_columns)) != 0)
    return status; 

  /* free memory of string array */
  ASCII_free_string (string, *rows, *max_columns);

  
  return 0;  /* everything ok */
} 



/***********************************************************************************/
/* Function: ASCII_column                                                 @30_30i@ */
/* Description: Extract a specified column from a two-dimensional array of double. */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  Pointer to the column.                                                         */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

double *ASCII_column (double **value, int rows, int column)  
{
  int i=0;
  double *col = (double *) calloc (rows, sizeof(double));

  for (i=0; i<rows; i++)
    col[i] = value[i][column];

  return col;
}
  


/***********************************************************************************/
/* Function: ASCII_column_float                                           @30_30i@ */
/* Description: Extract a specified column from a two-dimensional array of float.  */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  Pointer to the column.                                                         */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

float *ASCII_column_float (float **value, int rows, int column)  
{
  int i=0;
  float *col = (float *) calloc (rows, sizeof(float));

  for (i=0; i<rows; i++)
    col[i] = value[i][column];

  return col;
}
  




/***********************************************************************************/
/* Function: read_1c_file                                                 @30_30i@ */
/* Description: Read an ASCII file with (at least) 1 column.                       */
/*              Only the first column is returned in array first;                  */
/*              n is the number of values returned.                                */
/*              memory allocation for first is done automatically and              */
/*              can be freed with a simple free().                                 */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int read_1c_file (char *filename, 
		  double **first, int *n)
{
  int max_columns=0, min_columns=0;
  double **data=NULL;
  int status=0;

  /* read file */
  if ( (status = ASCII_file2double (filename, n, 
				    &max_columns, &min_columns, &data)) != 0)
    return status;


  /* check, if at least two columns */
  if (min_columns < 1)   {
    ASCII_free_double (data, *n);
    return LESS_THAN_TWO_COLUMNS;
  }
  
  *first  = ASCII_column (data, *n, 0);
  
  ASCII_free_double (data, *n);

  return 0;
}




/***********************************************************************************/
/* Function: read_2c_file                                                 @30_30i@ */
/* Description: Read an ASCII file with (at least) 1 column.                       */
/*              Only the first two column are returned in arrays first and second. */
/*              n is the number of values returned.                                */
/*              memory allocation for first and second is done automatically and   */
/*              can be freed with a simple free().                                 */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int read_2c_file (char *filename, 
		  double **first, double **second, int *n)
{
  int max_columns=0, min_columns=0;
  double **data=NULL;
  int status=0;

  /* read file */
  if ( (status = ASCII_file2double (filename, n, 
				    &max_columns, &min_columns, &data)) != 0)
    return status;


  /* check, if at least two columns */
  if (min_columns < 2)   {
    ASCII_free_double (data, *n);
    return LESS_THAN_TWO_COLUMNS;
  }
  
  *first  = ASCII_column (data, *n, 0);
  *second = ASCII_column (data, *n, 1);
  
  ASCII_free_double (data, *n);

  return 0;
}



/***********************************************************************************/
/* Function: read_3c_file                                                 @30_30i@ */
/* Description: Read an ASCII file with (at least) 1 column.                       */
/*              Only the first three columns are returned in arrays first, second, */
/*              and third. n is the number of values returned.                     */
/*              memory allocation for first, second, and third is done             */
/*              automatically and can be freed with a simple free().               */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int read_3c_file (char *filename, 
		  double **first, double **second, double **third, int *n)
{
  int max_columns=0, min_columns=0;
  double **data=NULL;
  int status=0;

  /* read file */
  if ( (status = ASCII_file2double (filename, n, 
				    &max_columns, &min_columns, &data)) != 0)
    return status;


  /* check, if at least three columns */
  if (min_columns < 3)   {
    ASCII_free_double (data, *n);
    return LESS_THAN_THREE_COLUMNS;
  }
  
  *first  = ASCII_column (data, *n, 0);
  *second = ASCII_column (data, *n, 1);
  *third  = ASCII_column (data, *n, 2);
  
  ASCII_free_double (data, *n);

  return 0;
}





/***********************************************************************************/
/* Function: read_5c_file                                                 @30_30i@ */
/* Description: Read an ASCII file with (at least) 1 column.                       */
/*              Only the first three columns are returned in arrays first, second, */
/*              third, fourth and fifth. n is the number of values returned.       */
/*              memory allocation for first, second, third, fourth, and fifth      */
/*              is done automatically and can be freed with a simple free().       */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int read_5c_file (char *filename, 
		  double **first, double **second, double **third, 
		  double **fourth, double **fifth, int *n)
{
  int max_columns=0, min_columns=0;
  double **data=NULL;
  int status=0;

  /* read file */
  if ( (status = ASCII_file2double (filename, n, 
				    &max_columns, &min_columns, &data)) != 0)
    return status;


  /* check, if at least three columns */
  if (min_columns < 5)   {
    ASCII_free_double (data, *n);
    return LESS_THAN_THREE_COLUMNS;
  }
  
  *first  = ASCII_column (data, *n, 0);
  *second = ASCII_column (data, *n, 1);
  *third  = ASCII_column (data, *n, 2);
  *fourth = ASCII_column (data, *n, 3);
  *fifth  = ASCII_column (data, *n, 4);
  
  ASCII_free_double (data, *n);

  return 0;
}




/***********************************************************************************/
/* Function: substr                                                       @30_30i@ */
/* Description: Create substring starting at position start with length            */
/*              length. Result is written to buffer (which MUST be                 */
/*              allocated before).                                                 */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  Pointer to the substring.                                                      */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

char *substr(char *buffer, char *string, int start, int length)
{
  strncpy (buffer, string+start, length);                    
  buffer[length]=0;  /* end of string */

  return buffer;
}





/***********************************************************************************/
/* Function: ASCII_parse                                                  @30_30i@ */
/* Description: Parse string to an array of single words. Memory for an array      */
/*              of string pointers is allocated automatically. array[i]            */ 
/*              points to the address of word #i in string!                        */
/*		Word separator is specified in separator.                          */
/*		Number of words is returned in number.                             */
/*		Characters following the comment character are ignored.            */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int ASCII_parse (char *string, char *separator, char ***array, int *number)
{

  char *start=NULL;
  char *t=NULL;
  char *save=NULL;
  char **temp=NULL;

  /* save start address of string */
  start = string;

  /* save string */
  save = (char *) calloc (strlen(string)+1, sizeof (char));
  strcpy (save, string);


  /* reset number */
  *number=0;
  
  /* count words */
  if ( (t = strtok (string, separator) ) != NULL)  {  /* if not an empty line */ 
    if (!ASCII_comment(t[0]))  {  /* if not a comment     */
      (*number)++;
      while ( (t = strtok (NULL, separator) ) != NULL)  {
	if (ASCII_comment(t[0]))     /* if comment */
	  break;

	(*number)++;
      }
    }
  }

  if (*number==0)   {
    free(save);
    return 0;     /* no words found. but o.k. */ 
  }

  /* restore string */
  string = start;

  /* allocate memory for an array of *number character pointers */
  temp = (char **) calloc (*number, sizeof(char *));

  /* restore string */
  strcpy (string, save);

  /* reset *number */
  *number=0;  

  /* now set array pointers */
  if ( (t = strtok (string, separator) ) != NULL)  {
    if (!ASCII_comment(t[0]))  {             
      temp[(*number)++] = t;
      
      while ( (t = strtok (NULL, separator) ) != NULL)  {
	if (ASCII_comment(t[0]))    
	  break;

	temp[(*number)++] = t;
      }
    }
  }


  /* copy temp to array */
  *array = temp;

  free(save);
  return 0;   /* if o.k. */
}



/***********************************************************************************/
/* Function: ASCII_parsestring                                            @30_30i@ */
/* Description: For compatibility reasons: ASCII_parsestring is just               */
/*              a call to ASCII_parse() with field separator set to " \t\n"        */ 
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                @i30_30@ */
/***********************************************************************************/

int ASCII_parsestring (char *string, char ***array, int *number)
{
  int status=0;

  status = ASCII_parse (string, " \t\n", array, number);
  return status;
}



/***********************************************************************************/
/* Function: ASCII_comment                                                         */
/* Description: Check if an ASCII character is one of the comment characters.      */
/* Parameters:                                                                     */
/* Return value:                                                                   */
/*  1, if comment, and 0, if not a comment.                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                         */
/***********************************************************************************/

static int ASCII_comment (char t) {

  if (t == ASCII_COMMENT_1 || t == ASCII_COMMENT_2)
    return 1;
  else 
    return 0;
}
