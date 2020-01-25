CXX = clang++
FLAGS = -O

OPENCV = `pkg-config opencv --cflags --libs`
LIBS = $(OPENCV)

main : main.cpp
	$(CXX) $(FLAGS) $(LIBS) -o $@ $<

clean :
	rm main
