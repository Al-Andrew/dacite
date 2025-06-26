#pragma once

#include <vector>
#include <cstdint>
#include "value.h"

namespace dacite {

/// Opcodes for the VM
enum class OpCode : uint8_t {
    OP_CONSTANT,        // Load a constant from the constant pool
    OP_RETURN,          // Return from function
    
    // Arithmetic operations
    OP_ADD,             // Pop two values, add them, push result
    OP_SUBTRACT,        // Pop two values, subtract them, push result  
    OP_MULTIPLY,        // Pop two values, multiply them, push result
    OP_DIVIDE,          // Pop two values, divide them, push result
    
    // Comparison operations
    OP_EQUAL,           // Pop two values, compare for equality, push result
    OP_NOT_EQUAL,       // Pop two values, compare for inequality, push result
    OP_LESS,            // Pop two values, compare less than, push result
    OP_LESS_EQUAL,      // Pop two values, compare less or equal, push result
    OP_GREATER,         // Pop two values, compare greater than, push result
    OP_GREATER_EQUAL,   // Pop two values, compare greater or equal, push result
};

/// A chunk of bytecode with associated constants
class Chunk {
public:
    /// Default constructor
    Chunk() = default;
    
    /// Write a byte to the chunk
    void write_byte(uint8_t byte);
    
    /// Write an opcode to the chunk
    void write_opcode(OpCode opcode);
    
    /// Add a constant to the constant pool and return its index
    size_t add_constant(const Value& value);
    
    /// Get the bytecode
    const std::vector<uint8_t>& get_code() const { return code_; }
    
    /// Get the constants
    const std::vector<Value>& get_constants() const { return constants_; }
    
    /// Get a constant by index
    const Value& get_constant(size_t index) const;
    
    /// Get the size of the bytecode
    size_t size() const { return code_.size(); }
    
    /// Check if chunk is empty
    bool empty() const { return code_.empty(); }
    
    /// Clear the chunk
    void clear();
    
    /// Debug: Convert chunk to string representation
    std::string to_string() const;

private:
    std::vector<uint8_t> code_;        // Bytecode instructions
    std::vector<Value> constants_;     // Constant pool
};

} // namespace dacite