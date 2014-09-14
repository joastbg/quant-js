#ifndef OPTIONS_H
#define OPTIONS_H

#define DEFAULT_THREADS 2
#define DEFAULT_WIDTH 800
#define DEFAULT_HEIGHT 600

#include <string>
#include <iostream>

class Options {
public:
	Options() : threads(DEFAULT_THREADS), width(DEFAULT_WIDTH), height(DEFAULT_HEIGHT), tilesize(32), outfile("out.tga"), infile("-"), scene(0), statistics(true) { }
	int threads;
	int width;
	int height;
	int tilesize;
	std::string outfile;
	std::string infile;
	int scene;
	bool statistics;

	bool validate() const;

	friend std::ostream& operator<<(std::ostream& out, const Options& o);
};

#endif
