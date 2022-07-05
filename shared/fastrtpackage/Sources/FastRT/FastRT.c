//
//  File.c
//  
//
//  Created by Hunter Weeks on 7/1/22.
//

#include "FastRT.h"
#include "fastrt_.h"
#include "sun.h"
#include <math.h>

void print_debug_params(int argc, char** argv)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("%s ", argv[i]);
    }
}

int run_fastrt_test_inputs(double *doserates)
{
    
    double zenith = solar_zenith(37800,
                                 104,
                                 50.50,
                                 4.20,
                                 0.001545);
    char s_StartWavelength[10];
    snprintf(s_StartWavelength, 10, "%d", 290);
    
    char s_EndWavelength[10];
    snprintf(s_EndWavelength, 10, "%d", 400);
    
    char s_StepWavelength[10];
    snprintf(s_StepWavelength, 10, "%.2f", 1.0);
    
    char s_Zenith[10];
    snprintf(s_Zenith, 10, "%.2f", 42.40);
    
    char s_Ozone[10];
    snprintf(s_Ozone, 10, "%.1f", 350.0);
    
    char s_AngstromBeta[10];
    snprintf(s_AngstromBeta, 10, "%.2f", 0.11);
    
    char s_FWHM[10];
    snprintf(s_FWHM, 10, "%.2f", 0.6);
    
    char s_Albedo[10];
    snprintf(s_Albedo, 10, "%.2f", 0.03);

    char s_Altitude[10];
    snprintf(s_Altitude, 10, "%.2f", .15);
    
    char s_DayOfYear[10];
    snprintf(s_DayOfYear, 10, "%d", 104);
    
    char* argv[] = {"fastrt",
        "-a", s_Zenith,
        "-b", s_AngstromBeta,
        "-d", s_DayOfYear,
        "-o", s_Ozone,
        "-g", s_StartWavelength,
        "-e", s_EndWavelength,
        "-s", s_StepWavelength,
        "-f", s_FWHM,
        "-p", s_Albedo,
        "-z", s_Altitude
    };
    int argc = 21;
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("%s ", argv[i]);
    }
    
    return run_fastrt_(argc, argv, doserates);

}

int run_fastrt(
               double* doserates,
               int startWavelength,
               int endWavelength,
               double stepWavelength,
               int dayinyear,
               double latitude,
               double longitude,
               double altitude,
               int seconds_from_midnight,
               int sky_condition_type,
               bool silent
               )
{
    if (!silent)
    {
        printf("Lat %f, long %f, alt %f, Seconds from midnight %d", latitude, longitude, altitude, seconds_from_midnight);
    }
    double zenith = solar_zenith(seconds_from_midnight % 86400,
                                 dayinyear,
                                 latitude,
                                 -1 * longitude,
                                 0.0);

    if (zenith > 90.0 || zenith < 0.0)
        return 1;
    
//    double zenith = fmin( hour_angle(seconds_from_midnight) , 90.0);
    char s_StartWavelength[10];
    snprintf(s_StartWavelength, 10, "%d", startWavelength);
    
    char s_EndWavelength[10];
    snprintf(s_EndWavelength, 10, "%d", endWavelength);
    
    char s_StepWavelength[10];
    snprintf(s_StepWavelength, 10, "%.2f", stepWavelength);
    
    char s_Zenith[10];
    snprintf(s_Zenith, 10, "%.3f", zenith);
    
    //ozone column ([100,600] DU)
    char s_Ozone[10];
    snprintf(s_Ozone, 10, "%.1f", 400.0);
    
    //Range [0.0-0.4] (AOD = βλµm-1.3).
    char s_AngstromBeta[10];
    snprintf(s_AngstromBeta, 10, "%.2f", 0.2);
    
    // km, Range [5-350] km
    char s_Visibility[10];
    snprintf(s_Visibility, 10, "%d", 50);
    
    //%, Range [0-100].
    char s_CloudFraction[10];
    snprintf(s_CloudFraction, 10, "%d", 50);
    
    //g m-2, Range [0-5000].
    char s_CloudLiquidWaterColumn[10];
    snprintf(s_CloudLiquidWaterColumn, 10, "%d",
             sky_condition_type == 1 ? 50   :
             sky_condition_type == 2 ? 450 :
              45);
    
    //g m-2, Range [0-5000]. (Thin clouds: LWP < 50 g m-2; Thick clouds: LWP > 500 g m-2)
    char s_CloudLiquidWaterPath[10];
    snprintf(s_CloudLiquidWaterPath, 10, "%d", 100);
    
    
    // nm, Range [0.05-55.0] nm, multiples of 0.05 nm
    char s_FWHM[10];
    snprintf(s_FWHM, 10, "%.2f", 0.6);
    
    //Range [0.0-1.0].
    char s_Albedo[10];
    snprintf(s_Albedo, 10, "%.2f", 0.03);

    //km. Range [0.0-6.0] km
    char s_Altitude[10];
    snprintf(s_Altitude, 10, "%.3f", altitude);
    
    char s_DayOfYear[10];
    snprintf(s_DayOfYear, 10, "%d", dayinyear);
    
    // cloudless
    if (sky_condition_type == 0)
    {
        char* argv[] = {"fastrt",
            "-a", s_Zenith,
            "-v", s_Visibility,
            "-d", s_DayOfYear,
            "-o", s_Ozone,
            "-g", s_StartWavelength,
            "-e", s_EndWavelength,
            "-s", s_StepWavelength,
            "-f", s_FWHM,
            "-p", s_Albedo,
            "-z", s_Altitude
        };
        int argc = 21;
        if (!silent)
            print_debug_params(argc, argv);
        return run_fastrt_(argc, argv, doserates);
    }
    // scattered clouds
    else if (sky_condition_type == 1)
    {
        char* argv[] = {"fastrt",
            "-u", s_CloudLiquidWaterColumn,
            "-a", s_Zenith,
            "-d", s_DayOfYear,
            "-o", s_Ozone,
            "-g", s_StartWavelength,
            "-e", s_EndWavelength,
            "-s", s_StepWavelength,
            "-f", s_FWHM,
            "-p", s_Albedo,
            "-z", s_Altitude
        };
        int argc = 21;
        if (!silent)
            print_debug_params(argc, argv);
        return run_fastrt_(argc, argv, doserates);
    }
    // broken clouds
    else if (sky_condition_type == 2)
    {
        char* argv[] = {"fastrt",
            "-c",
            "-u", s_CloudLiquidWaterColumn,
            "-a", s_Zenith,
            "-d", s_DayOfYear,
            "-o", s_Ozone,
            "-g", s_StartWavelength,
            "-e", s_EndWavelength,
            "-s", s_StepWavelength,
            "-f", s_FWHM,
            "-p", s_Albedo,
            "-z", s_Altitude
        };
        int argc = 22;
        if (!silent)
            print_debug_params(argc, argv);
        return run_fastrt_(argc, argv, doserates);
    }
    // overcast
    else if (sky_condition_type == 3)
    {
        char* argv[] = {"fastrt",
            "-u", s_CloudLiquidWaterColumn,
            "-a", s_Zenith,
            "-d", s_DayOfYear,
            "-o", s_Ozone,
            "-g", s_StartWavelength,
            "-e", s_EndWavelength,
            "-s", s_StepWavelength,
            "-f", s_FWHM,
            "-p", s_Albedo,
            "-z", s_Altitude
        };
        int argc = 21;
        if (!silent)
            print_debug_params(argc, argv);
        return run_fastrt_(argc, argv, doserates);
    }
}
