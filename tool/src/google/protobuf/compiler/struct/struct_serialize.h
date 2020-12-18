#ifndef __STRUCT_SERIALIZE_H__
#define __STRUCT_SERIALIZE_H__

#include <vector>
#include <google/protobuf/compiler/cpp/cpp_helpers.h>

namespace google {
    namespace protobuf {
        class FileDescriptor;
        namespace io {
            class Printer;
        }

        namespace compiler {
            namespace cpp {

                struct Options;

                class codeSerialize {
                    struct FieldDescriptorArr {
                        std::vector<const FieldDescriptor *> _vec;
                        std::string _name;
                    };
                    std::vector<FieldDescriptorArr> _message_generators;
                    SCCAnalyzer scc_analyzer_;
                    const FileDescriptor* _file;
                public:
                    codeSerialize(const FileDescriptor* file, const Options& options);
                    ~codeSerialize();

                    void printHeader(google::protobuf::io::Printer& printer, const char* szName)const;
                    static std::string FieldName(const FieldDescriptor& field);
                private:
                    void prepareMsgs();
                    static bool sortMsgs(std::vector<FieldDescriptorArr>& msgs);
                    static uint32_t getInsertIdx(const std::vector<FieldDescriptorArr>& msgs, const FieldDescriptorArr& item);
                    void printInclude(google::protobuf::io::Printer& printer)const;
                    void printEnum(google::protobuf::io::Printer& printer)const;
                    void printStruct(google::protobuf::io::Printer& printer, FileDescriptor::Syntax syntax)const;
                    void printSerialize(google::protobuf::io::Printer& printer)const;
                    bool hasInt(google::protobuf::io::Printer& printer)const;
                    bool hasString(google::protobuf::io::Printer& printer)const;
                    bool hasVector(google::protobuf::io::Printer& printer)const;
                    bool hasMap(google::protobuf::io::Printer& printer)const;
                };

            }
        }
    }
}

#endif
