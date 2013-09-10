# Define the compiler options
CXXFLAGS  = -pipe -O2
CXXFLAGS += -pthread -Iinclude -I/opt/local/include/ -L/opt/local/lib/ -lQuantLib

# Define what to do when make is executed without arguments.
all: quantjs
	g++ -o quantjs -pthread main.o libv8.a  -L. -lv8 -Iinclude -I/opt/local/include/ -L/opt/local/lib/ -lQuantLib
quantjs: main.o

# Define the rules for the object files.
main.o: main.cc
	g++ -c $(CXXFLAGS) main.cc

clean:
	rm -rf *o quantjs
		
run:
	./quantjs algo1.js
		
runa:
	./quantjs algo1.js
