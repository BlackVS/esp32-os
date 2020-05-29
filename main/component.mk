#
# "main" pseudo-component makefile.
#
# (Uses default behaviour of compiling all source files in directory, adding 'include' to include path.)

#COMPONENT_ADD_INCLUDEDIRS := . 

COMPONENT_SRCDIRS := . 
COMPONENT_SRCDIRS += console
COMPONENT_SRCDIRS += hardware
COMPONENT_SRCDIRS += boards
COMPONENT_SRCDIRS += boards/nnc2019
COMPONENT_SRCDIRS += boards/m5stickc
COMPONENT_SRCDIRS += boards/TTGO
COMPONENT_SRCDIRS += resources

COMPONENT_ADD_INCLUDEDIRS := .
COMPONENT_ADD_INCLUDEDIRS += console
COMPONENT_ADD_INCLUDEDIRS += hardware
COMPONENT_ADD_INCLUDEDIRS += boards
COMPONENT_ADD_INCLUDEDIRS += boards/nnc2019
COMPONENT_ADD_INCLUDEDIRS += boards/m5stickc
COMPONENT_ADD_INCLUDEDIRS += boards/TTGO
COMPONENT_ADD_INCLUDEDIRS += resources

COMPONENT_ADD_LDFLAGS += -z muldefs