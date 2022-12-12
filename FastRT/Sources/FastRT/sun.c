/************************************************************************/
/* sun.c                                                                */
/*                                                                      */
/* sun.a - Solar zenith and azimuth calculations                   @ti@ */
/*                                                                      */
/* Author:                                                              */
/*   Bernhard Mayer                                                @au@ */
/*   Fraunhofer Institute for Atmospheric Environmental Research,  @ad@ */
/*   82467 Garmisch-Partenkirchen,                                 @ad@ */
/*   Germany                                                       @ad@ */
/*                                                                      */
/* Most of the formulas are adopted from:                               */
/*   Iqbal, Muhammad: "An Introduction to Solar Radiation",             */
/*   Academic Press, Inc., 1983                                         */
/* (page numbers in the comments refer to this book).                   */
/*                                                                      */
/* Time is specified in seconds from midnight,                          */
/* angles are specified in degrees.                                     */
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

#include <math.h>
#include <stdio.h>
#include <string.h>

#include <sun.h>



/* @42c@ */
/* @c42@ */
/****************************************************************************/
/* In order to use the functions provided by the sun library,     @42_10c@  */
/* #include <sun.h> in your source code and link with sun.a.                */
/*                                                                          */
/* @strong{Example:}                                                        */
/* Example for a source file:                                               */
/* @example                                                                 */
/*                                                                          */
/*   ...                                                                    */
/*   #include <sun.h>                                                       */
/*   ...                                                                    */
/*                                                                          */
/* @end example                                                             */
/*                                                                          */
/* Linking of the executable, using the GNU compiler gcc:                   */
/* @example                                                                 */
/*                                                                          */
/*   gcc -o test test.c -lsun                                               */
/*                                                                          */
/* @end example                                                   @c42_10@  */
/****************************************************************************/


/****************************************************************************/
/* The sun library provides functions for solar zenith and azimuth @42_20c@ */
/* angle and sun-earth-distance calculations. All formulas have been taken  */
/* from Iqbal, "An introduction to solar radiation".                        */
/*                                                                 @c42_20@ */
/****************************************************************************/


/* define _PI_ */
#define _PI_ 3.1415926

/* internal functions */
static double dayangle      (int day);


/***********************************************************************************/
/* Function: dayangle                                                              */
/* Description:                                                                    */
/*  Internal function to calculate the dayangle according to Iqbal, pg. 3          */
/*                                                                                 */
/* Parameters:                                                                     */
/*  int  day:  Julian day.                                                         */
/*                                                                                 */
/* Return value:                                                                   */
/*  double dayangle, 0..2pi                                                        */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                         */
/*                                                                                 */
/***********************************************************************************/

static double dayangle (int day)
{
  return 2.0 * _PI_ * (double) (day-1) / 365.0;
}



/***********************************************************************************/
/* Function: eccentricity                                                 @42_30i@ */
/* Description:                                                                    */
/*  Calculate the eccentricity correction factor E0 = (r0/r)**2 according to       */
/*  Iqbal, page 3. This factor, when multiplied with the irradiance, accounts      */
/*  for the annual variation of the sun-earth-distance.                            */
/*                                                                                 */
/* Parameters:                                                                     */
/*  int day:  Julian day (leap day is usually @strong{not} counted.                */
/*                                                                                 */
/* Return value:                                                                   */
/*  The eccentricity (double) for the specified day.                               */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                         */
/*                                                                        @i42_30@ */
/***********************************************************************************/

double eccentricity (int day)  
{
  double E0=0, angle=0;

  angle = dayangle (day); 

  E0 =   1.000110 + 0.034221 * cos(angle) + 0.001280 * sin(angle)
       + 0.000719 * cos(2*angle) + 0.000077 * sin(2*angle);

  return E0;
}



/***********************************************************************************/
/* Function: declination                                                  @42_30i@ */
/* Description:                                                                    */
/*  Calculate the declination for a specified day (Iqbal, page 7).                 */
/*                                                                                 */
/* Parameters:                                                                     */
/*  int day:  Julian day (leap day is usually @strong{not} counted.                */
/*                                                                                 */
/* Return value:                                                                   */
/*  The declination in degrees (double) for the specified day.                     */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                         */
/*                                                                        @i42_30@ */
/***********************************************************************************/

double declination (int day)
{
  double delta=0, angle=0;

  angle = dayangle (day); 

  delta =   0.006918 - 0.399912 * cos (angle) + 0.07257 * sin (angle) 
          - 0.006758 * cos (2*angle) + 0.000907 * sin (2*angle) 
          - 0.002697 * cos (3*angle) + 0.00148 * sin (3*angle);
  
  delta *= 180.0/_PI_;

  return delta;
}



/***********************************************************************************/
/* Function: equation_of_time                                             @42_30i@ */
/* Description:                                                                    */
/*  Calculate the equation of time for a specified day (Iqbal, page 11).           */
/*                                                                                 */
/* Parameters:                                                                     */
/*  int day:  Julian day (leap day is usually @strong{not} counted.                */
/*                                                                                 */
/* Return value:                                                                   */
/*  The equation of time in seconds (double) for the specified day.                */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                         */
/*                                                                        @i42_30@ */
/***********************************************************************************/

int equation_of_time (int day)  
{
  double angle=0, et=0;

  angle = dayangle (day);

  et = (0.000075 + 0.001868 * cos(angle) - 0.032077 * sin(angle) 
        -0.014615 * cos(2*angle) - 0.04089 * sin(2*angle)) * 13750.8;

  return (int) (et+0.5);
}



/***********************************************************************************/
/* Function: LAT                                                          @42_30i@ */
/* Description:                                                                    */
/*  Calculate the local apparent time for a given standard time and location.      */
/*                                                                                 */
/* Parameters:                                                                     */
/*  int time_std:     Standard time [seconds since midnight].                      */ 
/*  int day:          Julian day (leap day is usually @strong{not} counted.        */
/*  double longitude: Longitude [degrees] (West positive).                         */
/*  double long_std:  Standard longitude [degrees].                                */
/*                                                                                 */
/* Return value:                                                                   */
/*  The local apparent time in seconds since midnight (double).                    */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                         */
/*                                                                        @i42_30@ */
/***********************************************************************************/

int LAT (int time_std, int day, double longitude, double long_std) 
{
  int lat=0;

  lat = time_std + (int) (240.0 * (long_std-longitude)) 
                 + equation_of_time (day);

  return lat;
} 


/************************************************/
/* convert local apparent time to standard time */
/************************************************/

int standard_time (int lat, int day, double longitude, double long_std) 
{
  return lat - (int) (240.0 * (long_std-longitude)) - equation_of_time (day);
} 


/****************************/
/* hour angle omega; pg. 15 */
/****************************/

double hour_angle (int time)  
{
  double omega=0;

  omega = _PI_ * (1.0 - ((double) time) / 43200.0);

  omega *= 180.0/_PI_;
  return omega;
}



/***********************************************************************************/
/* Function: solar_zenith                                                 @42_30i@ */
/* Description:                                                                    */
/*  Calculate the solar zenith angle for a given time and location.                */
/*                                                                                 */
/* Parameters:                                                                     */
/*  int time:         Standard time [seconds since midnight].                      */
/*  int day:          Julian day (leap day is usually @strong{not} counted.        */
/*  double latitude:  Latitude [degrees] (North positive).                         */
/*  double longitude: Longitude [degrees] (West positive).                         */
/*  double long_std:  Standard longitude [degrees].                                */
/*                                                                                 */
/* Return value:                                                                   */
/*  The solar zenith angle [degrees].                                              */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                         */
/*                                                                        @i42_30@ */
/***********************************************************************************/

double solar_zenith (int time, int day, 
		     double latitude, double longitude, double long_std)
{
  double theta=0, omega=0, delta=0, phi = latitude*_PI_/180.0;
  int lat = LAT (time, day, longitude, long_std);

  delta = _PI_/180.0*declination (day);
  omega = _PI_/180.0*hour_angle (lat);
    
  theta = acos(sin(delta) * sin(phi) + cos(delta) * cos(phi) * cos(omega));

  theta *= (180.0/_PI_);   /* convert to degrees */
  return theta;
}




/***********************************************************************************/
/* Function: solar_azimuth                                                @42_30i@ */
/* Description:                                                                    */
/*  Calculate the solar azimuth angle for a given time and location.               */
/*                                                                                 */
/* Parameters:                                                                     */
/*  int time:         Standard time [seconds since midnight].                      */
/*  int day:          Julian day (leap day is usually @strong{not} counted.        */
/*  double latitude:  Latitude [degrees] (North positive).                         */
/*  double longitude: Longitude [degrees] (West positive).                         */
/*  double long_std:  Standard longitude [degrees].                                */
/*                                                                                 */
/* Return value:                                                                   */
/*  The solar azimuth angle [degrees].                                             */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                         */
/*                                                                        @i42_30@ */
/***********************************************************************************/

double solar_azimuth (int time, int day, 
		      double latitude, double longitude, double long_std)
{
  double theta=0, omega=0, delta=0, phi = latitude*_PI_/180.0, psi=0;
  int lat = LAT (time, day, longitude, long_std);

  delta = _PI_/180.0*declination (day);
  omega = _PI_/180.0*hour_angle (lat);
  theta = _PI_/180.0*solar_zenith (time, day, latitude, longitude, long_std);


  psi = -acos((cos(theta)*sin(phi) - sin(delta)) / sin(theta) / cos (phi));

  /* adjust sign */
  if (lat>43200 || lat<0)
    psi=-psi;

  psi *= (180.0/_PI_);   /* convert to degrees */
  
  return psi;
}



/***********************************************************************************/
/* Function: day_of_year                                                  @42_30i@ */
/* Description:                                                                    */
/*  Calculate the Julian day for given date (leap days are not considered.         */
/*                                                                                 */
/* Parameters:                                                                     */
/*  int day:          Day of month (1..31).                                        */
/*  int month:        Month (1..12).                                               */
/*                                                                                 */
/* Return value:                                                                   */
/*  The Julian day (int); -1 if error.                                             */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                         */
/*                                                                        @i42_30@ */
/***********************************************************************************/

int day_of_year (int day, int month)
{
  char mon =  0;
  int  doy = -1;

  if (month<1 || month > 12)
    return (-1);

  if (day<1 || day > 31)
    return (-1);
  
  mon = (char) month;

  switch (mon)  {
  case 1:
    doy = day + 0;
    break;
  case 2:
    doy = day + 31;
    break;
  case 3:
    doy = day + 59;
    break;
  case 4:
    doy = day + 90;
    break;
  case 5:
    doy = day + 120;
    break;
  case 6:
    doy = day + 151;
    break;
  case 7:
    doy = day + 181;
    break;
  case 8:
    doy = day + 212;
    break;
  case 9:
    doy = day + 243;
    break;
  case 10:
    doy = day + 273;
    break;
  case 11:
    doy = day + 304;
    break;
  case 12:
    doy = day + 334;
    break;
  default:
    doy = -1;
  }

  return (doy);
}




/**************************************************************************/
/* convert time to string                                                 */
/* memory for timestr must be allocated by programmer (at least 10 bytes) */
/**************************************************************************/

char *time2str (char *timestr, int hour, int min, int sec)
{
  char hourstr[3] = "";
  char minstr[3]  = "";
  char secstr[3]  = "";

  strcpy (timestr, "");

  if (hour<0 || hour>24)  
    return NULL;

  if (min<0 || min>60)  
    return NULL;

  if (sec<0 || sec>60)  
    return NULL;



  if (hour<10)
    sprintf (hourstr, "0%d", hour);
  else 
    sprintf (hourstr,  "%d", hour);
    
  if (min<10)
    sprintf (minstr, "0%d", min);
  else 
    sprintf (minstr,  "%d", min);
      
  if (sec<10)
    sprintf (secstr, "0%d", sec);
  else 
    sprintf (secstr,  "%d", sec);
      
  sprintf (timestr, "%s:%s:%s", hourstr, minstr, secstr);

  return timestr;
}




/***********************************************************************************/
/* Function: zenith2time                                                  @42_30i@ */
/* Description:                                                                    */
/*  Calculate the times for a given solar zenith angle, Julian day and location.   */
/*                                                                                 */
/* Parameters:                                                                     */
/*  int day:              Julian day.                                              */
/*  double zenith_angle:  Solar zenith angle [degrees].                            */
/*  double latitude:      Latitude [degrees] (North positive).                     */
/*  double longitude:     Longitude [degrees] (West positive).                     */
/*  double long_std:      Standard longitude [degrees].                            */
/*  int *time1:           1st time of occurence.                                   */ 
/*  int *time2:           2nd time of occurence.                                   */ 
/*                                                                                 */
/* Return value:                                                                   */
/*  0  if o.k., <0 if error.                                                       */
/*                                                                                 */
/* Example:                                                                        */
/* Files:                                                                          */
/* Known bugs:                                                                     */
/* Author:                                                                         */
/*                                                                        @i42_30@ */
/***********************************************************************************/

int zenith2time (int day, 
		 double zenith_angle,
		 double latitude,
		 double longitude,
		 double long_std,
		 int *time1, 
		 int *time2)
{
  double delta = _PI_/180.0*declination (day);
  double phi   = _PI_/180.0*latitude;
  double theta = _PI_/180.0*zenith_angle;
  double cos_omega = (cos(theta) - sin(delta)*sin(phi)) / 
                      cos(delta) / cos(phi);
  double omega1=0, omega2=0;
  int lat1=0, lat2=0;

  if (fabs(cos_omega) > 1.0)
    return ERROR_NO_ZENITH;
  
  omega1 = acos (cos_omega);
  omega2 = 0.0-omega1;
  
  lat1 = 43200*(1-omega1/_PI_);
  lat2 = 43200*(1-omega2/_PI_);
  
  *time1 = standard_time (lat1, day, longitude, long_std);
  *time2 = standard_time (lat2, day, longitude, long_std);

  return 0;
}
