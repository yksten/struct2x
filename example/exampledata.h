#ifndef __EXAMPLE_DATA_H__
#define __EXAMPLE_DATA_H__

#include <stdint.h>
#include <string>
#include <vector>
#include <map>
#include <struct2x/struct2x.h>

namespace example {

    enum EnumType {
        ET1 = 0,
        ET2,
        ET3,
    };

    struct data {
        data() : e(ET2), b(true), i(-25), ui(253), i64(-847), ui64(3647457), f(253.28503), db(2535.78925), str("1111") {}

        EnumType e;
        bool b;
        int32_t i;
        uint32_t ui;
        int64_t i64;
        uint64_t ui64;
        float f;
        double db;
        std::string str;
        
        bool operator==(const data& that) const {
            if (e != that.e) {
                return false;
            }
            if (i != that.i) {
                return false;
            }
            if (ui != that.ui) {
                return false;
            }
            if (i64 != that.i64) {
                return false;
            }
            if (ui64 != that.ui64) {
                return false;
            }
            if (abs(f - that.f) > 0.000001) {
                return false;
            }
            if (abs(db - that.db) > 0.00000000001) {
                return false;
            }
            
            return true;
        }

        template<typename T>
        void serialize(T& t) {
            t & SERIALIZATION(1, e) & SERIALIZATION(2, b) & SERIALIZATION(3, i) & SERIALIZATION(4, ui);
            t & SERIALIZATION(5, i64) & SERIALIZATION(6, ui64) & SERIALIZATION(7, f) & SERIALIZATION(8, db) & SERIALIZATION(9, str);
        }
    };

    struct containerData {
        containerData() {}

        data d;
        std::vector<int32_t> vi;
        std::vector<data> v;
        std::map<int32_t, data> m;
        std::vector<std::vector<data> > vv;
        std::map<std::string, std::map<int32_t, data> > mm;
        std::vector<std::map<int32_t, data> > vm;
        std::map<std::string, std::vector<data> > mv;
        
        
        bool operator==(const containerData& that) const {
            if (!(d == that.d)) {
                return false;
            }
            if (vi != that.vi) {
                return false;
            }
            if (v != that.v) {
                return false;
            }
            if (m != that.m) {
                return false;
            }
            if (vv != that.vv) {
                return false;
            }
            if (mm != that.mm) {
                return false;
            }
            if (vm != that.vm) {
                return false;
            }
            if (mv != that.mv) {
                return false;
            }
            
            return true;
        }

        template<typename T>
        void serialize(T& t) {
            t & SERIALIZATION(1, d) & SERIALIZATION(2, vi) & SERIALIZATION(3, v) & SERIALIZATION(4, m);
            t & SERIALIZATION(5, vv) & SERIALIZATION(6, mm) & SERIALIZATION(7, vm) & SERIALIZATION(8, mv);
        }
    };

}

#endif
