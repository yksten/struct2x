#include "struct_serialize.h"

#include <google/protobuf/compiler/cpp/cpp_message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/io/printer.h>

namespace google {
    namespace protobuf {
        namespace compiler {
            namespace cpp {

                const char* type2string(const FieldDescriptor& field) {
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
                    case FieldDescriptor::TYPE_ENUM:
                        return "int32_t";
                    case FieldDescriptor::TYPE_BOOL:
                        return "bool";
                    case FieldDescriptor::TYPE_STRING:
                    case FieldDescriptor::TYPE_BYTES:
                        return "std::string";
                    default:
                        return field.message_type()->name().c_str();
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

                const char* type2tag(const FieldDescriptor& field, FileDescriptor::Syntax syntax) {
                    static std::string strResult;
                    strResult.clear();
                    switch (field.type()) {
                    case FieldDescriptor::TYPE_FIXED64:
                    case FieldDescriptor::TYPE_SFIXED64:
                        strResult.append(", serialization::TYPE_FIXED64");
                        break;
                    case FieldDescriptor::TYPE_SINT32:
                    case FieldDescriptor::TYPE_SINT64:
                        strResult.append(", serialization::TYPE_SVARINT");
                        break;
                    case FieldDescriptor::TYPE_FIXED32:
                    case FieldDescriptor::TYPE_SFIXED32:
                        strResult.append(", serialization::TYPE_FIXED32");
                        break;
                    }

                    if (field.is_packed())
                        strResult.append(", serialization::TYPE_PACK");

                    if (syntax == FileDescriptor::SYNTAX_PROTO2) {
                        if (field.label() == FieldDescriptor::LABEL_OPTIONAL)
                            strResult.append(", &has_").append(field.name());
                    }

                    return strResult.c_str();
                }

                codeSerialize::codeSerialize(const FileDescriptor* file, const Options& options) :scc_analyzer_(options), _file(file) {
                    std::vector<const Descriptor*> msgs = FlattenMessagesInFile(file);
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
                }

                codeSerialize::~codeSerialize() {
                }

                void codeSerialize::print(google::protobuf::io::Printer& printer, const char* szName)const {
                    std::string strStructName(szName), strNameSpace;
                    if (!(_file->package()).empty())
                        strNameSpace.append((_file->package())).append("_");
                    std::transform(strStructName.begin(), strStructName.end(), strStructName.begin(), ::toupper);
                    std::transform(strNameSpace.begin(), strNameSpace.end(), strNameSpace.begin(), ::toupper);
                    //1.program once
                    printer.Print("#ifndef __$nameSpace$$basename$__H_\n#define __$nameSpace$$basename$__H_\n", "basename", strStructName, "nameSpace", strNameSpace);
                    //2.include
                    printInclude(printer);
                    //3.namespace
                    //struct
                    //constructed function
                    //Initialization fidlds
                    //fields
                    printStruct(printer, _file->syntax());
                    //4.serialize functions
                    //printSerialize(printer);
                    printer.Print("\n#endif\n");
                }

                void codeSerialize::printInclude(google::protobuf::io::Printer& printer)const {
                    printer.Print("\n");
                    printer.Print("#include \"struct2x.h\"\n");
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
                    printer.Print("\n");
                }

                void codeSerialize::printStruct(google::protobuf::io::Printer& printer, FileDescriptor::Syntax syntax)const {
                    if (!_file->package().empty())
                        printer.Print("namespace $name$ \{\n\n", "name", _file->package());
                    uint32_t size = _message_generators.size();
                    for (uint32_t i = 0; i < size; ++i) {
                        const FieldDescriptorArr& messages = _message_generators.at(i);
                        if (messages._vec.empty() || messages._name.find("Entry_DoNotUse") != std::string::npos) continue;
                        if (i) printer.Print("\n");
                        printer.Print("    struct $struName$ \{\n        $struName$\()", "struName", messages._name);
                        std::string fields;
                        uint32_t message_size = messages._vec.size();
                        for (uint32_t idx = 0, flag = 0; idx < message_size; ++idx) {
                            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                                bool init = (!field->is_map() && !field->is_repeated()
                                    && field->type() != FieldDescriptor::TYPE_STRING
                                    && field->type() != FieldDescriptor::TYPE_MESSAGE);
                                if (init) {
                                    if (!flag) printer.Print(": "); else printer.Print(", ");
                                    printer.Print("$fieldName$\(", "fieldName", field->name());
                                    if (field->has_default_value())
                                        printer.Print("$value$", "value", type2value(*field));
                                    printer.Print(")");
                                    flag = 1;
                                }
                                //has init
                                if (syntax == FileDescriptor::SYNTAX_PROTO2) {
                                    if (field->label() == FieldDescriptor::LABEL_OPTIONAL)
                                        printer.Print(", has_$fieldName$\(false)", "fieldName", field->name());
                                }
                                fields.append("    ");
                                if (field->is_map()) {
                                    fields.append("    std::map<").append(map2string(*field)).append(">");
                                } else if (field->is_repeated()) {
                                    fields.append("    std::vector<").append(type2string(*field)).append(">");
                                } else {
                                    fields.append("    ").append(type2string(*field));
                                }
                                fields.append(" ").append(field->name()).append(";\n");
                                //has declare
                                if (syntax == FileDescriptor::SYNTAX_PROTO2) {
                                    if (field->label() == FieldDescriptor::LABEL_OPTIONAL)
                                        fields.append("        bool has_").append(field->name()).append(";\n");
                                }
                            }
                        }

                        printer.Print(" {}\n$fields$", "fields", fields);

                        printer.Print("\n        template<typename T>\n        void serialize(T& t) {\n            t", "nameSpace", _file->package(), "struName", messages._name);
                        for (uint32_t idx = 0; idx < message_size; ++idx) {
                            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                                char sz[20] = { 0 };
                                sprintf(sz, "%d", field->number());
                                printer.Print(" & SERIALIZE($number$, $field$$tag$)", "number", sz, "field", field->name(), "tag", type2tag(*field, syntax));
                            }
                        }
                        printer.Print(";\n        }\n    \};\n");

                    }
                    if (!_file->package().empty())
                        printer.Print("\n}\n");
                }



                void codeSerialize::printSerialize(google::protobuf::io::Printer& printer)const {
                    uint32_t size = _message_generators.size();
                    for (uint32_t i = 0; i < size; ++i) {
                        const FieldDescriptorArr& messages = _message_generators.at(i);
                        if (messages._vec.empty()) continue;
                        printer.Print("\ntemplate<typename T>\ninline void serialize(T& t, $nameSpace$::$struName$& item) {\n    t", "nameSpace", _file->package(), "struName", messages._name);
                        uint32_t message_size = messages._vec.size();
                        for (uint32_t idx = 0, flag = 0; idx < message_size; ++idx) {
                            if (const FieldDescriptor* field = messages._vec.at(idx)) {
                                char sz[20] = { 0 };
                                sprintf(sz, "%d", field->number());
                                printer.Print(" & SERIALIZE($number$, item.$field$$tag$)", "number", sz, "field", field->name(), "tag", type2tag(*field, _file->syntax()));
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
            }
        }
    }
}