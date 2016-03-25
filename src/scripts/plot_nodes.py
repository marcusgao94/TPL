#!/usr/bin/python3

import os
import sys
import time

from pyparsing import Literal, Word, Regex, nums, alphanums, Group, ZeroOrMore, OneOrMore, Optional

#import matplotlib
#matplotlib.use('GTKAgg')

import numpy as np
import matplotlib.pyplot as plt
from matplotlib.collections import PatchCollection
from matplotlib.patches import Rectangle


#define globals
x_max = 0
y_max = 0

ids         = []
sizes       = []
coordinates = []

#define sematic actions
def add_id(s, loc, tok):
    ids.append( tok[0] )

def add_size(s, loc, tok):
    sizes.append( tok[1] )
    #print( '{0}\t\t : width({1}) height({2})'.format(tok[0], tok[1][0], tok[1][1]) )

def add_coordinate(s, loc, tok):
    coordinates.append( tuple(tok[1]) )
    #print( '{0}\t\t : x({1}) y({2})'.format(tok[0], tok[1][0], tok[1][1]) )

#define grammars
###########################################################################################################################
#========================================================common rule======================================================#
###########################################################################################################################
header        = Regex(r"^UCLA.*")#.setParseAction( lambda tokens : print( tokens[0]) )                                    #
comment       = Regex(r"#.*"    )#.setParseAction( lambda tokens : print( tokens[0]) )                                    #
node_id       = Word(alphanums  ).setParseAction( add_id )                                                                #
###########################################################################################################################
#========================================================node   rule======================================================#
###########################################################################################################################
num_nodes     = Regex(r"NumNodes.*"    )#.setParseAction( lambda tokens : print( tokens[0]) )                             #
num_terminals = Regex(r"NumTerminals.*")#.setParseAction( lambda tokens : print( tokens[0]) )                             #
width         = Word(nums).setParseAction(lambda tokens : int(tokens[0]))#.setResultsName("width")                        #
height        = Word(nums).setParseAction(lambda tokens : int(tokens[0]))#.setResultsName("height")                       #
node          = ( node_id + Group(width  + height) + Optional(Literal("terminal")) ).setParseAction(add_size)             #
node_grammar  = header + ZeroOrMore(comment) + num_nodes + num_terminals + OneOrMore(node)                                #
###########################################################################################################################
#========================================================pl     rule======================================================#
###########################################################################################################################
x             = Word(nums).setParseAction(lambda tokens : int(tokens[0])).setResultsName("x")                             #
y             = Word(nums).setParseAction(lambda tokens : int(tokens[0])).setResultsName("y")                             #
pl            = ( node_id + Group(x + y) + Literal(": N") + Optional(Literal(r"/FIXED")) ).setParseAction(add_coordinate) #
pl_grammar    = header + ZeroOrMore(comment) + OneOrMore(pl)                                                              #
###########################################################################################################################
###########################################################################################################################

if __name__ == "__main__":
    time1 = time.time()

    node_file_name = sys.argv[1]
    pl_file_name   = sys.argv[2]

    sizes       = np.genfromtxt(node_file_name, skip_header=6, usecols=(1,2))
    coordinates = np.genfromtxt(pl_file_name,   skip_header=4, usecols=(1,2))
    #node_grammar.parseFile(node_file_name)
    #pl_grammar.parseFile(pl_file_name)

    time2 = time.time();

    plt.figure()
    axes = plt.gca()

    patches = []

    for i in range(len(sizes)):
        #print( 'Add node {0} to figure.'.format(ids[i]) )

        if coordinates[i][0] + sizes[i][0] > x_max:
            x_max = coordinates[i][0] + sizes[i][0]
        if coordinates[i][1] + sizes[i][1] > y_max:
            y_max = coordinates[i][1] + sizes[i][1]

        patches.append( Rectangle(coordinates[i], sizes[i][0], sizes[i][1], fill=False, color='b') )

    collection = PatchCollection(patches)
    axes.add_collection(collection)
    plt.xlim(0, x_max)
    plt.ylim(0, y_max)

    png_file_name = os.path.basename(node_file_name).replace('.nodes', '.png')
    plt.savefig('./{0}'.format(png_file_name), format='png', dpi=1000)
    time3 = time.time()

    print('Parsing  time : {0}'.format(time2-time1))
    print('Plotting time : {0}'.format(time3-time2))

