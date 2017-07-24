#ifndef QUANTUM_AQC_COMPILER_EMBEDDING_HPP_
#define QUANTUM_AQC_COMPILER_EMBEDDING_HPP_

#include <map>
#include <vector>
#include <boost/algorithm/string.hpp>

namespace xacc {

namespace quantum {

class Embedding : public std::map<int, std::vector<int>> {

public:

	void persist(std::ostream& stream) {
		for (auto& kv : *this) {
			stream << kv.first << ": ";
			for (int i = 0; i < kv.second.size(); i++) {
				if (i == kv.second.size()-1) {
					stream << kv.second[i];
				} else {
					stream << kv.second[i] << " ";
				}
			}
			stream << "\n";
		}
	}

	void load(std::istream& stream) {
		std::string s(std::istreambuf_iterator<char>(stream), {});

		std::vector<std::string> splitNewLine, splitColon, splitSpaces;
		boost::split(splitNewLine, s, boost::is_any_of("\n"));

		for (auto line : splitNewLine) {
			if (!line.empty()) {
				boost::split(splitColon, line, boost::is_any_of(":"));
				auto probVert = std::stoi(splitColon[0]);
				std::vector<int> hardwareVerts;
				boost::split(splitSpaces, splitColon[1], boost::is_any_of(" "));
				for (auto i : splitSpaces) {
					if (!i.empty()) {
						hardwareVerts.push_back(std::stoi(i));
					}
				}
				insert(std::make_pair(probVert, hardwareVerts));
			}
		}

		return;
	}

};

}

}


#endif
