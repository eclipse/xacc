#include <boost/bind.hpp>
#include <boost/tokenizer.hpp>

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
std::vector<std::string> tokenize(const std::string& input) {
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
void for_each(Tuple&& tuple, F&& f) {
    constexpr std::size_t N = std::tuple_size<std::remove_reference_t<Tuple>>::value;
    for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
                  std::make_index_sequence<N>{});
}
}
