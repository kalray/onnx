#pragma once

#include <stdexcept>
#include <unordered_map>
#include "onnx/onnx_pb.h"
#include "onnx/string_utils.h"

namespace onnx {
namespace checker {
class ValidationError : public std::runtime_error {
 public:
  using std::runtime_error::runtime_error;
  const char* what() const noexcept override {
    if (!expanded_message_.empty()) {
      return expanded_message_.c_str();
    }
    return std::runtime_error::what();
  }
  void AppendContext(const std::string& context) {
    expanded_message_ = onnx::MakeString(
        std::runtime_error::what(), "\n\n==> Context: ", context);
  }

 private:
  std::string expanded_message_;
};

#define fail_check(...) \
  throw onnx::checker::ValidationError(onnx::MakeString(__VA_ARGS__));

class CheckerContext {
  int ir_version;
  std::unordered_map<std::string, int> opset_imports;
public:
  int get_ir_version() const { return ir_version; }
  void set_ir_version(int v) { ir_version = v; }
  const std::unordered_map<std::string, int>& get_opset_imports() const {
    return opset_imports;
  }
  void set_opset_imports(const std::unordered_map<std::string, int>& imps) {
    opset_imports = imps;
  }
  explicit CheckerContext() : ir_version(-1) {}
};

using IR_VERSION_TYPE = decltype(Version::IR_VERSION);
void check_value_info(const ValueInfoProto& value_info, const CheckerContext&);
void check_tensor(const TensorProto& tensor, const CheckerContext&);
void check_attribute(const AttributeProto& attr, const CheckerContext&);
void check_node(const NodeProto& node, const CheckerContext&);
void check_graph(const GraphProto& graph, const CheckerContext&);
void check_model(const ModelProto& model);
} // namespace checker
} // namespace onnx
