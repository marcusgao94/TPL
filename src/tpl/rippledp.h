/*--------------------------------------------------*/
/*  Ripple-DP (ISPD2014 contest version)              */
/*  Copyright (c) 2014                              */
/*  Department of Computer Science and Engineering  */
/*  The Chinese Univeristy of Hong Kong             */
/*                                                  */
/*  Contact:                                        */
/*  Wing-Kai Chow <wkchow@cse.cuhk.edu.hk>          */
/*  Evangeline F.Y. Young <fyyoung@cse.cuhk.edu.hk> */
/*--------------------------------------------------*/

#ifndef _RIPPLEDP_H_
#define _RIPPLEDP_H_

/*read benchmarks and placed global placement solution*/
void rippledp_read(char *tech_file, char *cell_file, char *floorplan_file, char *placed_file);

/*Perform displacement-constrained legalization and detailed placement*/
/* target_util = target utility */
/* max_disp    = maximum displacement constraint */
void rippledp(double target_util, double max_disp);

/*write placement result in DEF format*/
void rippledp_write(char *output_file);

#endif

