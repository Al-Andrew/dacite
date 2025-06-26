#pragma once

#include <string>
#include <variant>

namespace dacite {

/// Value types supported by the VM
enum class ValueType {
    NIL,
    INTEGER,
    BOOLEAN,
    FUNCTION
};

/// A discriminated union representing values in the VM
class Value {
public:
    /// Default constructor creates NIL value
    Value() : data_(std::monostate{}) {}
    
    /// Constructor for integer values
    explicit Value(int32_t value) : data_(value) {}
    
    /// Constructor for boolean values
    explicit Value(bool value) : data_(value) {}
    
    /// Get the type of this value
    ValueType get_type() const;
    
    /// Check if this value is of a specific type
    bool is_nil() const { return std::holds_alternative<std::monostate>(data_); }
    bool is_integer() const { return std::holds_alternative<int32_t>(data_); }
    bool is_boolean() const { return std::holds_alternative<bool>(data_); }
    
    /// Get the integer value (throws if not integer)
    int32_t as_integer() const;
    
    /// Get the boolean value (throws if not boolean)
    bool as_boolean() const;
    
    /// Convert to string for debugging
    std::string to_string() const;
    
    /// Equality comparison
    bool operator==(const Value& other) const;
    bool operator!=(const Value& other) const { return !(*this == other); }

private:
    std::variant<std::monostate, int32_t, bool> data_;
};

} // namespace dacite