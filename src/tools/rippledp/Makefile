#--------------------------------------------------#
#  Ripple-DP (ISPD2014 contest version)            #
#  Copyright (c) 2014                              #
#  Department of Computer Science and Engineering  #
#  The Chinese Univeristy of Hong Kong             #
#                                                  #
#  Contact:                                        #
#  Wing-Kai Chow <wkchow@cse.cuhk.edu.hk>          #
#  Evangeline F.Y. Young <fyyoung@cse.cuhk.edu.hk> #
#--------------------------------------------------#

OPT= -O2 -DNDEBUG
#OPT= -O0 -ggdb
TYPE= -static
#WFLAG= -Wall -Winline

CC= g++ $(OPT) $(TYPE) $(WFLAG) $(DEBUG)

LIBS= -lm -pthread

SRCS = ${OBJS:%.o=%.c}
BFILE = rippledp

all:    $(BFILE)

$(BFILE): main.o rippledp.a libdef.a liblef.a
	$(CC) -o $(BFILE) main.o rippledp.a libdef.a liblef.a $(LIBS)

%.o : %.c %.h
	$(CC) -c $*.c


clean:
	rm -f *.o $(BFILE) core
