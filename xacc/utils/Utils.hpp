/***********************************************************************************
 * Copyright (c) 2017, UT-Battelle
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the xacc nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *   Initial API and implementation - Alex McCaskey
 *
 **********************************************************************************/
#ifndef XACC_UTILS_UTILS_HPP_
#define XACC_UTILS_UTILS_HPP_

#include <boost/bind.hpp>
#include <boost/tokenizer.hpp>
#include "spdlog/spdlog.h"

namespace xacc {

template<typename InputIterator, typename OutputIterator, typename Predicate>
OutputIterator copy_if(InputIterator first, InputIterator last,
		OutputIterator result, Predicate pred) {
	while (first != last) {
		if (pred(*first))
			*result++ = *first;
		++first;
	}
	return result;
}

/**
 * Tokenize a string.  The tokens will be separated by each non-quoted
 * space or equal character.  Empty tokens are removed.
 *
 * @param input The string to tokenize.
 *
 * @return Vector of tokens
 */
inline std::vector<std::string> tokenize(const std::string& input) {
	typedef boost::escaped_list_separator<char> separator_type;
	separator_type separator("\\", "= ", "\"\'");
	// Tokenize the input.
	boost::tokenizer<separator_type> tokens(input, separator);
	// Copy non-empty tokens from the tokenizer into the result.
	std::vector<std::string> result;
	xacc::copy_if(tokens.begin(), tokens.end(), std::back_inserter(result),
			!boost::bind(&std::string::empty, _1));
	return result;
}

template <typename Tuple, typename F, std::size_t ...Indices>
void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>) {
    using swallow = int[];
    (void)swallow{1,
        (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...
    };
}

template <typename Tuple, typename F>
void tuple_for_each(Tuple&& tuple, F&& f) {
    constexpr std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
    for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                  std::make_index_sequence<N>{});
}

template<
  typename Tuple,
  typename Indices=std::make_index_sequence<std::tuple_size<Tuple>::value>>
struct runtime_get_func_table;

template<typename Tuple,size_t ... Indices>
struct runtime_get_func_table<Tuple,std::index_sequence<Indices...>>{
    using return_type=typename std::tuple_element<0,Tuple>::type&;
    using get_func_ptr=return_type (*)(Tuple&) noexcept;
    static constexpr get_func_ptr table[std::tuple_size<Tuple>::value]={
        &std::get<Indices>...
    };
};

template<typename Tuple,size_t ... Indices>
constexpr typename
runtime_get_func_table<Tuple,std::index_sequence<Indices...>>::get_func_ptr
runtime_get_func_table<Tuple,std::index_sequence<Indices...>>::table[std::tuple_size<Tuple>::value];

template<typename Tuple>
constexpr
typename std::tuple_element<0,typename std::remove_reference<Tuple>::type>::type&
tuple_runtime_get(Tuple&& t,size_t index){
    using tuple_type=typename std::remove_reference<Tuple>::type;
    if(index>=std::tuple_size<tuple_type>::value)
        throw std::runtime_error("Out of range");
    return runtime_get_func_table<tuple_type>::table[index](t);
}


class XACCException: public std::exception {
protected:

	std::string errorMessage;

public:

	XACCException(std::string error) :
			errorMessage(error) {
		spdlog::get("xacc-console")->error(errorMessage);			\
	}

	virtual const char * what() const throw () {
		return errorMessage.c_str();
	}

	~XACCException() throw () {
	}
};

#define XACC_Abort do {std::abort();} while(0);

class XACCInfoT {
public:
	static void printInfo(const std::string& msg) {
		auto c = spdlog::get("xacc-console");
		if (c) {
			c->info(msg);
		}
	}
};

#define XACCError(errorMsg)												\
	do {																\
		using namespace xacc; 											\
    	throw XACCException("\n\n XACC Error caught! \n\n"	 		    \
            	+ std::string(errorMsg) + "\n\n");						\
	} while(0)

#define XACCInfo(infoMsg)												\
	do {																\
		xacc::XACCInfoT::printInfo(std::string(infoMsg));						\
	} while(0)


}
#endif
