#include "chunk.h"
#include <stdexcept>
#include <sstream>

namespace dacite {

void Chunk::write_byte(uint8_t byte) {
    code_.push_back(byte);
}

void Chunk::write_opcode(OpCode opcode) {
    write_byte(static_cast<uint8_t>(opcode));
}

size_t Chunk::add_constant(const Value& value) {
    constants_.push_back(value);
    return constants_.size() - 1;
}

const Value& Chunk::get_constant(size_t index) const {
    if (index >= constants_.size()) {
        throw std::out_of_range("Constant index out of range");
    }
    return constants_[index];
}

void Chunk::clear() {
    code_.clear();
    constants_.clear();
}

std::string Chunk::to_string() const {
    std::ostringstream oss;
    oss << "Chunk {\n";
    oss << "  Code: [";
    for (size_t i = 0; i < code_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << static_cast<int>(code_[i]);
    }
    oss << "]\n";
    oss << "  Constants: [";
    for (size_t i = 0; i < constants_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << constants_[i].to_string();
    }
    oss << "]\n}";
    return oss.str();
}

} // namespace dacite