//
//  fastrtlib.h
//  sunshine-helper
//
//  Created by Hunter Weeks on 7/1/22.
//

#ifndef fastrtlib_h
#define fastrtlib_h

#include <stdbool.h>
#include "numeric.h"
#include "regress.h"
#include "ascii.h"
#include "spl.h"
#include "table.h"
#include "sun.h"
#include "integrat.h"
#include "function.h"
#include "cnv.h"
#include "equation.h"
#include "fastrt_.h"

int run_fastrt_test_inputs(double *doserates);

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
               );

#endif /* fastrtlib_h */
