#include "tracer/obj_parser/ast.h"

namespace tracer {
namespace obj_parser {

SmoothNode::SmoothNode(const std::string& val) : str_value(val) {}
SmoothNode::SmoothNode(int number) : value(number) {}
VertexNode::VertexNode(const Vec3 &vertex) : value(vertex) {}
NormalNode::NormalNode(const Vec3 &normal) : value(normal) {}
TextureCoordNode::TextureCoordNode(const Vec2 &val) : value(val) {}
FaceNode::FaceNode(const Face &face) : value(face) {}
MtlLibNode::MtlLibNode(const std::string &val) : value(val) {}
UseMtlNode::UseMtlNode(const std::string &val) : value(val) {}
TexParamsNode::TexParamsNode(const TexParams &val) : value(val) {}

void SmoothNode::evaluate(Value &value) {
  value.v_number = this->value;
  value.v_type = Value::ValueType::V_NUMBER;
}

void VertexNode::evaluate(Value &value) {
  value.v_vertex = this->value;
  value.v_type = Value::ValueType::V_VERTEX;
}

void NormalNode::evaluate(Value &value) {
  value.v_normal = this->value;
  value.v_type = Value::ValueType::V_NORMAL;
}

void TextureCoordNode::evaluate(Value &value) {
  value.v_texture_coord = this->value;
  value.v_type = Value::ValueType::V_TEXTURE;
}

void FaceNode::evaluate(Value &value) {
  value.v_face = this->value;
  value.v_type = Value::ValueType::V_FACE;
}

void MtlLibNode::evaluate(Value &value) {
  value.v_mat_path = this->value;
  value.v_type = Value::ValueType::V_MAT_PATH;
}

void UseMtlNode::evaluate(Value &value) {
  value.v_mat_name = this->value;
  value.v_type = Value::ValueType::V_MAT_NAME;
}

void TexParamsNode::evaluate(Value &value) {
  value.v_params.push_back(this->value);
}

} // namespace obj_parser
} // namespace tracer