
srcfiles 	:= $(wildcard src/*/*.cpp)
objects		:= $(patsubst %.cpp, %.o, $(srcfiles))

INCLUDE = -I./include -I./include/mesh 



all:
	g++ -o 

	
