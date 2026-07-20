/*******************************************************************************
 * Copyright (c) 2024 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution
 * License is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Daniel Claudino - initial API and implementation
 *******************************************************************************/

 // The ANTLR parser is overkill for such a simple parsing need
 // and it happens to be way more efficient to turn to a simple regex parser
 // most of this struct is somewhat syntatic sugar 
#ifndef QUANTUM_PAULIOPERATOR_PARSER_HPP_
#define QUANTUM_PAULIOPERATOR_PARSER_HPP_

#include <complex>
#include <regex>
#include <map>
#include <string>
#include <algorithm>

struct PauliOperatorParser {

    const std::regex matchPattern;
    const std::regex sitePauliPattern;

    // Ctor initializes regex patterns
    PauliOperatorParser()
        : matchPattern(std::string(signPattern) + "|" + complexPattern + "|" + realPattern + "|" +
                       stringPattern + "|" + identityPattern1 + "|" + identityPattern2 + "|" +
                       identityPattern3 + "|" + identityPattern4 + "|" + identityPattern5),
          sitePauliPattern(R"(([XYZ])(\d+))") {}

    // Enum for match types
    enum class MatchType {
        SIGN,
        COMPLEX_AND_P,
        REAL_AND_P,
        P_ONLY,
        IDENTITY_ONLY,
        REAL_AND_I,
        COMPLEX_AND_I,
        REAL_ONLY,
        COMPLEX_ONLY,
        UNKNOWN // this raises error!
    };

    // Determine the type of match
    MatchType getMatchType(const std::smatch &match) const {
        if (match[1].matched) return MatchType::SIGN;
        if (match[2].matched && match[3].matched && match[4].matched) return MatchType::COMPLEX_AND_P;
        if (match[5].matched && match[6].matched) return MatchType::REAL_AND_P;
        if (match[7].matched) return MatchType::P_ONLY;
        if (match[8].matched) return MatchType::IDENTITY_ONLY;
        if (match[9].matched && match[10].matched) return MatchType::REAL_AND_I;
        if (match[11].matched && match[12].matched && match[13].matched) return MatchType::COMPLEX_AND_I;
        if (match[14].matched) return MatchType::REAL_ONLY;
        if (match[15].matched && match[16].matched) return MatchType::COMPLEX_ONLY;
        return MatchType::UNKNOWN;
    }

    // Parse Pauli String into a map<int, std::string>
    void parsePauliString(const std::string &str, std::map<int, std::string> &pauliMap) const {
        std::smatch match;
        std::string::const_iterator searchStart(str.cbegin());
        while (std::regex_search(searchStart, str.cend(), match, sitePauliPattern)) {
            pauliMap.emplace(std::stoi(match[2].str()), match[1].str());
            searchStart = match.suffix().first;
        }
    }

    // remove white space from strings in-place
    static void removeWhiteSpaces(std::string &str) {
        str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    }

private:
    // Regex patterns
    static constexpr const char *complexPattern =
        R"(\s*\(\s*([\d.eE+-]+)\s*,\s*([\d.eE+-]+)\s*\)\s*((?:[XYZ]\d+\s*)+))";
    static constexpr const char *realPattern = R"(\s*([\d.eE+-]+)\s*((?:[XYZ]\d+\s*)+))";
    static constexpr const char *stringPattern = R"(\s*((?:[XYZ]\d+\s*)+))";
    static constexpr const char *signPattern = R"(\s*([+-]\s*))";
    static constexpr const char *identityPattern1 = R"(\s*(I(?!\d)\s*))";
    static constexpr const char *identityPattern2 = R"(\s*([\d.eE+-]+)\s*(I(?!\d)\s*))";
    static constexpr const char *identityPattern3 =
        R"(\s*\(\s*([\d.eE+-]+)\s*,\s*([\d.eE+-]+)\s*\)\s*(I(?!\d)\s*))";
    static constexpr const char *identityPattern4 = R"(\s*([\d.eE+-]+)\s*)";
    static constexpr const char *identityPattern5 =
        R"(\s*\(\s*([\d.eE+-]+)\s*,\s*([\d.eE+-]+)\s*\)\s)";
};

#endif  // QUANTUM_PAULIOPERATOR_PARSER_HPP_
