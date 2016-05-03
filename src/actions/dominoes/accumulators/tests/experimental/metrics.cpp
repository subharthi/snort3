#include <stdio.h>
#include <iostream>

#include <boost/mpl/vector.hpp>
#include <boost/mpl/push_back.hpp>
#include <boost/preprocessor.hpp>

#include <map>

//#define METRICS \
(count)         \
(kurtosis)      \
(max)           \
(mean)          \
(median)        \
(min)           \
(skewness)      \
(sum)           \
(lazy_variance)

#define METRICS \
((count))         \
((kurtosis))      \
((max))           \
((mean))          \
((median))        \
((min))           \
((skewness))      \
((sum))           \
((lazy_variance))  \
((moment) (1))    \
((moment) (2))    \
((moment) (3))    \
((moment) (4))

//#define METRICS \
((count) (test1))         \
((kurtosis) (test2))      \
((max) (test3))           \
((mean) (test4))          \
((median) (test5))        \
((min) (test6))           \
((skewness) (test7))      \
((sum) (test8))           \
((lazy_variance) (test9))  \
((moment) (test)) 
//((moment) (1))    
//((moment) (2))    \
//((moment) (3))    \
//((moment) (4))


//#define OPER(s, data, elem) BOOST_PP_TUPLE_ELEM(0, elem)
//elem
//BOOST_PP_SEQ_HEAD(elem)

//#define METRIC_NAMES BOOST_PP_SEQ_TRANSFORM(OPER, 0, METRICS)
//
//#define METRIC_NAME(elem) BOOST_PP_SEQ_HEAD(BOOST_PP_SEQ_ELEM(0, elem))
//#define METRIC_PARAM(elem) BOOST_PP_SEQ_HEAD(BOOST_PP_SEQ_ELEM(1, elem))

#define METRIC_NAME(elem) BOOST_PP_SEQ_ELEM(0, elem)
#define METRIC_PARAM(elem) BOOST_PP_SEQ_ELEM(1, elem ())

#define MACRO(r, data, elem) \
( \
    BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(elem),1), \
        BOOST_PP_CAT(METRIC_NAME(elem), BOOST_PP_CAT(data, METRIC_PARAM(elem))), \
        BOOST_PP_SEQ_ELEM(0, elem)  \
    ) \
)

#define METRICS_SEQ BOOST_PP_SEQ_FOR_EACH(MACRO, _, METRICS)

enum metric_type {
    BOOST_PP_SEQ_ENUM(METRICS_SEQ)
};

    
//(BOOST_PP_CAT(METRIC_NAME(elem), data))

//BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(elem),1), \
    (BOOST_PP_CAT(METRIC_NAME(elem), BOOST_PP_CAT(data, BOOST_PP_SEQ_SIZE(elem)))), \
    BOOST_PP_CAT(METRIC_NAME(elem), data)  \
)

#define MACRO2(r, data, elem) \
BOOST_PP_STRINGIZE( \
BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(elem),1), \
    BOOST_PP_CAT(METRIC_NAME(elem), BOOST_PP_CAT(data, METRIC_PARAM(elem))), \
    BOOST_PP_SEQ_ELEM(0, elem)  \
) \
        ) ", " 

#define MACRO3(r, data, elem) \
BOOST_PP_STRINGIZE( \
BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(elem),1), \
    BOOST_PP_CAT(METRIC_NAME(elem), BOOST_PP_CAT(data, METRIC_PARAM(elem))), \
    BOOST_PP_SEQ_ELEM(0, elem)  \
) \
        )

#define MACRO4(r, data, elem) \
BOOST_PP_STRINGIZE( \
BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(elem),1), \
METRIC_NAME(elem)<METRIC_PARAM(elem)>, \
METRIC_NAME(elem)  \
) \
        ) ", "

//BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(elem),1), \
    BOOST_PP_CAT(METRIC_NAME(elem), <METRIC_PARAM(elem ())>), \
    BOOST_PP_SEQ_ELEM(0, elem)  \
) \
        )

//BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(elem),1), \
    (BOOST_PP_SEQ_ELEM(1, (elem))), \
    (BOOST_PP_SEQ_ELEM(0, (elem))  \
) \
) ", "

//BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(elem),1), \
    (BOOST_PP_CAT(METRIC_NAME(elem), BOOST_PP_CAT(data, METRIC_PARAM((elem))))), \
    (METRIC_NAME(elem))  \
)


    //" " BOOST_PP_STRINGIZE(METRIC_NAME(elem))
    //
//BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(elem),1), \
    (BOOST_PP_CAT(METRIC_NAME(elem), BOOST_PP_CAT(data, BOOST_PP_SEQ_SIZE(elem)))), \
    BOOST_PP_CAT(METRIC_NAME(elem), data)  \
)
    

#define METRICS_PRINT_SEQ BOOST_PP_SEQ_FOR_EACH(MACRO4, _, METRICS)

//BOOST_PP_IF(BOOST_PP_SEQ_SIZE(elem)-1, \
      BOOST_PP_CAT(BOOST_PP_SEQ_HEAD(elem), BOOST_PP_CAT(data, BOOST_PP_SEQ_TAIL(elem))), \
      BOOST_PP_SEQ_HEAD(elem) \
),

//BOOST_PP_CAT(elem, data)


//BOOST_PP_TUPLE_SIZE(elem)
//#define MACRO(r, data, elem) {elem, STRINGIZE(elem)}
//#define MACRO(r, data, elem) {elem, STRINGIZE(elem)}

//const std::map<metric_type, std::string> metric_type_decoder_ = {
//#define METRIC(NAME) {NAME, #NAME},
//#define METRIC_(NAME, PARAM) {NAME ## _ ## PARAM, #NAME "_" #PARAM},
//#include "metrics.hpp"
//#undef METRIC
//#undef METRIC_
//};
//}

//class a {};
//class b {};
//class c {};
//class d {};
//class e {};
//class f {};
//class g {};
//class h {};
//class i {};
//class j {};
//class k {};
//class l {};
//class m {};
//class n {};
//class o {};
//class p {};
//class q {};
//class r {};
//class s {};
//class t {};
//class u {};
//class v {};
//class w {};
//class x {};
//class y {};
//class z {};
//
////typedef boost::mpl::vector<a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z> alpha;
//typedef boost::mpl::vector<a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t> alpha;
//typedef boost::mpl::push_back<alpha, u>::type alpha2;
//typedef boost::mpl::push_back<alpha, v>::type alpha3;
//typedef boost::mpl::push_back<alpha, w>::type alpha4;
//typedef boost::mpl::push_back<alpha, x>::type alpha5;
//typedef boost::mpl::push_back<alpha, y>::type alpha6;
//typedef boost::mpl::push_back<alpha, z>::type alpha7;

#define DEREF(...) __VA_ARGS__

//#define MACRO4(r, data, elem) \
BOOST_PP_STRINGIZE( \
BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(elem),1), \
METRIC_NAME(elem)<METRIC_PARAM(elem)>, \
METRIC_NAME(elem)  \
) \
        ) ", "

//boost::fusion::pair<tag::NAME<PARAM>,std::string>,
#define FUSION_PAIR(r, data, elem) \
BOOST_PP_STRINGIZE( \
    BOOST_PP_IF(BOOST_PP_GREATER(BOOST_PP_SEQ_SIZE(elem),1), \
    (boost::fusion::pair<tag::METRIC_NAME(elem)<METRIC_PARAM(elem)>,std::string>), \
    (boost::fusion::pair<tag::METRIC_NAME(elem),std::string>) \
    ) \
) ", "

#define METRICS_SEQ_FUSION_PAIR BOOST_PP_SEQ_FOR_EACH(FUSION_PAIR, _, METRICS)
#define METRICS_SEQ_FUSION_PAIR2 BOOST_PP_SEQ_FOR_EACH(MACRO4, _, METRICS)

enum test {
    a,
    b
};

int main() {
    std::cout << count << std::endl;
    //std::cout << moment_test << std::endl;
    //std::cout << moment_test << std::endl;
    std::cout << moment_1 << std::endl;
    std::cout << moment_2 << std::endl;

    
    std::cout << METRICS_PRINT_SEQ << std::endl;
    std::cout << BOOST_PP_STRINGIZE(BOOST_PP_SEQ_ELEM(1,(a) (b))) << std::endl;
    std::cout << MACRO3(0, 0,BOOST_PP_SEQ_ELEM(8, METRICS)) << std::endl;
    std::cout << MACRO3(0, _,BOOST_PP_SEQ_ELEM(9, METRICS)) << std::endl;

    std::cout << METRICS_SEQ_FUSION_PAIR << std::endl;
}

