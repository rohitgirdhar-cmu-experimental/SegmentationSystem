CXXFLAGS += -std=c++0x -O2 -g
LDFLAGS += `pkg-config --libs /home/rgirdhar/data/Software/vision/opencv_install/lib/pkgconfig/opencv.pc`
LIBS := -L/home/rgirdhar/data/Software/vision/opencv_install/lib/
INC += -I/home/rgirdhar/data/Software/vision/opencv_install/include/

all: overlaySeg.bin

%.bin: %.cpp Makefile
	$(CXX)  \
		$(CXXFLAGS) \
		$(INC) \
		$< -o $@ \
		$(LIBS) \
		$(LDFLAGS) 

clean:
	rm -rf *.bin
