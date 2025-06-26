#pragma once

#include <vector>
#include <memory>
#include <string>
#include "value.h"
#include "chunk.h"

namespace dacite {

/// Result of VM execution
enum class VMResult {
    OK,
    COMPILE_ERROR,
    RUNTIME_ERROR
};

/// Configuration for the VM
struct VMConfig {
    bool debug_mode = false;
    size_t max_stack_size = 256;
};

/// Stack-based virtual machine
class VM {
public:
    /// Constructor with optional configuration
    explicit VM(const VMConfig& config = {});
    
    /// Execute a chunk of bytecode
    VMResult run(const Chunk& chunk);
    
    /// Get the top value from the stack (for testing)
    Value peek_stack_top() const;
    
    /// Check if stack is empty
    bool is_stack_empty() const { return stack_.empty(); }
    
    /// Get stack size
    size_t get_stack_size() const { return stack_.size(); }
    
    /// Reset the VM state
    void reset();
    
    /// Get any runtime error message
    const std::string& get_error_message() const { return error_message_; }
    
    /// Debug: Get stack contents as string
    std::string stack_to_string() const;

private:
    VMConfig config_;
    std::vector<Value> stack_;
    std::string error_message_;
    
    // Stack operations
    void push(const Value& value);
    Value pop();
    Value peek(size_t distance = 0) const;
    
    // Error handling
    void runtime_error(const std::string& message);
    
    // Debug output
    void debug_print(const std::string& message) const;
    void debug_print_instruction(const Chunk& chunk, size_t offset) const;
    void debug_print_stack() const;
};

} // namespace dacite