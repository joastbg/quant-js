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

#include <boost/numeric/ublas/hermitian.hpp>
#include <boost/numeric/ublas/io.hpp>

int main(int argc, char* argv[]) {

    /*
    using namespace boost::numeric::ublas;
    hermitian_matrix<std::complex<double>, lower> ml (3, 3);
    for (unsigned i = 0; i < ml.size1 (); ++ i) {
        for (unsigned j = 0; j < i; ++ j)
            ml (i, j) = std::complex<double> (3 * i + j, 3 * i + j);
        ml (i, i) = std::complex<double> (4 * i, 0);
    }
    std::cout << ml << std::endl;
    hermitian_matrix<std::complex<double>, upper> mu (3, 3);
    for (unsigned i = 0; i < mu.size1 (); ++ i) {
        mu (i, i) = std::complex<double> (4 * i, 0);
        for (unsigned j = i + 1; j < mu.size2 (); ++ j)
            mu (i, j) = std::complex<double> (3 * i + j, 3 * i + j);
    }
    std::cout << mu << std::endl;
    
    vector<double> v (3);
    for (unsigned i = 0; i < v.size (); ++ i)
        v (i) = i;
    std::cout << v << std::endl;
    std::cout << v*10.0 << std::endl;

    matrix<double> m (3, 3);
    for (unsigned i = 0; i < m.size1 (); ++ i)
        for (unsigned j = 0; j < m.size2 (); ++ j)
            m (i, j) = 3 * i + j;
    std::cout << m << std::endl;
    */

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
