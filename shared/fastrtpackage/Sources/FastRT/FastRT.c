//
//  File.c
//  
//
//  Created by Hunter Weeks on 7/1/22.
//

#include "FastRT.h"
#include "fastrt_.h"

int woo_yeah(void)
{
    char* argv[] = {"fastrt", "-a", "57.5", "-o", "315.", "-w", "300.15", "-d", "80", "-f", "0.6", "-z", "0.73"};
    return run_fastrt(13, argv);
}
