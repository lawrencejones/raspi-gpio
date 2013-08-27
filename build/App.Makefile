###############################################################################
# Common Makefile Template
# 
# == Building Apps ==
# Provides several targets, which do the following...
#
#   all: builds all target applications
#   clean: removes any binaries, .o or final
#
# == Creating New Apps ==
# To define a Makefile for a new application, the apps Makefile need only
# include this file and specify a few variables. These are...
#
#   BUILDROOT: the absolute path of the source tree
#   APPNAME: name of the final executable
#   APP_SRC: list of the source files inside the app folder
#   MODULES: list of required modules by folder
#
# Any specified modules should be the name of the folder in which the
# module source is housed. These module folders should be located in
# the src directory inside the BUILDROOT folder.
#
# Once these variables have been specified, the new apps Makefile should
# contain the line `$(BUILDROOT)/build/Makefile.App`, where BUILDROOT is
# the top level directory as stated above.
# 
# Be careful not to define any rules, as they will become the default target.
# 
# == Design notes ==
# As a guide, this is suitable for projects that...
#
#   -  Have a strong expectation of source tree layout
#   -  All modules are a single folder at the top level under 'src'.
#   -  The module folder contains any interface header files and a dir named
#     'src' that contains any implementation.
#   -  
# That implementation directory is automatically globbed for .c files to built.
# Additional files can be specified by providing an option Makefile that will
# be included if found. #
#
# An example of a scheduler might look like the following...
#
# <src>
#   |- <sched>
#   .    |- sched.h                         - public interface
#   .    |- <src>                           - source folder
#   .         |- sched_private.h            - private interface
#
# == Limitations (A TODO list) ==
# There is no dependency on header files. In the first instance, all code in 
# modules should depend on their own internal and public header files, and 
# possibly all code should depend on public header files. In the longer term a
# system of dependency between modules would be desirable (whether auto or 
# hand specified), and the appropriate interface header dependencies could 
# then be specified.
#
# Automatic platform detection.


###############################################################################

# The default target
all : $(APPNAME)

# Don't use existing suffix rules. We'll define our own patterns.
.SUFFIXES:

# Initialise variables:
APPLIBS :=
OBJECTS := 

# Build objects for the app source itself:
OBJECTS += $(addsuffix .o,$(basename $(APP_SRC)))

# Do platform specific setup
# For the moment, assume linux
PLATFORM ?= LINUX

# Select compiler
CC = gcc

# Legacy comand line support:
ifeq ($(DEBUG),true)
override DEBUG := 1
endif

# Debug or release build:
ifeq ($(DEBUG), 1)
CFLAGS += -O0 -g
$(info Debug Build)
else
$(info Release Build)
endif

# General build options:
CFLAGS += -I$(BUILDROOT)/src \
          -DTARGET_$(TARGET) \
          -Wall -pedantic -Wextra \
          -std=c99 \
          -fPIC \
          -D_POSIX_C_SOURCE=200803L
          
# Initialise LDFLAGS
LDFLAGS +=

# Allow for out of tree builds:
VPATH = $(BUILDROOT)/src

# Per module processing:
define MODULE_TEMPLATE

# Find all C files - VPATH will take care of finding them
OBJECTS += $(addprefix $(BUILDROOT)/src/,$(addsuffix .o,$(basename $(1))))
# Any extra customisation:

endef

# Call the template for each module:
$(foreach m,$(MODULES),$(eval $(call MODULE_TEMPLATE,$(m))))

# Pattern rule for build object files
%.o : %.c
	@echo Compiling $(<F)
	@-rm -f $@
	@$(CC) -c $(CFLAGS) $< -o $@

# Rule for linking
$(APPNAME) : $(OBJECTS)
	@echo Linking '$@'
	@-rm -f $@
	@$(CC) -o $(APPNAME) $(OBJECTS) $(APPLIBS) $(CFLAGS) $(CPPFLAGS) $(LDFLAGS)
	@echo 'Finished building target: $@'

clean :
	-rm -f $(OBJECTS)

rebuild : clean all

.PHONY: all clean

###############################################################################
# Disabling of default rules
###############################################################################
# Disabling these help to remove some noise from the output of the 'print 
# database' command line option (helpful when debugging odd issues).
%:: RCS/%,v
%:: RCS/%
%:: s.%
%:: SCCS/s.%

###############################################################################
# End
###############################################################################
