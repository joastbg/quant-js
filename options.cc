#include "options.h"

bool Options::validate() const 
{
	// Check input file is valid
	if (infile == "-") {
		std::cerr << "ERROR: No input file or scene given" << std::endl;
		return false;
	}
	return true;
}

std::ostream& operator<<(std::ostream& out, const Options& o) 
{
	if (!o.debug)	
		return out;

	out << std::boolalpha;
	out << std::endl;
	out << "-- Options" << std::endl;
	//out << "options.threads:\t" << o.threads << std::endl;
	//out << "options.width:\t\t" << o.width << std::endl;
	//out << "options.height:\t\t" << o.height << std::endl;
	//out << "options.outfile:\t" << o.outfile << std::endl;
	out << "options.infiles:\t\t" << o.infile << std::endl;
	//out << "options.scene:\t\t" << o.scene << std::endl;
	//out << "options.statistics:\t" << o.statistics;
	return out;
}
