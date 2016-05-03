#include <stdlib.h>
#include <iostream>
#include <climits>
#include <time.h>

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>

#include <boost/variant.hpp>

typedef int value_type;
using namespace boost::accumulators;

#define METRIC(NAME) accumulator_set< value_type, stats<tag::NAME> >,
#define METRIC_(NAME, PARAM) accumulator_set< value_type, stats<tag::NAME<PARAM> > >,
typedef boost::mpl::vector<
#include "metrics.hpp"
accumulator_set<value_type, stats<> > // maybe there is a better way to deal with the trailling comma
>::type acc_sets;
#undef METRIC
#undef METRIC_

typedef boost::make_variant_over<acc_sets>::type any_set;

#define METRIC(NAME) tag::NAME,
#define METRIC_(NAME, PARAM) tag::NAME<PARAM>,
typedef boost::mpl::vector<
#include "metrics.hpp"
tag::min // maybe there is a better way to deal with the trailling comma
>::type feature_mpl_vector;
#undef METRIC
#undef METRIC_

namespace boost { namespace accumulators {

// This is modified from how the accumulators library normally does things
// so that we can get the return type of an accumulator without having
// a accumulator_set that contains it
// It is most likely possible to improve this
template <class Sample>
struct accumulators {
    typedef typename detail::make_accumulator_tuple<
        feature_mpl_vector, Sample, void
    >::type accumulators_mpl_vector;

    typedef typename detail::meta::make_acc_list<accumulators_mpl_vector>::type accumulators_type;

    template<typename Feature>
    struct apply
    : fusion::result_of::value_of<
        typename fusion::result_of::find_if<
            accumulators_type
          , detail::matches_feature<Feature>
        >::type
      > { };
};

template<typename Feature>
struct accumulator_result
{
    typedef typename as_feature<Feature>::type feature_type;
    typedef typename mpl::apply<accumulators<int>, feature_type>::type::result_type type;
};

#define BOOST_ACCUMULATORS_EXTRACT_RESULT_FUN2(z, n, _)                     \
    template<                                                               \
        typename Feature                                                    \
      , typename AccumulatorSet                                             \
        BOOST_PP_ENUM_TRAILING_PARAMS_Z(z, n, typename A)                   \
    >                                                                       \
    typename mpl::apply<accumulators<int>, Feature>::type::result_type      \
    extract_result_(                                                        \
        AccumulatorSet const &acc                                           \
        BOOST_PP_ENUM_TRAILING_BINARY_PARAMS_Z(z, n, A, const &a)           \
    )                                                                       \
    {                                                                       \
        return find_accumulator<Feature>(acc).result(                       \
            detail::accumulator_params()(                                   \
                acc                                                         \
                BOOST_PP_ENUM_TRAILING_PARAMS_Z(z, n, a)                    \
            )                                                               \
        );                                                                  \
    }

BOOST_PP_REPEAT(
    BOOST_PP_INC(BOOST_ACCUMULATORS_MAX_ARGS)
  , BOOST_ACCUMULATORS_EXTRACT_RESULT_FUN2
  , _
)

} }


// This is used to visit each accumulator and insert the value into them
template <class Feature>
struct accumulator_set_extractor_visitor : public boost::static_visitor< typename accumulator_result<Feature>::type > {
    accumulator_set_extractor_visitor() { }

    template<typename set_type>
    typename accumulator_result<Feature>::type
    operator()(set_type &set, typename boost::enable_if<
                                    typename boost::mpl::contains<typename set_type::features_type, Feature>
                                            >::type *dummy = NULL ) const {
        return extract_result_<Feature>(set);
    }

    template<typename set_type>
    typename accumulator_result<Feature>::type 
    operator()(set_type &set, typename boost::disable_if<
                                    typename boost::mpl::contains<typename set_type::features_type, Feature>
                                            >::type *dummy = NULL ) const {
        throw "Not the same";
    }
};

template <class Feature, class Accumulator_Set>
typename accumulator_result<Feature>::type
extract_results(Accumulator_Set &s) {
    return boost::apply_visitor(accumulator_set_extractor_visitor<Feature>(), s);
}


// If the accumulator set does not contain the feature then this is used
//template <class Feature, class Accumulator_Set, class Enable = void>
//struct accumulator_extractor {
//    accumulator_extractor(Accumulator_Set &s) : set(s), is_valid(0) {}
//    int is_valid;
//    typename accumulator_result<Feature>::type operator()() {
//        // TODO make this throw a much more descriptive exception
//        throw "Could not extract";
//    }
//    Accumulator_Set &set;
//};
//
//// If the accumulator set contains the feature then this is used
//template <class Feature, class Accumulator_Set>
//struct accumulator_extractor <Feature, Accumulator_Set, class boost::enable_if<
//                                    typename boost::mpl::contains<typename Accumulator_Set::features_type, Feature>
//                                            >::type >
//{
//    accumulator_extractor(Accumulator_Set &s) : set(s), is_valid(1) {}
//    int is_valid;
//    typename accumulator_result<Feature>::type operator()() {
//        return extract_result_<Feature>(set);
//    }
//    Accumulator_Set &set;
//};
//
//template <class Feature, class Accumulator_Set>
//accumulator_extractor<Feature, Accumulator_Set> make_accumulator_extractor(Accumulator_Set &s) {
//    return accumulator_extractor<Feature, Accumulator_Set>(s);
//}

int main(int argc, char *argv[]) {

    using namespace boost::accumulators::detail;

	typedef accumulator_set<int, stats<> > count_set;
	typedef accumulator_set<int, stats<tag::mean> > mean_set;

    mean_set set1;
    count_set set2;

    set1(1); set1(2); set1(3);

    set2(1); set2(2); set2(3);

    any_set mean_set_variant = any_set(mean_set());

    //boost::apply_visitor(accumulator_set_extractor_visitor<tag::mean>(), mean_set_variant);
    //boost::apply_visitor(accumulator_set_extractor_visitor<tag::count>(), mean_set_variant);

    try {
        auto e = extract_results<tag::mean>(mean_set_variant);
        std::cout << e << std::endl;
    } catch( const char *e ) {
        std::cout << e << std::endl;
    }
    try {
        auto e2 = extract_results<tag::count>(mean_set_variant);
        std::cout << e2 << std::endl;
    } catch( const char *e ) {
        std::cout << e << std::endl;
    }

    //try {
    //    auto e1 = make_accumulator_extractor<tag::mean>(set1);
    //    std::cout << e1() << std::endl;
    //    auto e2 = make_accumulator_extractor<tag::mean>(set2);
    //    std::cout << e2() << std::endl;
    //} catch( const char *e) {
    //    std::cout << e << std::endl;
    //}


    //std::cout << extract_result_<tag::mean>(set1) << std::endl;
  
//#define METRIC(NAME) \
//    try{ std::cout << typeid(accumulator_result<tag::NAME>).name() << std::endl; } catch (exception& e) { std::cout << e << std::endl; }
//#define METRIC_(NAME, PARAM) \
//    try{ std::cout << typeid(accumulator_result<tag::NAME< PARAM > >).name() << std::endl; } catch (exception& e) { std::cout << e << std::endl; }
//#include "metrics.hpp"
//#undef METRIC
//#undef METRIC_

}
