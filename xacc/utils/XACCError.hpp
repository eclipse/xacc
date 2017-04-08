#ifndef UTILS_XACCERROR_HPP_
#define UTILS_XACCERROR_HPP_

#include <exception>
#include <sstream>

namespace xacc {

class XACCException: public std::exception {
protected:

	std::string errorMessage;

public:

	XACCException(std::string error) :
			errorMessage(error) {
	}

	virtual const char * what() const throw () {
		return errorMessage.c_str();
	}

	~XACCException() throw () {
	}
};

#define XACC_Abort do {std::abort();} while(0);

#define XACCError(errorMsg)												\
	{																	\
    	throw xacc::XACCException("\n\n XACC Error caught! \n\n"	\
            	+ std::string(errorMsg) + "\n\n");						\
	}

}
#endif
