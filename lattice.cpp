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
	Lattice::aval_type maxAvalSize = 0;
	Lattice::aval_type resetFreq = 0;
	Lattice::write_option_set writeOptions;
	std::string avalFileName;
	bool clusterStat = false;
	bool inverseClusterStat = false;

	int res = 0;
	while ((res = getopt(argc, argv, "12345a:cim:n:p:r:s:x:z:")) != -1) {
		switch (res){
		case '1':
			writeOptions.insert(Lattice::WriteOption::AvalancheSize);
			break;
		case '2':
			writeOptions.insert(Lattice::WriteOption::AvalancheLength);
			break;
		case '3':
			writeOptions.insert(Lattice::WriteOption::AverageZBefore);
			break;
		case '4':
			writeOptions.insert(Lattice::WriteOption::AverageZAfter);
			break;
		case '5':
			writeOptions.insert(Lattice::WriteOption::ExcitedCount);
			break;
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
		case 'r':
			resetFreq = atoi(optarg);
			break;
		case 's':
			skip = atoi(optarg);
			break;
		case 'x':
			maxAvalSize = atoi(optarg);
			break;
		case 'z':
			zc = atoi(optarg);
			break;
		case '?':
			std::cerr << "Usage: lattice <params>\n"
				"\t-1 \twrite avalanche size\n"
				"\t-2 \twrite avalanche length\n"
				"\t-3 \twrite <z> before avalanche\n"
				"\t-4 \twrite <z> after avalanche\n"
				"\t-5 \twrite number of excited nodes\n"
				"\t-a <path>\tavalanche file name\n"
				"\t-i \tinverse cluster statistics\n"
				"\t-c \tadd cluster statistics to avalanche file\n"
				"\t-m <int>\tlattice side size\n"
				"\t-n <int>\tnumber of runs\n"
				"\t-p <float>\tprobability of activity\n"
				"\t-r <int>\tactivity reset frequency\n"
				"\t-s <int>\tnumber of dry runs\n"
				"\t-x <int>\tmax avalanche size\n"
				"\t-z <int>\tcritical value\n";
			return 1;
			break;
        }
	}

	const long seed = std::chrono::system_clock::now().time_since_epoch().count();
	Lattice calc(seed, M, zc, p, maxAvalSize, resetFreq);

	if (skip > 0) {
		calc.run(nullptr, skip, false, false, writeOptions);
	}

	std::vector<unsigned> avalanches;
	{
		const std::unique_ptr<std::ostream> avalFile(avalFileName.empty() ? nullptr : new std::ofstream(avalFileName));
		avalanches = calc.run(avalFile.get(), N, clusterStat, inverseClusterStat, writeOptions);
	}

//	const std::vector<unsigned> untailed = Statistics::cutTail(data, 2);
//	std::vector<double> normalizedData = Statistics::normalize(untailed);
//	Statistics::statistics(&std::cout, normalizedData);
	//Statistics::dist(std::cout, normalizedData, 0.1, normalizedData.size() / 1000);
//	Statistics::write(std::cout, normalizedData);

	return 0;
}
