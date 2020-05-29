/******************************************************************/ /**
                                                                      * \file
                                                                      *direct_iterator.hpp
                                                                      * \author
                                                                      *Elliot
                                                                      *Goodrich
                                                                      *
                                                                      * Boost
                                                                      *Software
                                                                      *License -
                                                                      *Version 1.0
                                                                      *- August
                                                                      *17th,
                                                                      *2003
                                                                      *
                                                                      * Permission
                                                                      *is hereby
                                                                      *granted,
                                                                      *free of
                                                                      *charge,
                                                                      *to any
                                                                      *person or
                                                                      *organization
                                                                      * obtaining
                                                                      *a copy of
                                                                      *the
                                                                      *software
                                                                      *and
                                                                      *accompanying
                                                                      *documentation
                                                                      *covered
                                                                      *by this
                                                                      *license
                                                                      *(the
                                                                      *"Software")
                                                                      *to use,
                                                                      *reproduce,
                                                                      *display,
                                                                      *distribute,
                                                                      * execute,
                                                                      *and
                                                                      *transmit
                                                                      *the
                                                                      *Software,
                                                                      *and to
                                                                      *prepare
                                                                      *derivative
                                                                      *works of
                                                                      *the
                                                                      * Software,
                                                                      *and to
                                                                      *permit
                                                                      *third-parties
                                                                      *to whom
                                                                      *the
                                                                      *Software
                                                                      *is
                                                                      *furnished
                                                                      *to do so,
                                                                      *all
                                                                      *subject
                                                                      *to the
                                                                      *following:
                                                                      *
                                                                      * The
                                                                      *copyright
                                                                      *notices
                                                                      *in the
                                                                      *Software
                                                                      *and this
                                                                      *entire
                                                                      *statement,
                                                                      *including
                                                                      * the
                                                                      *above
                                                                      *license
                                                                      *grant,
                                                                      *this
                                                                      *restriction
                                                                      *and the
                                                                      *following
                                                                      *disclaimer,
                                                                      * must be
                                                                      *included
                                                                      *in all
                                                                      *copies of
                                                                      *the
                                                                      *Software,
                                                                      *in whole
                                                                      *or in
                                                                      *part, and
                                                                      * all
                                                                      *derivative
                                                                      *works of
                                                                      *the
                                                                      *Software,
                                                                      *unless
                                                                      *such
                                                                      *copies or
                                                                      *derivative
                                                                      * works
                                                                      *are
                                                                      *solely in
                                                                      *the form
                                                                      *of
                                                                      *machine-executable
                                                                      *object
                                                                      *code
                                                                      *generated
                                                                      *by a
                                                                      *source
                                                                      *language
                                                                      *processor.
                                                                      *
                                                                      * THE
                                                                      *SOFTWARE
                                                                      *IS
                                                                      *PROVIDED
                                                                      *"AS IS",
                                                                      *WITHOUT
                                                                      *WARRANTY
                                                                      *OF ANY
                                                                      *KIND,
                                                                      *EXPRESS
                                                                      *OR
                                                                      * IMPLIED,
                                                                      *INCLUDING
                                                                      *BUT NOT
                                                                      *LIMITED
                                                                      *TO THE
                                                                      *WARRANTIES
                                                                      *OF
                                                                      *MERCHANTABILITY,
                                                                      * FITNESS
                                                                      *FOR A
                                                                      *PARTICULAR
                                                                      *PURPOSE,
                                                                      *TITLE AND
                                                                      *NON-INFRINGEMENT.
                                                                      *IN NO
                                                                      *EVENT
                                                                      * SHALL
                                                                      *THE
                                                                      *COPYRIGHT
                                                                      *HOLDERS
                                                                      *OR ANYONE
                                                                      *DISTRIBUTING
                                                                      *THE
                                                                      *SOFTWARE
                                                                      *BE LIABLE
                                                                      * FOR ANY
                                                                      *DAMAGES
                                                                      *OR OTHER
                                                                      *LIABILITY,
                                                                      *WHETHER
                                                                      *IN
                                                                      *CONTRACT,
                                                                      *TORT OR
                                                                      *OTHERWISE,
                                                                      * ARISING
                                                                      *FROM, OUT
                                                                      *OF OR IN
                                                                      *CONNECTION
                                                                      *WITH THE
                                                                      *SOFTWARE
                                                                      *OR THE
                                                                      *USE OR
                                                                      *OTHER
                                                                      * DEALINGS
                                                                      *IN THE
                                                                      *SOFTWARE.
                                                                      *********************************************************************/

#ifndef INCLUDE_GUARD_ADE65E18_4D87_45D4_BF88_A95CC0B7DDAC
#define INCLUDE_GUARD_ADE65E18_4D87_45D4_BF88_A95CC0B7DDAC

#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

namespace ez {

namespace detail {

// is_defined<T> is aliased as std::true_type only if T is
template <typename>
using is_defined = std::true_type;

// is_true<bool X> has the typedef type defined as std::true_type only if X is
// true
template <bool>
struct is_true;

template <>
struct is_true<true> {
    typedef std::true_type type;
};

// calling is_iterator<T>(0) will return std::true_type if T has
// T::iterator_category defined and std::false_type otherwise
template <typename T>
auto is_iterator(int) -> decltype(
    is_defined<decltype(std::declval<typename T::iterator_category>())>());

template <typename T>
std::false_type is_iterator(long);

// calling has_difference_type<T>(0) will return std::true_type if T has
// T::difference_type defined and std::false_type otherwise
template <typename T>
auto has_difference_type(int) -> decltype(
    is_defined<decltype(std::declval<typename T::difference_type>())>());

template <typename T>
std::false_type has_difference_type(long);

// difference_type_impl decides what difference_type should be used for
// direct_iterator<T>
template <typename T, bool HasDifferenceType>
struct difference_type_impl {
    typedef std::ptrdiff_t type;
};

template <typename T>
struct difference_type_impl<T, true> {
    typedef typename T::difference_type type;
};

// difference_type forwards the correct arguments to difference_type_impl
template <typename T>
struct difference_type
    : difference_type_impl<T, std::remove_reference<decltype(
                                  has_difference_type<T>(0))>::type::value> {};

// calling input_compatible<T>(0) will return std::true_type if T is compatible
// with being a direct_iterator<T> which satisfies the InputIterator concepts
template <typename T>
auto input_compatible(int) -> decltype(
    // Iterator Concepts (most of these are checked when
    // Move constructible
    // Copy constructible
    // Move assignable
    // Copy assignable
    // Destructible
    // Swappable
    is_defined<decltype(++(std::declval<T&>()))>(), // Pre-incrementable
                                                    // InputIterator Concepts
    is_defined<decltype(std::declval<T&>() ==
                        std::declval<T&>())>(), // Equality
    is_defined<decltype(std::declval<T&>() !=
                        std::declval<T&>())>() // Inequality
    // Post-increment is implemented in terms of pre-increment
);

template <typename T>
std::false_type input_compatible(long);

// calling forward_compatible<T>(0) will return std::true_type if T is
// compatible with being a direct_iterator<T> which satisfies the
// ForwardIterator concepts
template <typename T>
auto forward_compatible(int) -> decltype(
    // ForwardIterator Concepts
    typename is_true<decltype(input_compatible<T>(0))::value>::type(),
    typename is_true<std::is_default_constructible<T>::value>::
        type() // Default constructible
);

template <typename T>
std::false_type forward_compatible(long);

// calling bidirectional_compatible<T>(0) will return std::true_type if T is
// compatible with being a direct_iterator<T> which satisfies the
// BidirectionalIterator concepts
template <typename T>
auto bidirectional_compatible(int) -> decltype(
    // ForwardIterator Concepts
    typename is_true<decltype(forward_compatible<T>(0))::value>::type(),
    // BidirectionalIterator Concepts
    is_defined<decltype(--std::declval<T&>())>() // Pre-decrementable
    // Post-decrement is implemented in terms of pre-decrement
);

template <typename T>
std::false_type bidirectional_compatible(long);

// calling random_access_compatible<T>(0) will return std::true_type if T is
// compatible with being a direct_iterator<T> which satisfies the
// RandomAccessIterator concepts
template <typename T>
auto random_access_compatible(int) -> decltype(
    // BidirectionalIterator Concepts
    typename is_true<decltype(bidirectional_compatible<T>(0))::value>::type(),
    // RandomAccessIterator Concepts
    is_defined<decltype(std::declval<T&>() +=
                        std::declval<typename difference_type<T>::type>())>(),
    is_defined<decltype(std::declval<T&>() +
                        std::declval<typename difference_type<T>::type>())>(),
    is_defined<decltype(std::declval<typename difference_type<T>::type&>() +
                        std::declval<T&>())>(),
    is_defined<decltype(std::declval<T&>() -=
                        std::declval<typename difference_type<T>::type>())>(),
    is_defined<decltype(std::declval<T&>() -
                        std::declval<typename difference_type<T>::type>())>(),
    is_defined<decltype(std::declval<T&>() - std::declval<T&>())>(),
    // Array index operator is defined in terms of *(t + n)
    is_defined<decltype(std::declval<T&>() < std::declval<T&>())>(),
    is_defined<decltype(std::declval<T&>() > std::declval<T&>())>(),
    is_defined<decltype(std::declval<T&>() <= std::declval<T&>())>(),
    is_defined<decltype(std::declval<T&>() >= std::declval<T&>())>());

template <typename T>
std::false_type random_access_compatible(long);

template <typename T, bool IsIterator, bool RandomAccess, bool Bidirectional,
          bool Forward, bool Input>
struct iterator_tag_impl;

// iterator_tag_impl decides what iterator_category should be used for
// direct_iterator<T>
template <typename T, bool RandomAccess, bool Bidirectional, bool Forward,
          bool Input>
struct iterator_tag_impl<T, true, RandomAccess, Bidirectional, Forward, Input> {
    typedef typename T::iterator_category tag;
};

template <typename T>
struct iterator_tag_impl<T, false, true, true, true, true> {
    typedef std::random_access_iterator_tag tag;
};

template <typename T>
struct iterator_tag_impl<T, false, false, true, true, true> {
    typedef std::bidirectional_iterator_tag tag;
};

template <typename T>
struct iterator_tag_impl<T, false, false, false, true, true> {
    typedef std::forward_iterator_tag tag;
};

template <typename T>
struct iterator_tag_impl<T, false, false, false, false, true> {
    typedef std::input_iterator_tag tag;
};

// iterator_tag forwards the correct arguments to iterator_input_tag
template <typename T>
struct iterator_tag
    : iterator_tag_impl<
          T, std::remove_reference<decltype(is_iterator<T>(0))>::type::value,
          std::remove_reference<decltype(
              random_access_compatible<T>(0))>::type::value,
          std::remove_reference<decltype(
              bidirectional_compatible<T>(0))>::type::value,
          std::remove_reference<decltype(
              forward_compatible<T>(0))>::type::value,
          std::remove_reference<decltype(
              input_compatible<T>(0))>::type::value> {};

} // namespace detail

template <typename T>
class direct_iterator;

/** This is a convenience function template that constructs an
 * ez::direct_iterator for \a value the type deduced from the type of the
 * argument. */
template <typename T>
direct_iterator<T> make_direct_iterator(T const& value);

/** An iterator for a type T, that when dereferenced, will return a reference to
 * T, rather than what T dereferences to.
 *
 *  direct_iterator<T> can be seen as a kind of inverse to
 * boost::indirect_iterator, which performs a double dereference in operator*().
 * In fact, boost::indirect_iterator<ez::direct_iterator<T>> should be
 * functionally equivalent to just T, when T is itself a const_iterator.
 *
 *  Because direct_iterator<T> never dereferences the type T, it is possible to
 * use direct_iterator with the integer types to have an iterator to int, char,
 * double etc.
 *
 *  This iterator will never return a non-const reference/pointer to the
 * underlying object, so there is not a separate const_direct_iterator as it
 * would be identical.
 *
 *  direct_iterator<T>::difference_type will always be the same as
 * T::difference_type, if it exits, or to std::ptrdiff_t otherwise. Simiarly,
 * direct_iterator<T>::iterator_category will be the same as
 * T::iterator_category, if it exists, or to the most strict iterator category
 * that T supports otherwise.
 *
 *  \code
 *  auto it = ez::make_direct_iterator(0);
 *  std::advance(it, 4);
 *  std::cout << *it; // prints 3
 *
 *  std::copy(ez::make_direct_iterator(1),
 *            ez::make_direct_iterator(11),
 *            std::ostream_iterator<int>(std::cout, " ")); // prints 1 2 3 4 5 6
 * 7 8 9 10 \endcode */
template <typename T>
class direct_iterator {
  public:
    typedef T value_type;
    typedef typename detail::difference_type<T>::type difference_type;
    typedef T const* pointer;
    typedef T const& reference;
    typedef typename detail::iterator_tag<T>::tag iterator_category;

    direct_iterator() noexcept(std::is_nothrow_default_constructible<T>::value)
        : m_value{} {}

    explicit direct_iterator(T const& value) noexcept(
        std::is_nothrow_copy_constructible<T>::value)
        : m_value{value} {}

    explicit direct_iterator(T&& value) noexcept(
        std::is_nothrow_move_constructible<T>::value)
        : m_value{std::move(value)} {}

    reference operator*() const noexcept { return m_value; }

    pointer operator->() const noexcept { return &m_value; }

    direct_iterator<T>& operator++() {
        ++m_value;
        return *this;
    }

    direct_iterator<T> operator++(int) {
        auto const copy = *this;
        ++(*this);
        return copy;
    }

    direct_iterator<T>& operator--() {
        --m_value;
        return *this;
    }

    direct_iterator<T> operator--(int) {
        auto const copy = *this;
        --(*this);
        return copy;
    }

    direct_iterator<T>& operator+=(difference_type n) {
        m_value += n;
        return *this;
    }

    direct_iterator<T>& operator-=(difference_type n) {
        m_value -= n;
        return *this;
    }

    direct_iterator<T> operator-(difference_type n) const {
        auto copy = *this;
        copy -= n;
        return copy;
    }

    direct_iterator<T> operator[](difference_type n) const {
        return direct_iterator<T>{static_cast<T>(m_value + n)};
    }

  private:
    T m_value;
};

template <typename T>
direct_iterator<T> operator+(direct_iterator<T> it,
                             typename direct_iterator<T>::difference_type n) {
    it += n;
    return it;
}

template <typename T>
direct_iterator<T> operator+(typename direct_iterator<T>::difference_type n,
                             direct_iterator<T> it) {
    it += n;
    return it;
}

template <typename T>
direct_iterator<T> operator-(direct_iterator<T> it,
                             typename direct_iterator<T>::difference_type n) {
    it -= n;
    return it;
}

template <typename T>
typename direct_iterator<T>::difference_type
operator-(direct_iterator<T> const& lhs, direct_iterator<T> const& rhs) {
    return *lhs - *rhs;
}

template <typename T>
bool operator==(direct_iterator<T> const& lhs, direct_iterator<T> const& rhs) {
    return *lhs == *rhs;
}

template <typename T>
bool operator!=(direct_iterator<T> const& lhs, direct_iterator<T> const& rhs) {
    return *lhs != *rhs;
}

template <typename T>
bool operator<(direct_iterator<T> const& lhs, direct_iterator<T> const& rhs) {
    return *lhs < *rhs;
}

template <typename T>
bool operator<=(direct_iterator<T> const& lhs, direct_iterator<T> const& rhs) {
    return *lhs <= *rhs;
}

template <typename T>
bool operator>(direct_iterator<T> const& lhs, direct_iterator<T> const& rhs) {
    return *lhs > *rhs;
}

template <typename T>
bool operator>=(direct_iterator<T> const& lhs, direct_iterator<T> const& rhs) {
    return *lhs >= *rhs;
}

template <typename T>
direct_iterator<T> make_direct_iterator(T const& value) {
    return direct_iterator<T>{value};
}

} // namespace ez

#endif
