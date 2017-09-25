/*******************************************************************************
 * Copyright (c) 2017 UT-Battelle, LLC.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompanies this
 * distribution. The Eclipse Public License is available at
 * http://www.eclipse.org/legal/epl-v10.html and the Eclipse Distribution License
 * is available at https://eclipse.org/org/documents/edl-v10.php
 *
 * Contributors:
 *   Alexander J. McCaskey - initial API and implementation
 *******************************************************************************/
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
