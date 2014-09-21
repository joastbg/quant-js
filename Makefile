# Define the compiler options
CXXFLAGS  = -pipe -O2
CXXFLAGS += -pthread -Iinclude -I/opt/local/include/ -I/usr/include/eigen3/

# Define what to do when make is executed without arguments.
all: quantjs
	clang++ -o quantjs -pthread main.o options.o core.o -lv8 -Iinclude -I/usr/include/eigen3/ -I/opt/local/include/ -L/opt/local/lib/ -lQuantLib -lboost_thread -lboost_date_time -lboost_system -lglut -lGL -lGLU -lGLEW -lX11
quantjs: options.o core.o main.o

# Define the rules for the object files.
main.o: main.cc
	clang++ -c $(CXXFLAGS) main.cc -std=c++11

options.o: options.cc
	clang++ -c $(CXXFLAGS) options.cc -std=c++11

core.o: core.cc
	clang++ -c $(CXXFLAGS) core.cc -std=c++11

clean:
	rm -rf *o quantjs
		
run:
	./quantjs quantjs.js
		
runa:
	./quantjs quantjs.js
