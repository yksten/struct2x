#ifndef GOOGLE_PROTOBUF_COMPILER_STRUCT_GENERATOR_H__
#define GOOGLE_PROTOBUF_COMPILER_STRUCT_GENERATOR_H__

#include <string>
#include <google/protobuf/compiler/code_generator.h>

namespace google {
    namespace protobuf {
        namespace compiler {
            namespace cpp {

class LIBPROTOC_EXPORT StructGenerator : public CodeGenerator {
public:
    StructGenerator();
    ~StructGenerator();

    // implements CodeGenerator ----------------------------------------
    bool Generate(const FileDescriptor* file,
        const string& parameter,
        GeneratorContext* generator_context,
        string* error) const;

private:
    GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(StructGenerator);
};

            }  // namespace cpp
        }  // namespace compiler
    }  // namespace protobuf

}  // namespace google
#endif  // GOOGLE_PROTOBUF_COMPILER_STRUCT_GENERATOR_H__
