#pragma once

#include <string>
#include <variant>

namespace dacite {

/// Value types supported by the VM
enum class ValueType {
    NIL,
    INTEGER,
    FUNCTION
};

/// A discriminated union representing values in the VM
class Value {
public:
    /// Default constructor creates NIL value
    Value() : data_(std::monostate{}) {}
    
    /// Constructor for integer values
    explicit Value(int32_t value) : data_(value) {}
    
    /// Get the type of this value
    ValueType get_type() const;
    
    /// Check if this value is of a specific type
    bool is_nil() const { return std::holds_alternative<std::monostate>(data_); }
    bool is_integer() const { return std::holds_alternative<int32_t>(data_); }
    
    /// Get the integer value (throws if not integer)
    int32_t as_integer() const;
    
    /// Convert to string for debugging
    std::string to_string() const;
    
    /// Equality comparison
    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const { return !(*this == other); }

private:
    std::variant<std::monostate, int32_t> data_;
};

} // namespace dacite