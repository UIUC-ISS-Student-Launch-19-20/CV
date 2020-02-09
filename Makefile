CXX = clang++
FLAGS = -O

OPENCV = `pkg-config opencv4 --cflags --libs`
LIBS = $(OPENCV)

main : main.cpp
	$(CXX) $(FLAGS) $(LIBS) -o $@ $<

clean :
	rm main
