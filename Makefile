#
# University of California, Santa Barbara
# HumanActivity Recognition {Walking,Biking}
# 
# Authors:
# Norma S. Savage   <saiph@cs.ucsb.edu>
# Victor M. Fragoso <vfragoso@cs.ucsb.edu>
#

# Makefile for Project CS/ECE 281b
CXX       = g++
CC        = gcc
LD        = $(CXX)
CXXFLAGS  = -Wall -pedantic -I. 
OPENCV_INC=`pkg-config --cflags opencv-2.1`
OPENCV_LIB=`pkg-config --libs opencv-2.1`
IPPINC=opt/intel/Compiler/11.1/056/include/
IPPLIBHOME=-L/opt/intel/Compiler/11.1/056/lib/ia32
IPPLIBS   = $(IPPLIBHOME) \
	-lcxaguard  -lguide  -lguide_stats  -limf  -lintlc  -liomp5 \
	-liompprof5  -liompstubs5  -lirc  -lomp_db  -lompstub  \
	-lpdbx  -lsvml -lpthread
#LDLIBS    = $(OPENCV_LIB) $(IPPLIBS)
LDLIBS    = $(OPENCV_LIB)

INCLUDE= $(OPENCV_INC) \
	-I./include/utils \
	-I./include/tracking \
	-I./include/motion_analysis \
	-I./include/descriptors \

	# -I./3rdParty/fast-C-src-2.1/ \

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
$(OBJDIR)/3rdParty/fast-C-src-2.1/%.o: ./3rdParty/fast-C-src-2.1/%.c
	if [ ! -f $@ ] ; then mkdir -p $@ ; rmdir $@; fi ; \
    $(CC) $< -o $@ -c $(CXXFLAGS)

$(OBJDIR)/Tracking/%.o: ./Tracking/%.cpp
	if [ ! -f $@ ] ; then mkdir -p $@ ; rmdir $@; fi ; \
    $(CXX) $< -o $@ -c $(CXXFLAGS)

$(OBJDIR)/utils/%.o: ./utils/%.cpp
	if [ ! -f $@ ] ; then mkdir -p $@ ; rmdir $@; fi ; \
    $(CXX) $< -o $@ -c $(CXXFLAGS)

$(OBJDIR)/Recognition/%.o: ./Recognition/%.cc
	if [ ! -f $@ ] ; then mkdir -p $@ ; rmdir $@; fi ; \
    $(CXX) $< -o $@ -c $(CXXFLAGS)

$(OBJDIR)/Descriptors/%.o: ./Descriptors/%.cpp
	if [ ! -f $@ ] ; then mkdir -p $@ ; rmdir $@; fi ; \
    $(CXX) $< -o $@ -c $(CXXFLAGS)

$(OBJDIR)/MotionAnalysis/%.o: ./MotionAnalysis/%.cpp
	if [ ! -f $@ ] ; then mkdir -p $@ ; rmdir $@; fi ; \
    $(CXX) $< -o $@ -c $(CXXFLAGS)

$(OBJDIR)/%.o: %.cpp
	if [ -f $@ ] ; then echo -n; else mkdir -p $@ ; rmdir $@; fi ; \
    $(CXX) $< -o $@ -c $(CXXFLAGS)
### generic targets

OBJS=\
	$(OBJDIR)/Tracking/Tracker.o \
	$(OBJDIR)/Tracking/MeanShift.o \
	$(OBJDIR)/Tracking/LucasKanade.o \
	$(OBJDIR)/utils/utils.o \
	$(OBJDIR)/MotionAnalysis/MotionAnalyzer.o \
	$(OBJDIR)/Descriptors/hog.o \
#	$(OBJDIR)/MotionAnalysis/LagrangeInterpolator.o \
#	$(OBJDIR)/MotionAnalysis/BSplineFitting.o \
	# $(OBJDIR)/utils/Histogram.o \
	# $(OBJDIR)/utils/FASTCVWrapper.o \
	# $(OBJDIR)/Tracking/L1MinTracker.o \
	# $(OBJDIR)/3rdParty/fast-C-src-2.1/fast.o \
	# $(OBJDIR)/3rdParty/fast-C-src-2.1/fast_9.o \
	# $(OBJDIR)/3rdParty/fast-C-src-2.1/fast_10.o \
	# $(OBJDIR)/3rdParty/fast-C-src-2.1/fast_11.o \
	# $(OBJDIR)/3rdParty/fast-C-src-2.1/fast_12.o \
	# $(OBJDIR)/3rdParty/fast-C-src-2.1/nonmax.o \
#	main.o 

TARGET=recog

#Targets
all: main

main: $(TARGET)
	$(CXX) -c main.cpp $(CXXFLAGS) -o $(OBJDIR)/main.o
	$(LD) -o $(TARGET) $(OBJS) $(OBJDIR)/main.o $(LDLIBS)

$(TARGET) : $(OBJS)

testTracking: $(OBJS) test/MeanShiftTest.cpp test/LucasKanadeTest.cpp
	@echo \<\<MeanShiftTracking Test\>\>
	$(CXX) -c test/MeanShiftTest.cpp $(CXXFLAGS) -o test/MeanShiftTest.o
	$(LD) -o test/meanShiftTracking $(OBJS) test/MeanShiftTest.o $(LDLIBS)
	@echo \<\<LucasKanadeTracking Test\>\>
	$(CXX) -c test/LucasKanadeTest.cpp $(CXXFLAGS) -o test/LucasKanadeTest.o
	$(LD) -o test/lkTracking $(OBJS) test/LucasKanadeTest.o $(LDLIBS)

testMotion: $(OBJS) test/testMotionAnalyzer.cpp
	$(CXX) -c test/testMotionAnalyzer.cpp $(CXXFLAGS) -o test/testMotionAnalyzer.o
	$(LD) -o test/testManal $(OBJS) test/testMotionAnalyzer.o $(LDLIBS)


testSVM: test/HocTest.cpp
	$(CXX) -c -g -ggdb test/HocTest.cpp $(CXXFLAGS) -o test/HocTest.o
	$(LD) -o test/hogTest $(OBJS) test/HocTest.o $(LDLIBS)

clean:
	rm -rf $(OBJDIR_DEF) $(OBJDIR_DEB)
	rm -rf $(BINDIR_DEF) $(BINDIR_DEB)
	rm *.o
	rm $(TARGET)



