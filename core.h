#ifndef CORE_H
#define CORE_H

#include <v8.h>
#include <string.h>
#include "options.h"

class Core {
public:
    Core(Options options) : options(options), promptLine(0) {}
    void run();
	void execute(const std::string& str);
	void RunShell();

	std::string promptString();

private:
    const Options& options;
	v8::Persistent<v8::Context> context;
	int promptLine;
};

#endif
