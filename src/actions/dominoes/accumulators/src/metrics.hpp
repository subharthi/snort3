#include <boost/preprocessor.hpp>

#define METRICS_SEQ \
((count))         \
((max))           \
((min))           \
((sum))           \
/*
((mean))          \
((skewness))      \
((median))        \
((kurtosis))      \
((lazy_variance)) \
//((rolling_sum))   \
//((moment) (1))    \
//((moment) (2))    \
//((moment) (3))    \
//((moment) (4))
*/
//#define METRICS_LIST \
//(count)
//(covariance)
//(density)
//((error_of) (mean))
//(extended_p_square)
//(extended_p_square_quantile)
//(kurtosis)
//(max)
//(mean)
//(median)
//(min)
//((moment) (1))
//((moment) (2))
//((moment) (3))
//((moment) (4))
//(p_square_cumulative_distribution)
//(p_square_quantile)
//((peaks_over_threshold) (left))
//((peaks_over_threshold) (right))
//(pot_quantile)
//(pot_tail_mean)
//(rolling_count)
//(rolling_sum)
//(rolling_mean)
//((rolling_moment) (1))
//((rolling_moment) (2))
//((rolling_moment) (3))
//((rolling_moment) (4))
//(rolling_variance)
//(skewness)
//(sum)
//((tail) (left))
//((tail) (right))
//(coherent_tail_mean)
//(non_coherent_tail_mean)
//(tail_quantile)
//(tail_variate)
//(tail_variate_means)
//(lazy_variance)
//(variance)
//(weighted_covariance)
//(weighted_density)
//(weighted_extended_p_square)
//(weighted_kurtosis)
//(weighted_mean)
//(weighted_median)
//(weighted_moment)
//(weighted_p_square_cumulative_distribution)
//(weighted_p_square_quantile)
//(weighted_peaks_over_threshold)
//(weighted_skewness)
//(weighted_sum)
//(non_coherent_weighted_tail_mean)
//(weighted_tail_quantile)
//(weighted_tail_variate_means)
//(weighted_variance)

//(sample)
//(weighted_sample)
//(entropy)
//(topk)
//(unique)
//(std)

#define METRIC_NAME(elem) BOOST_PP_SEQ_ELEM(0, elem)
#define METRIC_PARAM(elem) BOOST_PP_SEQ_ELEM(1, elem ())

#define RUNTIME_NAME(elem) \
    BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(elem),1), \
        BOOST_PP_CAT(METRIC_NAME(elem), BOOST_PP_CAT(_, METRIC_PARAM(elem))), \
        BOOST_PP_SEQ_ELEM(0, elem)  \
    )

#define COMPILETIME_NAME(elem) \
    BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(elem),1), \
    tag::METRIC_NAME(elem)<METRIC_PARAM(elem)>, \
    tag::METRIC_NAME(elem)  \
    )

// TODO rename this
// NAME_PARAM
#define MACRO1(r, data, elem) \
( \
  RUNTIME_NAME(elem) \
)

#define METRICS_SEQ_1 BOOST_PP_SEQ_FOR_EACH(MACRO1, _, METRICS_SEQ)

// TODO rename this
// NAME<PARAM>
#define MACRO2(r, data, elem) \
( \
  COMPILETIME_NAME(elem) \
)

#define METRICS_SEQ_2 BOOST_PP_SEQ_FOR_EACH(MACRO2, _, METRICS_SEQ)


// {NAME_PARAM, "NAME_PARAM"}
#define DECODER_MACRO(r, data, i, elem) \
    BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(0,i)) \
    {elem BOOST_PP_COMMA() BOOST_PP_STRINGIZE(elem)} \

#define METRIC_TYPE_DECODER BOOST_PP_SEQ_FOR_EACH_I(DECODER_MACRO, _, METRICS_SEQ_1)

//,std::pair<tag::NAME<PARAM>,std::string>
#define TAG_METRIC_MACRO(r, data, i, elem) \
    BOOST_PP_COMMA_IF(BOOST_PP_NOT_EQUAL(0,i)) \
    boost::mpl::pair<COMPILETIME_NAME(elem) BOOST_PP_COMMA() boost::mpl::int_<metric::metric_type::RUNTIME_NAME(elem)> > \

#define TAG_METRIC_SEQ BOOST_PP_SEQ_FOR_EACH_I(TAG_METRIC_MACRO, _, METRICS_SEQ)

//METRIC(count)
////METRIC(covariance)
////METRIC(density) //PARAM(cache_size) PARAM(num_bins) METRIC_END()
////METRIC_(error_of, mean) //PARAM(cache_size) PARAM(num_bins) METRIC_END()
////METRIC(extended_p_square) //PARAM(probabilities) METRIC_END()
////METRIC(extended_p_square_quantile)
//  //METRIC(kurtosis)
//METRIC(max)
//  //METRIC(mean)
//  //METRIC(median)
//METRIC(min)
////METRIC_(moment, 1)
////METRIC_(moment, 2)
////METRIC_(moment, 3)
////METRIC_(moment, 4)
////METRIC(p_square_cumulative_distribution)
////METRIC(p_square_quantile)
////METRIC_(peaks_over_threshold, left)
////METRIC_(peaks_over_threshold, right)
////METRIC(pot_quantile)
////METRIC(pot_tail_mean)
////METRIC(rolling_count) PARAM(window_size)
////METRIC(rolling_sum) //PARAM(window_size)
////METRIC(rolling_mean) PARAM(window_size)
////METRIC_(rolling_moment, 1) PARAM(window_size)
////METRIC_(rolling_moment, 2) PARAM(window_size)
////METRIC_(rolling_moment, 3) PARAM(window_size)
////METRIC_(rolling_moment, 4) PARAM(window_size)
////METRIC(rolling_variance) PARAM(window_size)
//    //METRIC(skewness)
//METRIC(sum)
////METRIC_(tail, left)
////METRIC_(tail, right)
////METRIC(coherent_tail_mean)
////METRIC(non_coherent_tail_mean)
////METRIC(tail_quantile)
////METRIC(tail_variate)
////METRIC(tail_variate_means)
//  //METRIC(lazy_variance)
////METRIC(variance)
////METRIC(weighted_covariance)
////METRIC(weighted_density)
////METRIC(weighted_extended_p_square)
////METRIC(weighted_kurtosis)
////METRIC(weighted_mean)
////METRIC(weighted_median)
////METRIC(weighted_moment)
////METRIC(weighted_p_square_cumulative_distribution)
////METRIC(weighted_p_square_quantile)
////METRIC(weighted_peaks_over_threshold)
////METRIC(weighted_skewness)
////METRIC(weighted_sum)
////METRIC(non_coherent_weighted_tail_mean)
////METRIC(weighted_tail_quantile)
////METRIC(weighted_tail_variate_means)
////METRIC(weighted_variance)
////METRIC(sample)
////METRIC(weighted_sample)
////METRIC(entropy)
////METRIC(topk)
////METRIC(unique)
////METRIC(std)
