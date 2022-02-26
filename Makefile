# #
# # 'make depend' uses makedepend to automatically generate dependencies 
# #               (dependencies are added to end of Makefile)
# # 'make'        build executable file 'libc-http'
# # 'make clean'  removes all .o and executable files
# #

# The current working directory
PWD := $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))


# Define the C compiler to use
CC := gcc

# define any compile-time flags
CFLAGS := -Wextra -Wall -Wshadow -g

# define any directories containing header files other than /usr/include
INCLUDES := -I src/headers

# define the C source files
SRCS := $(shell find $(PWD) -name '*.c')

# define the C object files 
#
# This uses Suffix Replacement within a macro:
#   $(name:string1:=string2)
#         For each word in 'name' replace 'string1' with 'string2'
# Below we are replacing the suffix .c of all words in the macro SRCS
# with the .o suffix
#
# OBJS := $(SRCS:.c:=.o)

# define the executable file 
MAIN := libc-http

# Defines the values for valgrind
VALFLAGS := --leak-check:=full -s
VALCMD := ./libc-http http://www.neverssl.com/?query:=1

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean

$(MAIN): $(SRCS) 
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS) -o $(MAIN) 

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c.o:
	$(CC) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

test:
	make && valgrind $(VALFLAGS) $(VALCMD)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it