#pragma once

#include <vector>
#include <map>
#include <iostream>
#include <algorithm>

class Statistics {
public:

	template <typename T>
	static std::vector<T> cutTail(const std::vector<T>& raw, const unsigned /* minValue */) {
		std::vector<T> result = raw;
		//result.resize(result.size() - result.size() / 10);

		return result;
	}

	template <typename T>
	static std::vector<double> normalize(const std::vector<T>& raw) {
		std::vector<double> data;

		decltype(data)::value_type sum = 0.0;
		for(auto v : raw) {
			const decltype(data)::value_type vv = log10(v);
			sum += vv;
			data.push_back(vv);
		}

		if (!data.empty()) {
			const auto mean = sum / data.size();
			const auto mult = 1 / mean;
			for (auto& v : data) {
				v *= mult;
			}
		}

		return data;
	}

	static double median(const std::vector<double>& data) {
		const size_t n = data.size() / 2;

		if (data.size() % 2) {
			return data[n];
		} else {
			return 0.5 * (data[n - 1] + data[n]);
		}
	}

	static void statistics(std::ostream* out, std::vector<double>& data) {
		double sum1 = 0.0, sum2 = 0.0, sum3 = 0.0, sum4 = 0.0;

		for(auto v : data) {
			sum1 += v;
			sum2 += v * v;
			sum3 += v * v * v;
			sum4 += v * v * v * v;
		}

		const double m1 = sum1 / data.size();
		const double m2 = sum2 / data.size();
		const double m3 = sum3 / data.size();
		const double m4 = sum4 / data.size();

		sum1 = 0.0, sum2 = 0.0, sum3 = 0.0, sum4 = 0.0;
		for(auto v : data) {
			auto c = v - m1;
			sum2 += c * c;
			sum3 += c * c * c;
			sum4 += c * c * c * c;
		}

		const double cm2 = sum2 / data.size();
		const double cm3 = sum3 / data.size();
		const double cm4 = sum4 / data.size();

		std::sort(data.begin(), data.end());
		const double first = data[data.size() / 10];
		const double med = median(data);
		const double last = data[data.size() - data.size() / 10];

		if (out) {
			*out << "#\t" << data.size() << '\t' << first << '\t' << med << '\t' << last << '\t' << m1 << '\t' << m2 << '\t' << m3 << '\t' << m4 << '\t' << cm2 << '\t' << cm3 << '\t' << cm4 << std::endl;
		}
	}

	static void write(std::ostream& out, const std::vector<double>& data) {
		for (auto v : data) {
			out << v << '\n';
		}
		out << std::endl;
	}

	static void dist(std::ostream& out, const std::vector<double>& data, const double frame, const unsigned threshold) {
		std::map<unsigned, unsigned> dist;

		for (auto v : data) {
			const unsigned h = floor(v / frame);
			dist[h]++;
		}

		for (auto v : dist) {
			if (v.second >= threshold) {
				out << (v.first + 1) * frame << '\t' << v.second << '\n';
			}
		}
		out << std::endl;
	}

};
