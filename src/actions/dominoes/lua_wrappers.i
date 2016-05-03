%module kv_types

%{
#include "src/accumulators/real_extractors.h"
%}

%rename("$ignore", "not" %$isenum, "not" %$isenumitem, regextarget=1, fullname=1) "^extractors::packet::

%include "src/accumulators/real_extractors.h" 
