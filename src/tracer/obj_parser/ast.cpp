#include "tracer/obj_parser/ast.h"

namespace tracer {
namespace obj_parser {

MtlLibNode::MtlLibNode(const std::string &val) : value(val) {}
VertexNode::VertexNode(const Vec3 &vertex) : value(vertex) {}
TextureCoordNode::TextureCoordNode(const Vec2 &val) : value(val) {}
NormalNode::NormalNode(const Vec3 &normal) : value(normal) {}
ObjectNode::ObjectNode(const std::string &val) : value(val) {}
GroupNode::GroupNode(const std::string &val) : value(val) {}
UseMtlNode::UseMtlNode(const std::string &val) : value(val) {}
SmoothNode::SmoothNode(const std::string &val) : str_value(val) {}
SmoothNode::SmoothNode(int number) : value(number) {}
FaceNode::FaceNode(const Face &face) : value(face) {}
MaterialParamsNode::MaterialParamsNode(const MaterialParams &val)
    : value(val) {}

void MtlLibNode::evaluate(Value &value) {
  value.v_mat_path = this->value;
  value.v_type = Value::ValueType::V_MAT_PATH;
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

void ObjectNode::evaluate(Value &value) {}

void GroupNode::evaluate(Value &value) {}

void UseMtlNode::evaluate(Value &value) {
  value.v_usemtl_name = this->value;
  value.v_type = Value::ValueType::V_USEMTL_NAME;
}

void SmoothNode::evaluate(Value &value) {
  value.v_number = this->value;
  value.v_type = Value::ValueType::V_NUMBER;
}

void FaceNode::evaluate(Value &value) {
  value.v_face = this->value;
  value.v_type = Value::ValueType::V_FACE;
}

void MaterialParamsNode::evaluate(Value &value) {
  value.v_params.push_back(this->value);
  value.v_type = Value::ValueType::V_TEX_PARAMS;
}

} // namespace obj_parser
} // namespace tracer