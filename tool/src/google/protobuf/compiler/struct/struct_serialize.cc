#include "struct_serialize.h"

#include <google/protobuf/compiler/cpp/cpp_message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/wire_format.h>

namespace google {
    namespace protobuf {
        namespace compiler {
            namespace cpp {

                const char* const kKeywordList[] = {
                    "alignas", "alignof", "and", "and_eq", "asm", "auto", "bitand", "bitor",
                    "bool", "break", "case", "catch", "char", "class", "compl", "const",
                    "constexpr", "const_cast", "continue", "decltype", "default", "delete", "do",
                    "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern",
                    "false", "float", "for", "friend", "goto", "if", "inline", "int", "long",
                    "mutable", "namespace", "new", "noexcept", "not", "not_eq", "NULL",
                    "operator", "or", "or_eq", "private", "protected", "public", "register",
                    "reinterpret_cast", "return", "short", "signed", "sizeof", "static",
                    "static_assert", "static_cast", "struct", "switch", "template", "this",
                    "thread_local", "throw", "true", "try", "typedef", "typeid", "typename",
                    "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t",
                    "while", "xor", "xor_eq"
                };

                hash_set<string> MakeKeywordsMap() {
                    hash_set<string> result;
                    for (int i = 0; i < GOOGLE_ARRAYSIZE(kKeywordList); i++) {
                        result.insert(kKeywordList[i]);
                    }
                    return result;
                }

                hash_set<string> kKeywords = MakeKeywordsMap();

                const char* type2string(const FieldDescriptor& field, const char* sz = "") {
                    switch (field.type()) {
                    case FieldDescriptor::TYPE_DOUBLE:
                        return "double";
                    case FieldDescriptor::TYPE_FLOAT:
                        return "float";
                    case FieldDescriptor::TYPE_INT64:
                    case FieldDescriptor::TYPE_UINT64:
                    case FieldDescriptor::TYPE_FIXED64:
                    case FieldDescriptor::TYPE_SFIXED64:
                    case FieldDescriptor::TYPE_SINT64:
                        return "int64_t";
                    case FieldDescriptor::TYPE_INT32:
                    case FieldDescriptor::TYPE_FIXED32:
                    case FieldDescriptor::TYPE_UINT32:
                    case FieldDescriptor::TYPE_SFIXED32:
                    case FieldDescriptor::TYPE_SINT32:
                        return "int32_t";
                    case FieldDescriptor::TYPE_ENUM:
                    {
                        static std::string strRet;
                        strRet = sz + ClassName(field.enum_type(), true);
                        return strRet.c_str();
                    }
                    case FieldDescriptor::TYPE_BOOL:
                        return "bool";
                    case FieldDescriptor::TYPE_STRING:
                    case FieldDescriptor::TYPE_BYTES:
                        return "std::string";
                    default:{
                        static std::string strRet;
                        strRet = sz + ClassName(field.message_type(), true);
                        return strRet.c_str();
                    }
                    }
                }

                const char* map2string(const FieldDescriptor& field) {
                    static std::string strRet;
                    strRet.clear();
                    assert(field.message_type()->field_count() == 2);
                    const FieldDescriptor* key = field.message_type()->field(0);
                    const FieldDescriptor* value = field.message_type()->field(1);
                    strRet.append(type2string(*key)).append(", ").append(type2string(*value));
                    return strRet.c_str();
                }

                const char* type2value(const FieldDescriptor& field) {
                    static char sz[64] = { 0 };
                    memset(sz, 0, sizeof(sz));
                    switch (field.type()) {
                    case FieldDescriptor::TYPE_DOUBLE:
                        sprintf(sz, "%lf", field.default_value_double());
                        break;
                    case FieldDescriptor::TYPE_FLOAT:
                        sprintf(sz, "%f", field.default_value_float());
                        break;
                    case FieldDescriptor::TYPE_INT64:
                    case FieldDescriptor::TYPE_FIXED64:
                    case FieldDescriptor::TYPE_SFIXED64:
                    case FieldDescriptor::TYPE_SINT64:
                        sprintf(sz, "%lld", field.default_value_int64());
                        break;
                    case FieldDescriptor::TYPE_UINT64:
                        sprintf(sz, "%llu", field.default_value_uint64());
                        break;
                    case FieldDescriptor::TYPE_INT32:
                    case FieldDescriptor::TYPE_FIXED32:
                    case FieldDescriptor::TYPE_SFIXED32:
                    case FieldDescriptor::TYPE_SINT32:
                        sprintf(sz, "%d", field.default_value_int32());
                        break;
                    case FieldDescriptor::TYPE_UINT32:
                        sprintf(sz, "%u", field.default_value_uint32());
                        break;
                    case FieldDescriptor::TYPE_ENUM:
                        sprintf(sz, "%d", field.default_value_enum());
                        break;
                    case FieldDescriptor::TYPE_BOOL:
                        if (field.default_value_bool())
                            return "true";
                        return "false";
                    case FieldDescriptor::TYPE_STRING:
                    case FieldDescriptor::TYPE_BYTES:
                    default:
                        assert(false);
                    }
                    return sz;
                }

                const char* getFieldType(const FieldDescriptor& field) {
                    if (field.type() == FieldDescriptor::TYPE_FIXED64 || field.type() == FieldDescriptor::TYPE_SFIXED64) {
                        return "serialize::TYPE_FIXED64";
                    } else if (field.type() == FieldDescriptor::TYPE_SINT32 || field.type() == FieldDescriptor::TYPE_SINT64) {
                        return "serialize::TYPE_SVARINT";
                    } else if (field.type() == FieldDescriptor::TYPE_FIXED32 || field.type() == FieldDescriptor::TYPE_SFIXED32) {
                        return "serialize::TYPE_FIXED32";
                    }
                    return NULL;
                }

                const char* type2tag(const FieldDescriptor& field, FileDescriptor::Syntax syntax) {
                    static std::string strResult;
                    strResult.clear();
                    switch (field.type()) {
                    case FieldDescriptor::TYPE_FIXED64:
                    case FieldDescriptor::TYPE_SFIXED64:
                        strResult.append(", serialize::TYPE_FIXED64");
                        break;
                    case FieldDescriptor::TYPE_SINT32:
                    case FieldDescriptor::TYPE_SINT64:
                        strResult.append(", serialize::TYPE_SVARINT");
                        break;
                    case FieldDescriptor::TYPE_FIXED32:
                    case FieldDescriptor::TYPE_SFIXED32:
                        strResult.append(", serialize::TYPE_FIXED32");
                    default:
                        //strResult.append(", serialize::TYPE_VARINT");
                        break;
                    }

                    if (field.is_packed()) {
                        if (strResult.empty()) strResult.append(", serialize::TYPE_VARINT");
                        strResult.append("| serialize::TYPE_PACKED");
                    } else if (field.is_map()) {
                        const FieldDescriptor* key = field.message_type()->field(0);
                        if (const char* szKeyType = getFieldType(*key)) {
                            strResult.append(szKeyType).append(" << serialize::BITNUM");
                        }
                        const FieldDescriptor* value = field.message_type()->field(1);
                        if (const char* szValueType = getFieldType(*key)) {
                            if (!strResult.empty()) {
                                strResult.append(" | ");
                            }
                            strResult.append(szValueType);
                        }

                    }

                    if (syntax == FileDescriptor::SYNTAX_PROTO2) {
                        if (field.label() == FieldDescriptor::LABEL_OPTIONAL)
                            strResult.append(", &has_").append(codeSerialize::FieldName(field));
                    }

                    return strResult.c_str();
                }

                class packagePartsWrapper {
                    const std::vector<string>& _package_parts;
                    google::protobuf::io::Printer& _printer;
                public:
                    packagePartsWrapper(const std::vector<string>& package_parts, google::protobuf::io::Printer& printer)
                        :_package_parts(package_parts), _printer(printer) {
                        uint32_t nSize = _package_parts.size();
                        for (uint32_t idx = 0; idx < nSize; ++idx) {
                            _printer.Print("namespace $name$ {\n", "name", _package_parts.at(idx));
                        }
                    }
                    ~packagePartsWrapper() {
                        _printer.Print("\n");
                        for (std::vector<string>::const_reverse_iterator it = _package_parts.rbegin(); it != _package_parts.rend(); ++it) {
                            _printer.Print("} // namespace $name$\n", "name", *it);
                        }
                    }
                };

                codeSerialize::codeSerialize(const FileDescriptor* file, const Options& options) :scc_analyzer_(options), _file(file) {
                    prepareMsgs();
                }

                codeSerialize::~codeSerialize() {
                }

                void codeSerialize::prepareMsgs() {
                    std::vector<const Descriptor*> msgs = FlattenMessagesInFile(_file);
                    for (int i = 0; i < msgs.size(); ++i) {
                        const Descriptor* descriptor = msgs[i];
                        FieldDescriptorArr optimized_order;
                        optimized_order._name = ClassName(msgs[i]);
                        for (int idx = 0; idx < descriptor->field_count(); ++idx) {
                            const FieldDescriptor* field = descriptor->field(idx);
                            if (field->options().weak()) {
                                ;
                            } else if (!field->containing_oneof()) {
                                optimized_order._vec.push_back(field);
                            }
                        }
                        _message_generators.push_back(optimized_order);
                    }
                    //sort
                    sortMsgs(_message_generators);
                }

                bool codeSerialize::sortMsgs(std::vector<FieldDescriptorArr>& msgs) {
                    std::vector<FieldDescriptorArr> result;
                    uint32_t nSize = msgs.size();
                    for (uint32_t idx = 0; idx < nSize; ++idx) {
                        const FieldDescriptorArr& item = msgs.at(idx);
                        if (result.empty()) {
                            result.insert(result.begin(), item);
                        } else {
                            result.insert(result.begin() + getInsertIdx(result, item), item);
                        }
                    }
                    msgs.swap(result);
                    return true;
                }

                uint32_t codeSerialize::getInsertIdx(const std::vector<FieldDescriptorArr>& msgs, const FieldDescriptorArr& item) {
                    uint32_t idx = 0;
                    for (; idx < msgs.size(); ++idx) {
                        const FieldDescriptorArr& curItem = msgs.at(idx);
                        uint32_t curItemIdx = 0;
                        for (; curItemIdx < curItem._vec.size(); ++curItemIdx) {
                            const FieldDescriptor* field = curItem._vec.at(curItemIdx);
                            if (field->message_type() && field->message_type()->name() == item._name) {
                                return idx;
                            }
                        }
                    }
                    return idx;
                }

                void codeSerialize::printHeader(google::protobuf::io::Printer& printer, const char* szName)const {
                    std::string strStructName(szName);
                    std::transform(strStructName.begin(), strStructName.end(), strStructName.begin(), ::toupper);
                    //1.program once
                    printer.Print("#ifndef __STRUCT_$basename$_INCLUDE__H_\n#define __STRUCT_$basename$_INCLUDE__H_\n", "basename", strStructName);
                    //2.include
                    printInclude(printer);
                    do {
                        //3.namespace
                        std::vector<string> package_parts = Split(_file->package(), ".", true);
                        packagePartsWrapper ins(package_parts, printer);
                        printEnum(printer);
                        //struct
                        //constructed function
                        //Initialization fidlds
                        //fields
                        printStruct(printer, _file->syntax());
                    } while (false);
                    //4.serialize functions
                    //printSerialize(printer);
                    printer.Print("\n#endif\n");
                }

                void codeSerialize::printInclude(google::protobuf::io::Printer& printer)const {
                    printer.Print("\n");
                    printer.Print("#include \"serialize.h\"\n");
                    //if (hasInt(printer)) {
                    //    printer.Print("#include <stdint.h>\n");
                    //}
                    if (hasString(printer)) {
                        printer.Print("#include <string>\n");
                    }
                    //if (hasVector(printer)) {
                    //    printer.Print("#include <vector>\n");
                    //}
                    if (hasMap(printer)) {
                        printer.Print("#include <map>\n");
                    }
                    // import
                    std::set<string> public_import_names;
                    for (int i = 0; i < _file->public_dependency_count(); i++) {
                        public_import_names.insert(_file->public_dependency(i)->name());
                    }

                    for (int i = 0; i < _file->dependency_count(); i++) {
                        const bool use_system_include = IsWellKnownMessage(_file->dependency(i));
                        const string& name = _file->dependency(i)->name();
                        bool public_import = (public_import_names.count(name) != 0);
                        printer.Print(
                            "#include $left$$dependency$.pb.h$right$$iwyu$\n",
                            "dependency", StripProto(name),
                            "iwyu", (public_import) ? "  // IWYU pragma: export" : "",
                            "left", use_system_include ? "<" : "\"",
                            "right", use_system_include ? ">" : "\"");
                    }

                    printer.Print("\n");
                }

                void codeSerialize::printEnum(google::protobuf::io::Printer& printer)const {
                    for (int i = 0; i < _file->enum_type_count(); ++i) {
                        const EnumDescriptor* item = _file->enum_type(i);
                        printer.Print("\n");
                        printer.Print("    enum $enumName$ \{\n", "enumName", item->name());
                        for (int idx = 0; idx < item->value_count(); ++idx) {
                            const EnumValueDescriptor* value = item->value(idx);
                            char sz[64] = { 0 };
                            sprintf(sz, "%d", value->number());
                            printer.Print("        $valueName$ = $value$,\n", "valueName", value->name(), "value", sz);
                        }
                        printer.Print("    };\n");
                    }
                }

                void codeSerialize::printStruct(google::protobuf::io::Printer& printer, FileDescriptor::Syntax syntax)const {
                    uint32_t size = _message_generators.size();
                    for (uint32_t i = 0; i < size; ++i) {
                        const FieldDescriptorArr& messages = _message_generators.at(i);
                        if (messages._vec.empty() || messages._name.find("Entry_DoNotUse") != std::string::npos)
                            continue;
                        printer.Print("\n");
                        printer.Print("    struct $struName$ \{\n        $struName$\()", "struName", messages._name);
                        std::string fields;
                        uint32_t message_size = messages._vec.size();
                        for (uint32_t idx = 0, flag = 0; idx < message_size; ++idx) {
                            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                                bool init = (!field->is_map() && !field->is_repeated()
                                    && field->type() != FieldDescriptor::TYPE_STRING
                                    && field->type() != FieldDescriptor::TYPE_MESSAGE);
                                if (init) {
                                    if (!flag) printer.Print(" : "); else printer.Print(", ");
                                    printer.Print("$fieldName$\(", "fieldName", FieldName(*field));
                                    if (field->has_default_value())
                                        printer.Print("$value$", "value", type2value(*field));
                                    printer.Print(")");
                                    flag = 1;
                                }
                                //has init
                                if (syntax == FileDescriptor::SYNTAX_PROTO2) {
                                    if (field->label() == FieldDescriptor::LABEL_OPTIONAL)
                                        printer.Print(", has_$fieldName$\(false)", "fieldName", FieldName(*field));
                                }
                                fields.append("    ");
                                if (field->is_map()) {
                                    fields.append("    std::map<").append(map2string(*field)).append(">");
                                } else if (field->is_repeated()) {
                                    fields.append("    std::vector<").append(type2string(*field, " ")).append(">");
                                } else {
                                    fields.append("    ").append(type2string(*field));
                                }
                                fields.append(" ").append(FieldName(*field)).append(";\n");
                                //has declare
                                if (syntax == FileDescriptor::SYNTAX_PROTO2) {
                                    if (field->label() == FieldDescriptor::LABEL_OPTIONAL)
                                        fields.append("        bool has_").append(FieldName(*field)).append(";\n");
                                }
                            }
                        }

                        printer.Print(" {}\n$fields$", "fields", fields);

                        printer.Print("\n        template <typename T>\n        void serialize(T& t) {\n            t", "nameSpace", _file->package(), "struName", messages._name);
                        for (uint32_t idx = 0; idx < message_size; ++idx) {
                            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                                char sz[20] = { 0 };
                                sprintf(sz, "%d", field->number());
                                const std::string& strOrgName = field->name();
                                std::string fieldName(FieldName(*field));
                                if (strOrgName != fieldName) {
                                    printer.Print(" & serialize::makeItem(\"$orgName$\", $number$, $field$$tag$)", "orgName", strOrgName, "number", sz, "field", fieldName, "tag", type2tag(*field, syntax));
                                } else {
                                    printer.Print(" & SERIALIZATION($number$, $field$$tag$)", "number", sz, "field", fieldName, "tag", type2tag(*field, syntax));
                                }
                            }
                        }

                        printer.Print(";\n        }\n    };\n");
                    }
                }

                void codeSerialize::printSerialize(google::protobuf::io::Printer& printer)const {
                    uint32_t size = _message_generators.size();
                    for (uint32_t i = 0; i < size; ++i) {
                        const FieldDescriptorArr& messages = _message_generators.at(i);
                        if (messages._vec.empty()) continue;
                        printer.Print("\ntemplate <typename T>\ninline void serialize(T& t, $nameSpace$::$struName$& item) {\n    t", "nameSpace", _file->package(), "struName", messages._name);
                        uint32_t message_size = messages._vec.size();
                        for (uint32_t idx = 0; idx < message_size; ++idx) {
                            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                                char sz[20] = { 0 };
                                sprintf(sz, "%d", field->number());
                                printer.Print(" & SERIALIZATION($number$, item.$field$$tag$)", "number", sz, "field", field->name(), "tag", type2tag(*field, _file->syntax()));
                            }
                        }
                        printer.Print(";\n}\n");
                    }
                }

                bool codeSerialize::hasInt(google::protobuf::io::Printer& printer)const {
                    uint32_t size = _message_generators.size();
                    for (uint32_t i = 0; i < size; ++i) {
                        const FieldDescriptorArr& messages = _message_generators.at(i);
                        uint32_t message_size = messages._vec.size();
                        for (uint32_t idx = 0; idx < message_size; ++idx) {
                            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                                FieldDescriptor::Type type = field->type();
                                if (type == FieldDescriptor::TYPE_INT64
                                    || type == FieldDescriptor::TYPE_UINT64
                                    || type == FieldDescriptor::TYPE_INT32
                                    || type == FieldDescriptor::TYPE_FIXED64
                                    || type == FieldDescriptor::TYPE_FIXED32
                                    || type == FieldDescriptor::TYPE_UINT32
                                    || type == FieldDescriptor::TYPE_ENUM
                                    || type == FieldDescriptor::TYPE_SFIXED32
                                    || type == FieldDescriptor::TYPE_SFIXED64
                                    || type == FieldDescriptor::TYPE_SINT32
                                    || type == FieldDescriptor::TYPE_SINT64) {
                                    return true;
                                }
                            }
                        }
                    }
                    return false;
                }

                bool codeSerialize::hasString(google::protobuf::io::Printer& printer)const {
                    uint32_t size = _message_generators.size();
                    for (uint32_t i = 0; i < size; ++i) {
                        const FieldDescriptorArr& messages = _message_generators.at(i);
                        uint32_t message_size = messages._vec.size();
                        for (uint32_t idx = 0; idx < message_size; ++idx) {
                            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                                if (field->type() == FieldDescriptor::TYPE_STRING || field->type() == FieldDescriptor::TYPE_BYTES) {
                                    return true;
                                }
                            }
                        }
                    }
                    return false;
                }

                bool codeSerialize::hasVector(google::protobuf::io::Printer& printer)const {
                    uint32_t size = _message_generators.size();
                    for (uint32_t i = 0; i < size; ++i) {
                        const FieldDescriptorArr& messages = _message_generators.at(i);
                        uint32_t message_size = messages._vec.size();
                        for (uint32_t idx = 0; idx < message_size; ++idx) {
                            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                                if (field->is_repeated()) {
                                    return true;
                                }
                            }
                        }
                    }
                    return false;
                }
                bool codeSerialize::hasMap(google::protobuf::io::Printer& printer)const {
                    uint32_t size = _message_generators.size();
                    for (uint32_t i = 0; i < size; ++i) {
                        const FieldDescriptorArr& messages = _message_generators.at(i);
                        uint32_t message_size = messages._vec.size();
                        for (uint32_t idx = 0; idx < message_size; ++idx) {
                            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                                if (field->is_map()) {
                                    return true;
                                }
                            }
                        }
                    }
                    return false;
                }

                std::string codeSerialize::FieldName(const FieldDescriptor& field) {
                    string result = field.name();
                    LowerString(&result);
                    if (kKeywords.count(result) > 0) {
                        result.append("_");
                    }
                    return result;
                }
            }
        }
    }
}