#!/usr/bin/python3

from pyparsing import Literal, Word, Regex, nums, alphanums, Group, ZeroOrMore, OneOrMore, Optional
import sys

import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle

#define grammar
sizes       = []
coordinates = []

header        = Regex(r"^UCLA.*")
comment       = Regex(r"^#.*")
num_nodes     = Regex(r"^NumNodes.*")
num_terminals = Regex(r"^NumTerminals.*")
node_id       = Word(alphanums)
width         = Word(nums).setParseAction(lambda tokens : int(tokens[0])).setResultsName("width")
height        = Word(nums).setParseAction(lambda tokens : int(tokens[0])).setResultsName("height")
x             = Word(nums).setParseAction(lambda tokens : int(tokens[0])).setResultsName("x")
y             = Word(nums).setParseAction(lambda tokens : int(tokens[0])).setResultsName("y")
node          = node_id + Group(width  + height) + Optional(Literal("terminal"))
node.setParseAction( lambda tokens : sizes.append( tokens[1] ) )
node_grammar  = header + ZeroOrMore(comment) + num_nodes + num_terminals + OneOrMore(node)
pl            = node_id + Group(x + y) + Literal(": N") + Optional(Literal(r"/FIXED"))
pl.setParseAction( lambda tokens : coordinates.append( tuple(tokens[1]) ) )
pl_grammar    = header + ZeroOrMore(comment) + OneOrMore(pl)

if __name__ == "__main__":
    node_file_name = sys.argv[1]
    pl_file_name   = sys.argv[2]


    #ret = comment.parseString("#comment1 helel\n#coment2elwe\n#comment3 elwle")
    #print(ret)
    #ret = node_grammar.parseString('''UCLA nodes 1.0\n#comment 1 helloworld\n#comment 2 ldsfjl''')
    #print(ret)

    node_grammar.parseFile(node_file_name)
    #pl_grammar.parseFile(pl_file_name)

    plt.figure()
    axis = plt.gca()
    for i in range(len(sizes)):
        axis.addPatch( Rectangle(coordinates[i], sizes[0], sizes[1]) )
    plt.show()

