#
# University of California, Santa Barbara
# HumanActivity Recognition {Walking,Biking}
# 
# Authors:
# Norma S. Savage   <saiph@cs.ucsb.edu>
# Victor M. Fragoso <vfragoso@cs.ucsb.edu>
#

# Makefile for Project CS/ECE 281b
CC        = g++
LD        = $(CC)
CXXFLAGS  = -Wall -pedantic -I. 
OPENCV_INC=`pkg-config --cflags opencv`
OPENCV_LIB=`pkg-config --libs opencv`
LDLIBS    = $(OPENCV_LIB)

INCLUDE= $(OPENCV_INC) \
	-I./include/utils \
	-I./include/tracking \

CXXFLAGS += $(INCLUDE)

OBJDIR_DEF = obj
OBJDIR_DEB = obj_d
BINDIR_DEF = bin
BINDIR_DEB = bin_d

ifeq "$(visual)" "y"
	CXXFLAGS += -DVISUAL
endif

ifeq "$(debug)" "y"
	CXXFLAGS += -g -ggdb -DDEBUG
	OBJDIR   = $(OBJDIR_DEB)
	BINDIR   = $(BINDIR_DEB)
else
	CXXFLAGS += -march=i686 -O3 -ftree-vectorize -funroll-loops -ffast-math
	OBJDIR   = $(OBJDIR_DEF)
	BINDIR   = $(BINDIR_DEF)	
endif


### generic targets
$(OBJDIR)/Tracking/%.o: ./Tracking/%.cpp
	if [ ! -f $@ ] ; then mkdir -p $@ ; rmdir $@; fi ; \
    $(CXX) $< -o $@ -c $(CXXFLAGS)

$(OBJDIR)/utils/%.o: ./utils/%.cpp
	if [ ! -f $@ ] ; then mkdir -p $@ ; rmdir $@; fi ; \
    $(CXX) $< -o $@ -c $(CXXFLAGS)

$(OBJDIR)/Recognition/%.o: ./Recognition/%.cc
	if [ ! -f $@ ] ; then mkdir -p $@ ; rmdir $@; fi ; \
    $(CXX) $< -o $@ -c $(CXXFLAGS)

$(OBJDIR)/Descriptors/%.o: ../Descriptors/%.cpp
	if [ ! -f $@ ] ; then mkdir -p $@ ; rmdir $@; fi ; \
    $(CXX) $< -o $@ -c $(CXXFLAGS)

$(OBJDIR)/%.o: %.cpp
	if [ -f $@ ] ; then echo -n; else mkdir -p $@ ; rmdir $@; fi ; \
    $(CXX) $< -o $@ -c $(CXXFLAGS)
### generic targets

OBJS=\
	$(OBJDIR)/Tracking/Tracker.o \
	$(OBJDIR)/Tracking/L1MinTracker.o \
	$(OBJDIR)/Tracking/MeanShift.o \
	$(OBJDIR)/utils/utils.o \
	main.o 

TARGET=recog

#Targets
all: main

main: $(TARGET)
	$(LD) -o $(TARGET) $(OBJS) $(LDLIBS)

$(TARGET) : $(OBJS)

clean:
	rm *.o
	rm -rf $(OBJDIR_DEF) $(OBJDIR_DEB)
	rm -rf $(BINDIR_DEF) $(BINDIR_DEB)
	rm $(TARGET)



