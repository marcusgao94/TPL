#include "rippledp.h"

int main(int argc, char **argv){
    /* read benchmark files: tech.lef, cells.lef, floorplan.def */
    /* read global placement solution: placed.def */
    rippledp_read((char*) "tech.lef", (char*) "cells.lef", (char*) "floorplan.def", (char*) "placed.def");

    /* detailed placement with target utility and maximum displacement constraint */
    rippledp(0.8, 200000);

    /* write the detailed placement solution to output file */
    rippledp_write((char*)"dplaced.def");

    return 0;
}

