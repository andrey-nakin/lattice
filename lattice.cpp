#include <chrono>
#include <string>
#include <memory>
#include <fstream>
#include <unistd.h>
#include "lattice.hpp"
#include "statistics.hpp"

int main(const int argc, char* const* const argv) {
	Lattice::aval_vector_type::size_type N = 10000;
	Lattice::aval_vector_type::size_type skip = 100000;
	Lattice::size_type M = 100;
	Lattice::probability_type p = 0.8;
	Lattice::value_type zc = 5;
	std::string avalFileName;
	bool clusterStat = false;
	bool inverseClusterStat = false;

	int res = 0;
	while ((res = getopt(argc, argv, "a:cim:n:p:s:z:")) != -1) {
		switch (res){
		case 'a':
			avalFileName = optarg;
			break;
		case 'c':
			clusterStat = true;
			break;
		case 'i':
			inverseClusterStat = true;
			break;
		case 'm':
			M = atoi(optarg);
			break;
		case 'n':
			N = atoi(optarg);
			break;
		case 'p':
			p = atof(optarg);
			break;
		case 's':
			skip = atoi(optarg);
			break;
		case 'z':
			zc = atoi(optarg);
			break;
		case '?':
			std::cerr << "Usage: lattice <params>\n"
				"\t-a <path>\tavalanche file name\n"
				"\t-i \tinverse cluster statistics\n"
				"\t-c \tadd cluster statistics to avalanche file\n"
				"\t-m <int>\tlattice side size\n"
				"\t-n <int>\tnumber of runs\n"
				"\t-p <float>\tprobability of activity\n"
				"\t-s <int>\tnumber of dry runs\n"
				"\t-z <int>\tcritical value\n";
			return 1;
			break;
        }
	}

	const long seed = std::chrono::system_clock::now().time_since_epoch().count();
	Lattice calc(seed, M, zc, p);

	if (skip > 0) {
		calc.run(nullptr, skip, false, false);
	}

	std::vector<unsigned> avalanches;
	{
		const std::unique_ptr<std::ostream> avalFile(avalFileName.empty() ? nullptr : new std::ofstream(avalFileName));
		avalanches = calc.run(avalFile.get(), N, clusterStat, inverseClusterStat);
	}

//	const std::vector<unsigned> untailed = Statistics::cutTail(data, 2);
//	std::vector<double> normalizedData = Statistics::normalize(untailed);
//	Statistics::statistics(&std::cout, normalizedData);
	//Statistics::dist(std::cout, normalizedData, 0.1, normalizedData.size() / 1000);
//	Statistics::write(std::cout, normalizedData);

	return 0;
}
