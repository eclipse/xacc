#pragma once
#include <map>
#include <set>
#include <string>
#include <vector>

#include <iso646.h>
#include <cstdlib>
#include <iostream>
#include <limits>

#include "debug.hpp"

// Macros local to this file, undefined at the end
#define nullval int(0xffffffff)
#define max_P (numeric_limits<P>::max())

namespace pairing_queue {
// Import std library components
using std::numeric_limits;

// static constexpr int nullval = 0xffffffff;

//! A priority queue based on a pairing heap, with fixed memory footprint and support for a decrease-key operation
template <typename P>
class pairing_queue {
  public:
    typedef P value_type;

  protected:
    std::vector<P> val;

    std::vector<int> next;
    std::vector<int> desc;
    std::vector<int> prev;

    int root;

  public:
    pairing_queue(int n) : val(n, 0), next(n, 0), desc(n, 0), prev(n, 0), root(nullval) {}

    //! Reset the queue and fill the values with a default
    inline void reset_fill(const P &v) {
        root = nullval;
	std::fill(std::begin(val), std::end(val), v);
	std::fill(std::begin(next), std::end(next), nullval);
	std::fill(std::begin(desc), std::end(desc), nullval);
        int n = 0;
        for (auto &p : prev) p = n++;
    }

    inline bool has(int index) const { return 0 <= index < val.size(); }

    //! Reset the queue and set the default to the maximum value
    inline void reset() { reset_fill(max_P); }

    //! Remove the minimum value
    //! return true if any change is made
    inline bool delete_min() {
        if (empty()) return false;

        int newroot = desc[root];
        if (!empty(newroot)) {
            newroot = merge_pairs(newroot);
            prev[newroot] = nullval;
            next[newroot] = nullval;
        }
        desc[root] = nullval;
        next[root] = nullval;
        prev[root] = root;
        root = newroot;
        return true;
    }

    //! Remove and return (in args) the minimum key, value pair
    inline bool pop_min(int &key, P &value) {
        if (empty()) {
            return false;
        }
        key = min_key();
        value = min_value();
        delete_min();
        return true;
    }

    //! Decrease the value of k to v
    //! NOTE: Assumes that v is lower than the current value of k
    inline void decrease_value(int k, const P &v) {
        minorminer_assert(0 <= k and k < val.size());
        minorminer_assert(v < val[k]);
        val[k] = v;
        decrease(k);
    }

    //! Decrease the value of k to v
    //! Does nothing if v isn't actually a decrease.
    inline bool check_decrease_value(int k, const P &v) {
        if (v < val[k]) {
            decrease_value(k, v);
            return true;
        } else {
            return false;
        }
    }

    inline void set_value(int k, const P &v) {
        minorminer_assert(0 <= k and k < val.size());
        minorminer_assert(0 <= k and k < prev.size());

        if (prev[k] == k) {
            val[k] = v;
            root = merge_roots(k, root);
        } else if (v < val[k]) {
            val[k] = v;
            decrease(k);
        } else if (val[k] < v) {
            val[k] = v;
            remove(k);
            root = merge_roots(k, root);
        }
    }

    inline void set_value_unsafe(int k, const P &v) { val[k] = v; }

    inline P min_value() const { return val[root]; }

    inline int min_key() const { return root; }

    inline bool empty(void) const { return empty(root); }

    inline bool empty(int k) const { return k == nullval; }

    inline P value(int k) const { return val[k]; }

  protected:
    inline int merge_roots(int a, int b) {
        // even this version of merge_roots is slightly unsafe -- we never call it with a null, so let's not check!
        // * doesn't check for nullval
        minorminer_assert(!empty(a));

        if (empty(b)) return a;
        int c = merge_roots_unsafe(a, b);
        prev[c] = nullval;
        return c;
    }

    inline int merge_roots_unsafe(int a, int b) {
        // this unsafe version of merge_roots which
        // * doesn't check for nullval
        // * doesn't ensure that the returned node has prev[a] = nullval
        minorminer_assert(!empty(a));
        minorminer_assert(!empty(b));

        if (val[a] < val[b]) return merge_roots_unchecked(a, b);
        if (val[b] < val[a]) return merge_roots_unchecked(b, a);
        if (a < b) return merge_roots_unchecked(a, b);
        return merge_roots_unchecked(b, a);
    }

    inline int merge_roots_unchecked(int a, int b) {
        // this very unsafe version of self.merge_roots which
        // * doesn't check for nullval
        // * doesn't ensure that the returned node has prev[a] = nullval
        // * doesn't check that a < b
        minorminer_assert(!empty(a));
        minorminer_assert(!empty(b));
        // minorminer_assert(a < b);

        next[b] = desc[a];
        if (!empty(desc[a])) prev[desc[a]] = b;
        prev[b] = a;
        desc[a] = b;
        return a;
    }

    inline int merge_pairs(int a) {
        if (empty(a)) return a;
        int r = nullval;
        do {
            int b = next[a];
            if (!empty(b)) {
                int c = next[b];
                b = merge_roots_unsafe(a, b);
                prev[b] = r;
                r = b;
                a = c;
            } else {
                prev[a] = r;
                r = a;
                break;
            }
        } while (!empty(a));
        a = r;
        r = prev[a];
        while (!empty(r)) {
            int t = prev[r];
            a = merge_roots_unsafe(a, r);
            r = t;
        }
        return a;
    }

    inline void remove(int a) {
        if (desc[prev[a]] == a)
            desc[prev[a]] = next[a];
        else
            next[prev[a]] = next[a];
        if (!empty(next[a])) {
            prev[next[a]] = prev[a];
            next[a] = nullval;
        }
    }

    inline void decrease(int a) {
        if (!empty(prev[a])) {
            remove(a);
            root = merge_roots(a, root);
        }
    }
};

//! This is a specialization of the pairing_queue that has a constant time
//! reset method, at the expense of an extra check when values are set or updated.
template <typename P>
class pairing_queue_fast_reset : public pairing_queue<P> {
    using super = pairing_queue<P>;

  private:
    std::vector<int> time;
    int now;

    inline bool current(int k) {
        minorminer_assert(0 <= k and k < time.size());
        if (time[k] != now) {
            time[k] = now;
            minorminer_assert(0 <= k and k < super::prev.size());
            minorminer_assert(0 <= k and k < super::next.size());
            minorminer_assert(0 <= k and k < super::desc.size());
            super::prev[k] = k;
            super::next[k] = nullval;  // super::nullval;
            super::desc[k] = nullval;  // super::nullval;
            return false;
        }
        return true;
    }

  public:
    pairing_queue_fast_reset(int n) : super(n), time(n), now(0) {}

    inline void reset() {
        super::root = nullval;  // super::nullval;
        if (!now++) std::fill(std::begin(time), std::end(time), 0);
    }

    inline void set_value_unsafe(int k, const P &v) {
        current(k);
        super::set_value_unsafe(k, v);
    }

    inline void set_value(int k, const P &v) {
        if (current(k))
            super::set_value(k, v);
        else {
            super::val[k] = v;
            super::root = super::merge_roots(k, super::root);
        }
    }

    inline bool check_decrease_value(int k, const P &v) {
        if (current(k))
            return super::check_decrease_value(k, v);
        else {
            super::set_value(k, v);
            return true;
        }
    }

    inline P get_value(int k) const {
        minorminer_assert(0 <= k and k < time.size());
        minorminer_assert(0 <= k and k < super::val.size());
        if (time[k] == now)
            return super::val[k];
        else
            return max_P;
    }
};
#undef nullval
#undef max_P
}
