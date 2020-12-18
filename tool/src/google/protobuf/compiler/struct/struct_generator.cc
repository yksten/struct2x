#include <google/protobuf/compiler/struct/struct_generator.h>

#include <vector>
#include <memory>
#ifndef _SHARED_PTR_H
#include <google/protobuf/stubs/shared_ptr.h>
#endif
#include <utility>

#include <google/protobuf/compiler/cpp/cpp_file.h>
#include <google/protobuf/compiler/cpp/cpp_helpers.h>
#include <google/protobuf/io/printer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/stubs/strutil.h>

#include <google/protobuf/compiler/struct/struct_serialize.h>

namespace google {
namespace protobuf {
namespace compiler {
namespace cpp {

StructGenerator::StructGenerator() {}
StructGenerator::~StructGenerator() {}

bool StructGenerator::Generate(const FileDescriptor* file, const string& parameter, GeneratorContext* generator_context, string* error) const {
    std::vector<std::pair<string, string> > options;
    ParseGeneratorParameter(parameter, &options);

    Options file_options;
    string basename = StripProto(file->name());
    FileGenerator file_generator(file, file_options);
    compiler::cpp::codeSerialize obj(file, file_options);
    
    google::protobuf::scoped_ptr<io::ZeroCopyOutputStream> outputHeader(generator_context->Open(basename + ".pb.h"));
    io::Printer printerHeader(outputHeader.get(), '$', NULL);
    obj.printHeader(printerHeader, basename.c_str());

    return true;
}

}  // namespace cpp
}  // namespace compiler
}  // namespace protobuf
}  // namespace google
