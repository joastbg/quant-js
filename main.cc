#include <v8.h>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <string>
#include <vector>
#include <stdio.h>
#include <functional>
#include <stdlib.h>
#include <thread>
#include <chrono>
#include <pthread.h>

#include "core.h"

#define QJS_VERSION "0.0.4"

void banner() {
    std::cout << "quantjs v" << QJS_VERSION << std::endl;
    std::cout << "Copyright 2013-2014 Johan Astborg" << std::endl;
}

void usage() {
    std::cout << "usage: quantjs [--infiles filename(s)...] [--help]" << std::endl;
}

int main(int argc, char* argv[]) {

    Options options;
    if (argc == 1) usage();    
    for (int i = 1; i < argc; ++i) {
        if (!strncmp(argv[i], "--infiles", 8)) options.infile = argv[++i];
        if (!strncmp(argv[i], "--help", 6) || !strncmp(argv[i], "-h", 2)) usage();
        if (!strncmp(argv[i], "--debug", 7) || !strncmp(argv[i], "-d", 2)) options.debug=true;
    }

    banner();

    Core Core(options);
    Core.run();
    
    return 0;
}
