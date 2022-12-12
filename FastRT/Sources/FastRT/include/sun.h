/************************************************************************/
/* sun.h                                                                */
/*                                                                      */
/* Solar zenith and azimuth angle calculations.                         */
/*                                                                      */
/* This file is part of the Solar Database package                      */
/*                                                                      */
/* Author: Bernhard Mayer,                                              */
/*        Fraunhofer Institute for Atmospheric Environmental Research,  */
/*        82467 Garmisch-Partenkirchen,                                 */
/*        Germany                                                       */
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

#ifndef __sun_h
#define __sun_h

#if defined (__cplusplus)
extern "C" {
#endif


/* error messages */
#define ERROR_NO_ZENITH     -1


/* prototypes */

double eccentricity  (int day);            /* Julian day           */

double declination   (int day);            /* Julian day           */

int equation_of_time (int day);            /* Julian day           */

int LAT              (int time,            /* standard time        */       
		      int day,             /* Julian day           */
		      double longitude,    /* longitude            */
		      double long_std);    /* standard longitude   */

double hour_angle    (int time);           /* local apparent time  */

double solar_zenith  (int time,            /* standard time        */
		      int day,             /* Julian day           */
		      double latitude,     /* latitude             */
		      double longitude,    /* longitude            */  
		      double long_std);    /* standard longitude   */

double solar_azimuth (int time,            /* standard time        */
		      int day,             /* Julian day           */
		      double latitude,     /* latitude             */
		      double longitude,    /* longitude            */
		      double long_std);	   /* standard longitude   */

int zenith2time      (int day,             /* Julian day           */
		      double zenith_angle, /* zenith angle [deg]   */
		      double latitude,     /* latitude [deg]       */
		      double longitude,    /* longitude [deg]      */
		      double long_std,     /* standard longitude   */
		      int *time1,          /* first time           */
		      int *time2);         /* second time          */

int day_of_year (int day,                  /* day of month (1..31) */ 
		 int month);               /* month        (1..12) */       

char *time2str  (char *timestr,            /* buffer for timestr   */
		 int hour,                 /* hour                 */
		 int min,                  /* minute               */
		 int sec);                 /* second               */


#if defined (__cplusplus)
}
#endif


#endif







