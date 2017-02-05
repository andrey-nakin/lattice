#pragma once

#include <vector>
#include <iostream>
#include <numeric>
#include <random>
#include "statistics.hpp"

class Lattice {
public:
	typedef float probability_type;
	typedef int value_type;
	typedef std::vector<value_type> value_vector_type;
	typedef value_vector_type::size_type size_type;
	typedef value_vector_type::size_type index_type;
	typedef unsigned aval_type;
	typedef std::vector<aval_type> aval_vector_type;

private:

	const size_type M;	//	размер стороны решетки
	const value_type zc;		//	критическое значение z в узле
	const std::vector<probability_type> probOfActivity;
	const aval_type maxAvalSize;	// max possible avalanche size

	std::vector<unsigned char> active;
	value_vector_type values;

    std::mt19937 generator;
    std::uniform_int_distribution<index_type> indexGenerator;
    std::uniform_real_distribution<probability_type> probGenerator;

	static std::vector<probability_type> makeProbabilities(const size_type L, const probability_type p) {
		decltype(probOfActivity) result(L, p);
		return result;
	}

	bool isLowest(const index_type index) const {
		return index < M;
	}

	bool isUpper(const index_type index) const {
		return index >= values.size() - M;
	}

	bool isLeft(const index_type index) const {
		return index % M == 0;
	}

	bool isRight(const index_type index) const {
		return index % M == M - 1;
	}

	index_type lower(const index_type index) const {
		return index - M;
	}

	index_type higher(const index_type index) const {
		return index + M;
	}

	index_type rightOf(const index_type index) const {
		return index + 1;
	}

	index_type leftOf(const index_type index) const {
		return index - 1;
	}

	void pass(const index_type i, const bool inverseClusterStat, unsigned& size, std::vector<unsigned char>& passed) const {
		if (!passed[i]) {
			passed[i] = 1;

			if ((active[i] && !inverseClusterStat) || (!active[i] && inverseClusterStat)) {
				size++;

				if (!isLowest(i)) {
					pass(lower(i), inverseClusterStat, size, passed);
				}
				if (!isUpper(i)) {
					pass(higher(i), inverseClusterStat, size, passed);
				}
				if (!isLeft(i)) {
					pass(leftOf(i), inverseClusterStat, size, passed);
				}
				if (!isRight(i)) {
					pass(rightOf(i), inverseClusterStat, size, passed);
				}
			}
		}
	}

public:

	Lattice(const long seed, const size_type M, const value_type zc, const probability_type activeProb, const aval_type maxAvalSize) :
		M(M),
		zc(zc),
		probOfActivity(makeProbabilities(M * M, activeProb)),
		maxAvalSize(maxAvalSize),
		active(probOfActivity.size()),
		values(probOfActivity.size()),
		generator(seed),
		indexGenerator(0, values.size() - 1),
		probGenerator(0.0, 1.0) {
	}

	double avgValue() const {
		return 1.0 / values.size() * std::accumulate(values.begin(), values.end(), 0);
	}

	aval_type avalanche() {
		decltype(avalanche()) size = 0;	//	размер лавины

	    // добавляем z в случайный узел}
		values[indexGenerator(generator)] += 1;

		//	выбираем, какие узлы будут активны в данной лавине
		for (index_type i = 0; i < active.size(); i++) {
			active[i] = probGenerator(generator) < probOfActivity[i] ? 1 : 0;
		}

		auto tmpValues = values;

		bool endava;
		do {
			endava = true;

			for (index_type i = 0; i < values.size(); i++) {
				if (active[i] && values[i] >= zc) {
					endava = false;	//	отмечаем, что лавина не закончилась

					if (!isLowest(i)) {
						tmpValues[lower(i)] += 1;
					}
					if (!isUpper(i)) {
						tmpValues[higher(i)] += 1;
					}
					if (!isLeft(i)) {
						tmpValues[leftOf(i)] += 1;
					}
					if (!isRight(i)) {
						tmpValues[rightOf(i)] += 1;
					}

					tmpValues[i] -= 4;

					size++;	//	считаем размер лавины
				}
			}

			if (!endava) {
				values = tmpValues;
			}
		} while (!endava);

		return size;
	}

	std::vector<unsigned> clusters(const bool inverseClusterStat) const {
		std::vector<unsigned> result;
		std::vector<unsigned char> passed(active.size(), 0);

		for (index_type i = 0; i < active.size(); i++) {
			if (!passed[i]) {
				unsigned size = 0;
				pass(i, inverseClusterStat, size, passed);
				if (size > 0) {
					result.push_back(size);
				}
			}
		}

		return result;
	}

	aval_vector_type run(std::ostream* const out, const aval_vector_type::size_type numOfRuns, const bool clusterStat, const bool inverseClusterStat) {
		aval_vector_type result;
		result.reserve(numOfRuns);

		if (out) {
			*out << "# av.size\tavg.z.0\tavg.z";

			if (clusterStat) {
				*out << '\t';
				if (inverseClusterStat) {
					Statistics::statisticsHeader(out, "icl.");
				} else {
					Statistics::statisticsHeader(out, "cl.");
				}
			}

			*out << std::endl;
		}

		auto prevAvgValue = avgValue();
		for (auto run = numOfRuns; run > 0; run--) {
			const auto avSize = avalanche();
			result.push_back(avSize);

			if (out) {
				const auto currAvgValue = avgValue();
				*out << avSize << '\t' << prevAvgValue << '\t' << currAvgValue;
				prevAvgValue = currAvgValue;

				if (clusterStat) {
					std::vector<unsigned> cs = clusters(inverseClusterStat);
					Statistics::statistics(out, cs);
				}

				*out << std::endl;
			}
		}

		if (out) {
			*out << std::endl;
		}

		return result;
	}

};
