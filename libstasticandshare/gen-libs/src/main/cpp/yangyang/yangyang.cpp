//
// Created by jayden on 2020/9/3.
//

#include "yangyang.h"

std::string getYangYangResponse() {
    LOGE("Yang get response");
    return "Hello, I am YangYang";
}

std::int32_t getYangYangAge() {
    LOGE("Yang get age");
    return 2;
}

__attribute__ ((visibility ("default"))) YangYang_FUN yangYangFun = {
        getYangYangResponse,
        getYangYangAge
};
