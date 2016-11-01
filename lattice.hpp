#pragma once

#include <vector>
#include <iostream>
#include <numeric>
#include <random>

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

public:

	Lattice(const long seed, const size_type M, const value_type zc, const probability_type activeProb) :
		M(M),
		zc(zc),
		probOfActivity(makeProbabilities(M * M, activeProb)),
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

	aval_vector_type run(std::ostream* const out, const aval_vector_type::size_type numOfRuns) {
		aval_vector_type result;
		result.reserve(numOfRuns);

		for (auto run = numOfRuns; run > 0; run--) {
			const auto avSize = avalanche();
			result.push_back(avSize);

			if (out) {
				*out << avSize << '\t' << avgValue() << std::endl;
			}
		}

		return result;
	}

};
