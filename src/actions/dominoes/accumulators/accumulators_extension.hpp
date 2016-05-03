#ifndef __ACCUMULATORS_EXTENSION_HPP__
#define __ACCUMULATORS_EXTENSION_HPP__

#include <boost/type_traits/is_base_and_derived.hpp>

struct serializable_base { };

template<typename T>
struct serializable
    : boost::is_base_and_derived<serializable_base, T> { };

struct is_serializable {
    template<typename Accumulator>
    struct apply
        : boost::is_base_and_derived<serializable_base, Accumulator> 
    {};
};

struct mergable_base { };

template<typename T>
struct mergable
    : boost::is_base_and_derived<mergable_base, T> { };

struct is_mergable {
    template<typename Accumulator>
    struct apply
        : boost::is_base_and_derived<mergable_base, Accumulator> 
    {};
};

#endif

