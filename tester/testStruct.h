#ifndef __TEST_TEST1__H_
#define __TEST_TEST1__H_

#include<stdint.h>
#include <string>
#include <vector>
#include <map>

namespace testStruct {

    struct struExample {
        struExample() : id(), has_id(false), has_str(false), f(), db() {}
        int32_t id;
        bool has_id;
        std::string str;
        bool has_str;
        float f;
        double db;
        std::vector<int32_t> v;

        template<typename T>
        void serialize(T& t) {
            t & SERIALIZE(1, id, &has_id) & SERIALIZE(2, str, &has_str) & SERIALIZE(3, f) & SERIALIZE(4, db) &SERIALIZE(5, v);
        }
    };

    struct struExamples {
        struExamples() {}
        std::vector<struExample> v;
        std::map<int32_t, struExample> m;

        template<typename T>
        void serialize(T& t) {
            t & SERIALIZE(1, v) & SERIALIZE(2, m);
        }
    };

}

#endif
