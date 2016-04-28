#!/usr/bin/python3

import os
import pathlib
import numpy as np

benchmark_path = pathlib.Path(os.environ['BENCHMARK'])
file_names = ["adaptec1", "adaptec2", "adaptec3", "adaptec4", "bigblue1", "bigblue2", "bigblue3", "bigblue4"]

with open("chip_size.txt", 'w') as out_file:

    lines = []
    for fn in file_names:
        node_file_name = benchmark_path / "ispd2005/{0}/{0}.nodes".format(fn)
        pl_file_name   = benchmark_path / "ispd2005/{0}/{0}.pl".format(fn)
        print(node_file_name.as_posix())
        print(pl_file_name.as_posix())

        sizes       = np.genfromtxt(node_file_name.as_posix(), skip_header=6, usecols=(1,2))
        coordinates = np.genfromtxt(pl_file_name.as_posix(),   skip_header=4, usecols=(1,2))

        x_max,y_max = 0,0

        for i in range(len(sizes)):
            if coordinates[i][0] + sizes[i][0] > x_max:
                x_max = coordinates[i][0] + sizes[i][0]
            if coordinates[i][1] + sizes[i][1] > y_max:
                y_max = coordinates[i][1] + sizes[i][1]

        lines.append( "{0} width : {1} | height : {2}\n".format(fn, x_max, y_max) )

    out_file.writelines(lines)

