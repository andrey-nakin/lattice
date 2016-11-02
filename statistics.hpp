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

	template <typename T>
	static double median(const std::vector<T>& data) {
		const std::size_t n = data.size() / 2;

		if (data.size() % 2) {
			return data[n];
		} else {
			return 0.5 * (data[n - 1] + data[n]);
		}
	}

	static void statisticsHeader(std::ostream* out, const char* const prefix) {
		if (out) {
			*out
			<< prefix << "size"
			<< '\t' << prefix << "median"
			<< '\t' << prefix << "min"
			<< '\t' << prefix << "max"
			<< '\t' << prefix << "mean"
			<< '\t' << prefix << "m2"
			<< '\t' << prefix << "m3"
			<< '\t' << prefix << "m4"
			<< '\t' << prefix << "var"
			<< '\t' << prefix << "cm3"
			<< '\t' << prefix << "cm4";
		}
	}

	template <typename T>
	static void statistics(std::ostream* out, std::vector<T>& data) {
		if (data.empty()) {
			return;
		}

		double sum1 = 0.0, sum2 = 0.0, sum3 = 0.0, sum4 = 0.0;
		T min = data[0], max = data[0];

		for(auto v : data) {
			sum1 += v;
			sum2 += v * v;
			sum3 += v * v * v;
			sum4 += v * v * v * v;

			if (v < min) {
				min = v;
			}
			if (v > max) {
				max = v;
			}
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
		const double med = median(data);

		if (out) {
			*out << "\t" << data.size() << '\t' << med << '\t' << min << '\t' << max << '\t' << m1 << '\t' << m2 << '\t' << m3 << '\t' << m4 << '\t' << cm2 << '\t' << cm3 << '\t' << cm4;
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
