#include "value.h"
#include <stdexcept>

namespace dacite {

ValueType Value::get_type() const {
    if (std::holds_alternative<std::monostate>(data_)) {
        return ValueType::NIL;
    } else if (std::holds_alternative<int32_t>(data_)) {
        return ValueType::INTEGER;
    }
    // Should never happen with current implementation
    throw std::runtime_error("Unknown value type");
}

int32_t Value::as_integer() const {
    if (!is_integer()) {
        throw std::runtime_error("Value is not an integer");
    }
    return std::get<int32_t>(data_);
}

std::string Value::to_string() const {
    switch (get_type()) {
        case ValueType::NIL:
            return "nil";
        case ValueType::INTEGER:
            return std::to_string(as_integer());
        case ValueType::FUNCTION:
            return "<function>";
    }
    return "<unknown>";
}

bool Value::operator==(const Value& other) const {
    return data_ == other.data_;
}

} // namespace dacite