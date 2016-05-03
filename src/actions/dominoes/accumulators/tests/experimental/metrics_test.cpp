/*
 * =====================================================================================
 *
 *       Filename:  metrics.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  12/16/15 15:10:39
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include "metrics2.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <boost/preprocessor/stringize.hpp>
#include <boost/mpl/map.hpp>

int main() {

    //std::cout << BOOST_PP_STRINGIZE(METRICS_SEQ_FUSION_PAIR) << std::endl;
    std::cout << BOOST_PP_STRINGIZE((METRICS_SEQ_FUSION_PAIR)) << std::endl;
    std::cout << BOOST_PP_STRINGIZE((METRICS_SEQ_1)) << std::endl;
    std::cout << BOOST_PP_STRINGIZE((METRICS_SEQ_2)) << std::endl;
    std::cout << BOOST_PP_STRINGIZE((METRIC_TYPE_DECODER)) << std::endl;
    std::cout << BOOST_PP_STRINGIZE((TAG_METRIC_SEQ)) << std::endl;
}

