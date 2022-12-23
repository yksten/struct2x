#include "parser.h"
#include "genericreader.h"


namespace json2cpp {

    class StructDetails {
        const std::string _name;
        const custom::GenericValue& _value;
    public:
        enum ITEMTYPE { TYPE_NULL = 0, TYPE_STRING = 1, TYPE_VECTOR = 2, TYPE_MAP = 4 };
        StructDetails(const std::string& name, const custom::GenericValue& value) : _name(name), _value(value) {

        }
        std::string toText() const {
            std::string strResult("struct struct_");
            strResult.append(_name).append(" {\n");
            // constructor
            strResult.append("    struct_").append(_name).append("() ");
            std::string strConstructor;
            for (const custom::GenericValue* item = _value.child; item; item = item->next) {
                if (item->type == custom::GenericValue::VALUE_BOOL) {
                    if (!strConstructor.empty()) {
                        strConstructor.append(", ");
                    }
                    strConstructor.append(item->key, item->keySize).append("(false)");
                } else if (item->type == custom::GenericValue::VALUE_NUMBER) {
                    if (!strConstructor.empty()) {
                        strConstructor.append(", ");
                    }
                    strConstructor.append(item->key, item->keySize).append("(0)");
                }
            }
            if (!strConstructor.empty()) {
                strResult.append(": ").append(strConstructor);
            }
            strResult.append(" {}\n\n");
            // field
            for (const custom::GenericValue* item = _value.child; item; item = item->next) {
                strResult.append("    ").append(StructDetails::toTypeByItem(*item, true));
                std::string fieldName;
                if (!item->key) {
                    const custom::GenericValue* itemPrev = item->prev;
                    fieldName.append(itemPrev->key, itemPrev->keySize).append("child");
                } else {
                    fieldName.append(item->key, item->keySize);
                }
                strResult.append(1, ' ').append(fieldName).append(1, ';');
                strResult.append(1, '\n');
            }
            // serialize
            strResult.append("\n    template<typename Archive>");
            strResult.append("\n    void serialize(Archive& ar) {\n        ar");
            for (const custom::GenericValue* item = _value.child; item; item = item->next) {
                std::string fieldName;
                if (!item->key) {
                    const custom::GenericValue* itemPrev = item->prev;
                    fieldName.append(itemPrev->key, itemPrev->keySize).append("child");
                } else {
                    fieldName.append(item->key, item->keySize);
                }
                strResult.append(".convert(").append(fieldName).append(1, ')');
            }
            strResult.append(";\n    }\n");

            strResult.append("};");

            return strResult;
        }
        int32_t hasType() const {
            int32_t type = TYPE_NULL;
            for (const custom::GenericValue* item = _value.child; item; item = item->next) {
                if (item->type == custom::GenericValue::VALUE_STRING) {
                    type |= TYPE_STRING;
                } else if (item->type == custom::GenericValue::VALUE_ARRAY) {
                    type |= TYPE_VECTOR;
                }
                // 无法区分object、map
            }
            return type;
        }
    private:
        static std::string toTypeByItem(const custom::GenericValue& item, const bool bChild) {
            std::string strResult;
            if (item.type == custom::GenericValue::VALUE_BOOL) {
                strResult.append("bool");
            } else if (item.type == custom::GenericValue::VALUE_NUMBER) {
                bool isFloat = false;
                for (uint32_t idx = 0; idx < item.valueSize; ++idx) {
                    if (item.value[idx] == '.') {
                        isFloat = true;
                        break;
                    }
                }
                if (isFloat) {
                    double db = custom::GenericReader::convertDouble(item.value, item.valueSize);
                    strResult.append("float");
                } else {
                    int64_t value = custom::GenericReader::convertInt(item.value, item.valueSize);
                    strResult.append("int32_t");
                }
            } else if (item.type == custom::GenericValue::VALUE_STRING) {
                strResult.append("std::string");
            } else if (item.type == custom::GenericValue::VALUE_ARRAY) {
                if (item.child) {
                    strResult.append("std::vector<").append(toTypeByItem(*item.child, false)).append(">");
                }
            } else if (item.type == custom::GenericValue::VALUE_OBJECT) {
                std::string itemName;
                if (!item.key) {
                    const custom::GenericValue* itemPrev = item.prev;
                    itemName.append(itemPrev->key, itemPrev->keySize);
                    if (bChild) {
                        itemName.append("child");
                    }
                } else {
                    itemName.append(item.key, item.keySize);
                }
                strResult.append("struct_").append(itemName);
            }
            return strResult;
        }
    };

    /*----------------------------------------------------------------------------------------------------------------*/

    class CppDescription {
        int32_t _hasStringVectorMap;
        std::vector<StructDetails> _vec;
    public:
        CppDescription() : _hasStringVectorMap(StructDetails::TYPE_NULL) {}
        void push(const StructDetails& item) {
            _hasStringVectorMap |= item.hasType();
            _vec.push_back(item);
        }
        std::string toText() const {
            std::string strResult("#include <stdint.h>");
            if ((_hasStringVectorMap & StructDetails::TYPE_STRING) == StructDetails::TYPE_STRING) {
                strResult.append("\n#include <string>");
            }
            if ((_hasStringVectorMap & StructDetails::TYPE_VECTOR) == StructDetails::TYPE_VECTOR) {
                strResult.append("\n#include <vector>");
            }
            if ((_hasStringVectorMap & StructDetails::TYPE_MAP) == StructDetails::TYPE_MAP) {
                strResult.append("\n#include <map>");
            }
            strResult.append("\n\n");
            for (int32_t idx = _vec.size() - 1; idx >= 0; idx--) {
                strResult.append(1, '\n');
                const StructDetails& item = _vec.at(idx);
                strResult.append(item.toText());
                strResult.append(1, '\n');
            }
            return strResult;
        }

        static CppDescription getDescription(const custom::GenericValue* root) {
            CppDescription descriptions;
            descriptions.push(StructDetails("result", *root));
            for (const custom::GenericValue* item = root->child; item; item = item->next) {
                if (item->type == custom::GenericValue::VALUE_OBJECT
                    || (item->type == custom::GenericValue::VALUE_ARRAY && item->child &&
                        item->child->type == custom::GenericValue::VALUE_OBJECT)) {
                    descriptions.push(StructDetails(std::string(item->key, item->keySize), *item));
                }
                for (const custom::GenericValue* itemChild = item->child; itemChild; itemChild = itemChild->child) {
                    if (itemChild->type == custom::GenericValue::VALUE_OBJECT
                        || (itemChild->type == custom::GenericValue::VALUE_ARRAY && itemChild->child &&
                            itemChild->child->type == custom::GenericValue::VALUE_OBJECT)) {
                        std::string itemName;
                        if (!itemChild->key) {
                            const custom::GenericValue* itemPrev = itemChild->prev;
                            itemName.append(itemPrev->key, itemPrev->keySize).append("child");
                        } else {
                            itemName.append(itemChild->key, itemChild->keySize);
                        }
                        descriptions.push(StructDetails(itemName, *itemChild));
                    }
                }
            }
            return descriptions;
        }
    };

    /*----------------------------------------------------------------------------------------------------------------*/

    std::string buildStruct(const char* json) {
        std::vector<custom::GenericValue> vec;
        custom::GenericReader reader(vec);
        const custom::GenericValue* root = reader.Parse(json);
        if (!root) {
            return std::string(reader.getError());
        }
        CppDescription descriptions = CppDescription::getDescription(root);

        return descriptions.toText();
    }

}
