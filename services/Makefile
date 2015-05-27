# Set the following:
CAFFE_ROOT := ../nips14_loc_seg_testonly/Caffe_Segmentation/
CUDA_ROOT := /usr/local/cuda
CPU_ONLY := 0

CXXFLAGS += -std=c++0x -fopenmp -O2 -g
LIBS += -L$(CAFFE_ROOT)/build/lib -L/usr/local/lib/
LDFLAGS += -lcaffe -lglog -lboost_program_options -lboost_filesystem -lboost_system -lopencv_core -lopencv_imgproc -lopencv_imgcodecs
INC += -I$(CAFFE_ROOT)/include/ -I$(CAFFE_ROOT)/build/src/ -I/usr/local/include/opencv2/ -I../ver2/
ifneq ($(CPU_ONLY),1)
    LIBS += -L$(CUDA_ROOT)/lib64
    INC += -I$(CUDA_ROOT)/include
    LDFLAGS += -lcudart -lcublas -lcurand
else
	CPPFLAGS += -DCPU_ONLY
endif

all: $(patsubst %.cpp, %.bin, $(wildcard *.cpp))

%.bin: %.cpp Makefile
	$(CXX)  \
		$(CXXFLAGS) \
    $(INC) \
		$< -o $@ \
		$(LIBS) \
		$(LDFLAGS) 

clean:
	rm -rf *.bin
