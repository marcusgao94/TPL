#!/usr/bin/python3

import os
import pathlib
import sys

from pyparsing import Literal, Word, Regex, nums, alphanums, Group, ZeroOrMore, OneOrMore, Optional, Suppress


class NodeParser:
    def __init__(self):
        self.num = Word(nums)

        self.header = Regex(r"^UCLA.*")
        self.comment = Regex(r"#.*")
        self.bkid = Word(alphanums)

        self.num_nodes = Literal("NumNodes") + Literal(":") + self.num("NumNodes")
        self.num_terminals = Literal("NumTerminals") + Literal(":") + self.num("NumTerminals")
        self.size = Group(self.num("width") + self.num("height"))
        self.terminal = Optional(Literal("terminal"))
        self.node = self.bkid("id") + self.size("size") + self.terminal
        self.node_grammar = self.header + ZeroOrMore(self.comment) + self.num_nodes + self.num_terminals + \
                                OneOrMore(self.node)

        self.coordinate = Group(self.num("x") + self.num("y"))
        self.pl = self.bkid("id") + self.coordinate("coordinate") + \
                        Suppress(Literal(": N") + Optional(Literal(r"/FIXED")))
        self.pl_grammar = self.header + ZeroOrMore(self.comment) + OneOrMore(self.pl)

    def compute_chip_size(self, benchmark):
        benchmark_path = pathlib.Path(os.environ["BENCHMARK"])
        node_file = benchmark_path / "ispd2005/{0}/{0}.nodes".format(benchmark)
        pl_file = benchmark_path / "ispd2005/{0}/{0}.pl".format(benchmark)
        print(node_file.as_posix())
        print(pl_file.as_posix())

        x_max = 0
        y_max = 0
        sizes = []
        coordinates = []

        self.size.setParseAction(lambda tokens: sizes.append([tokens.width, tokens.height]))
        self.coordinate.setParseAction(lambda tokens: coordinates.append((tokens.x, tokens.y)))
        self.bkid.setParseAction(lambda tokens: print(tokens[0]))

        self.node_grammar.parseFile(node_file.as_posix())
        self.pl_grammar.parseFile(pl_file.as_posix())

        for i in range(len(sizes)):
            print(i)
            if coordinates[i][0] + sizes[i][0] > x_max:
                x_max = coordinates[i][0] + sizes[i][0]
            if coordinates[i][1] + sizes[i][1] > y_max:
                y_max = coordinates[i][1] + sizes[i][1]

        return x_max, y_max


if __name__ == '__main__':
    parser = NodeParser()
    width, height = parser.compute_chip_size(sys.argv[1])
    print(width, height)
