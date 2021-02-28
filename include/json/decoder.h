#ifndef __JSON_DECODER_H__
#define __JSON_DECODER_H__

#include "serialize.h"
#include <string>
#include <map>

#include "thirdParty/json/GenericReader.h"


namespace serialize {

    class EXPORTAPI JSONDecoder {
        custom::jsonConverterMgr* _mgr;
        custom::StringStream _str;

        template<typename T>
        static custom::jsonConverterMgr getSet(T& value) {
            custom::jsonConverterMgr mgr(&value);
            JSONDecoder decoder(NULL, 0);
            decoder._mgr = &mgr;
            internal::serializeWrapper(decoder, value);
            return mgr;
        }
    public:
        JSONDecoder(const char* sz, uint32_t length);

        template<typename T>
        bool operator>>(T& value) {
            _mgr = obtain(value, NULL, 0, NULL);
            custom::Handler handler(_mgr, &value);
            return custom::CustomGenericReader().Parse(_str, handler);
        }

        template<typename T>
        JSONDecoder& operator&(serializeItem<T> value) {
            return convert(value.name, *(typename internal::TypeTraits<T>::Type*)(&value.value), value.bHas);
        }

        template<typename T>
        JSONDecoder& convert(const char* sz, T& value, bool* pHas = NULL) {
            decodeValue(sz, *(typename internal::TypeTraits<T>::Type*)(&value), pHas);
            return *this;
        }
    private:
        template<typename T>
        static custom::jsonConverterMgr* obtain(T& value, const char*, uint32_t length, void* owner) {
            static custom::jsonConverterMgr mgr = getSet(value);
            mgr.setStruct(&value, owner);
            return &mgr;
        }

        template<typename T>
        static custom::jsonConverterMgr* obtainArray(std::vector<T>& value, const char*, uint32_t length, void* owner) {
            value.push_back(T());
            custom::jsonConverterMgr* mgr = obtain(value.back(), NULL, length, owner);
            return mgr;
        }

        template<typename K, typename V>
        static custom::jsonConverterMgr* obtain(std::map<K, V>& value, const char* sz, uint32_t length, void* owner) {
            static custom::jsonConverterMgr mgr(NULL, true);
            if (sz && length) {
                std::string strKey(sz, length);
                mgr.clear();
                K key = internal::STOT::type<typename internal::TypeTraits<K>::Type>::strto(strKey.c_str());
                mgr.setStruct(&value[key], owner);
                mgr.insert(std::pair<std::string, custom::jsonConverter>(strKey, custom::jsonConverterMgr::bindMap<K, V>(&JSONDecoder::convertValue, 0, 0)));
            }
            return &mgr;
        }

        template<typename T>
        void decodeValue(const char* sz, T& value, bool* pHas = NULL) {
            size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
            size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
            _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bind<T>(&JSONDecoder::convertValue, offset, has, &JSONDecoder::obtain)));
        }

        template<typename T>
        static void clearArray(std::vector<T>& value) {
            if (!value.empty()) {
                value.clear();
            }
        }

        template<typename T>
        void decodeValue(const char* sz, std::vector<T>& value, bool* pHas = NULL) {
            size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
            size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
            _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindArray<T>(&JSONDecoder::convertArray, offset, has, &JSONDecoder::obtainArray, &JSONDecoder::clearArray)));
        }

        template<typename K, typename V>
        void decodeValue(const char* sz, std::map<K, V>& value, bool* pHas = NULL) {
            size_t offset = ((uint8_t*)(&value)) - _mgr->getStruct();
            size_t has = pHas ? ((uint8_t*)(pHas)) - _mgr->getStruct() : 0;
            _mgr->insert(std::pair<std::string, custom::jsonConverter>(sz, custom::jsonConverterMgr::bindMap<K, V>(&JSONDecoder::convertValue, offset, has, &JSONDecoder::obtain)));
        }

        void decodeValue(const char* sz, bool& value, bool* pHas = NULL);
        void decodeValue(const char* sz, uint32_t& value, bool* pHas = NULL);
        void decodeValue(const char* sz, int32_t& value, bool* pHas = NULL);
        void decodeValue(const char* sz, uint64_t& value, bool* pHas = NULL);
        void decodeValue(const char* sz, int64_t& value, bool* pHas = NULL);
        void decodeValue(const char* sz, float& value, bool* pHas = NULL);
        void decodeValue(const char* sz, double& value, bool* pHas = NULL);
        void decodeValue(const char* sz, std::string& value, bool* pHas = NULL);

        void decodeValue(const char* sz, std::vector<bool>& value, bool* pHas = NULL);
        void decodeValue(const char* sz, std::vector<uint32_t>& value, bool* pHas = NULL);
        void decodeValue(const char* sz, std::vector<int32_t>& value, bool* pHas = NULL);
        void decodeValue(const char* sz, std::vector<uint64_t>& value, bool* pHas = NULL);
        void decodeValue(const char* sz, std::vector<int64_t>& value, bool* pHas = NULL);
        void decodeValue(const char* sz, std::vector<float>& value, bool* pHas = NULL);
        void decodeValue(const char* sz, std::vector<double>& value, bool* pHas = NULL);
        void decodeValue(const char* sz, std::vector<std::string>& value, bool* pHas = NULL);

        static void convertValue(bool& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertValue(int32_t& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertValue(uint32_t& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertValue(int64_t& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertValue(uint64_t& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertValue(float& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertValue(double& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertValue(std::string& value, const char* cValue, uint32_t length, bool* pHas);
        template<typename P>
        static void convertValue(P& value, const char* cValue, uint32_t length, bool* pHas) {
            //value = cValue;
            //custom::Handler::convert(value, cValue, length);
            if (pHas) *pHas = true;
        }


        static void convertArray(std::vector<bool>& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertArray(std::vector<int32_t>& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertArray(std::vector<uint32_t>& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertArray(std::vector<int64_t>& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertArray(std::vector<uint64_t>& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertArray(std::vector<float>& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertArray(std::vector<double>& value, const char* cValue, uint32_t length, bool* pHas);
        static void convertArray(std::vector<std::string>& value, const char* cValue, uint32_t length, bool* pHas);
        template<typename P>
        static void convertArray(std::vector<P>& value, const char* cValue, uint32_t length, bool* pHas) {
            value.push_back(P());
            if (pHas) *pHas = true;
        }

        //template<typename K, typename V>
        //static void convertMap(std::map<K, V>& value, const V& cValue, bool* pHas) {
        //    //value.push_back(cValue);
        //    if (pHas) *pHas = true;
        //}
    };

}


#endif