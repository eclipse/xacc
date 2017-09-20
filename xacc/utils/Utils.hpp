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

static inline bool is_base64(unsigned char c);

std::string base64_decode(std::string const& encoded_string);

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

template<typename TupleType, typename FunctionType>
void tuple_for_each(TupleType&&, FunctionType,
		std::integral_constant<size_t,
				std::tuple_size<typename std::remove_reference<TupleType>::type>::value>) {
}

template<std::size_t I, typename TupleType, typename FunctionType,
		typename = typename std::enable_if<
				I
						!= std::tuple_size<
								typename std::remove_reference<TupleType>::type>::value>::type>
void tuple_for_each(TupleType&& t, FunctionType f,
		std::integral_constant<size_t, I>) {
	f(std::get<I>(t));
	tuple_for_each(std::forward<TupleType>(t), f,
			std::integral_constant<size_t, I + 1>());
}

template<typename TupleType, typename FunctionType>
void tuple_for_each(TupleType&& t, FunctionType f) {
	tuple_for_each(std::forward<TupleType>(t), f,
			std::integral_constant<size_t, 0>());
}

class XACCException: public std::exception {
protected:

	std::string errorMessage;

public:

	XACCException(std::string error) :
			errorMessage(error) {
		spdlog::get("xacc-console")->error("\033[1;31m" + errorMessage + "\033[0m");			\
	}

	virtual const char * what() const throw () {
		return "";//errorMessage.c_str();
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
			c->info("\033[1;34m" + msg + "\033[0m");
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
