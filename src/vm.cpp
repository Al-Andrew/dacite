#include "vm.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

namespace dacite {

VM::VM(const VMConfig& config) : config_(config) {
    reset();
}

VMResult VM::run(const Chunk& chunk) {
    if (chunk.empty()) {
        return VMResult::OK;
    }
    
    const auto& code = chunk.get_code();
    size_t ip = 0; // instruction pointer
    
    debug_print("=== VM Execution ===");
    
    while (ip < code.size()) {
        if (config_.debug_mode) {
            debug_print_stack();
            debug_print_instruction(chunk, ip);
        }
        
        OpCode instruction = static_cast<OpCode>(code[ip]);
        ip++;
        
        switch (instruction) {
            case OpCode::OP_CONSTANT: {
                if (ip >= code.size()) {
                    runtime_error("Missing constant index after OP_CONSTANT");
                    return VMResult::RUNTIME_ERROR;
                }
                uint8_t constant_index = code[ip];
                ip++;
                
                try {
                    const Value& constant = chunk.get_constant(constant_index);
                    push(constant);
                } catch (const std::exception& e) {
                    runtime_error("Invalid constant index: " + std::string(e.what()));
                    return VMResult::RUNTIME_ERROR;
                }
                break;
            }
            
            case OpCode::OP_RETURN: {
                if (stack_.empty()) {
                    runtime_error("Cannot return: stack is empty");
                    return VMResult::RUNTIME_ERROR;
                }
                Value result = pop();
                debug_print("Function returned: " + result.to_string());
                // For now, we just leave the result on the stack
                push(result);
                return VMResult::OK;
            }
            
            // Arithmetic operations
            case OpCode::OP_ADD: {
                if (stack_.size() < 2) {
                    runtime_error("Not enough values on stack for addition");
                    return VMResult::RUNTIME_ERROR;
                }
                Value b = pop();
                Value a = pop();
                if (!a.is_integer() || !b.is_integer()) {
                    runtime_error("Addition requires integer values");
                    return VMResult::RUNTIME_ERROR;
                }
                push(Value(a.as_integer() + b.as_integer()));
                break;
            }
            
            case OpCode::OP_SUBTRACT: {
                if (stack_.size() < 2) {
                    runtime_error("Not enough values on stack for subtraction");
                    return VMResult::RUNTIME_ERROR;
                }
                Value b = pop();
                Value a = pop();
                if (!a.is_integer() || !b.is_integer()) {
                    runtime_error("Subtraction requires integer values");
                    return VMResult::RUNTIME_ERROR;
                }
                push(Value(a.as_integer() - b.as_integer()));
                break;
            }
            
            case OpCode::OP_MULTIPLY: {
                if (stack_.size() < 2) {
                    runtime_error("Not enough values on stack for multiplication");
                    return VMResult::RUNTIME_ERROR;
                }
                Value b = pop();
                Value a = pop();
                if (!a.is_integer() || !b.is_integer()) {
                    runtime_error("Multiplication requires integer values");
                    return VMResult::RUNTIME_ERROR;
                }
                push(Value(a.as_integer() * b.as_integer()));
                break;
            }
            
            case OpCode::OP_DIVIDE: {
                if (stack_.size() < 2) {
                    runtime_error("Not enough values on stack for division");
                    return VMResult::RUNTIME_ERROR;
                }
                Value b = pop();
                Value a = pop();
                if (!a.is_integer() || !b.is_integer()) {
                    runtime_error("Division requires integer values");
                    return VMResult::RUNTIME_ERROR;
                }
                if (b.as_integer() == 0) {
                    runtime_error("Division by zero");
                    return VMResult::RUNTIME_ERROR;
                }
                push(Value(a.as_integer() / b.as_integer()));
                break;
            }
            
            // Comparison operations
            case OpCode::OP_EQUAL: {
                if (stack_.size() < 2) {
                    runtime_error("Not enough values on stack for equality comparison");
                    return VMResult::RUNTIME_ERROR;
                }
                Value b = pop();
                Value a = pop();
                push(Value(a == b));
                break;
            }
            
            case OpCode::OP_NOT_EQUAL: {
                if (stack_.size() < 2) {
                    runtime_error("Not enough values on stack for inequality comparison");
                    return VMResult::RUNTIME_ERROR;
                }
                Value b = pop();
                Value a = pop();
                push(Value(a != b));
                break;
            }
            
            case OpCode::OP_LESS: {
                if (stack_.size() < 2) {
                    runtime_error("Not enough values on stack for less than comparison");
                    return VMResult::RUNTIME_ERROR;
                }
                Value b = pop();
                Value a = pop();
                if (!a.is_integer() || !b.is_integer()) {
                    runtime_error("Less than comparison requires integer values");
                    return VMResult::RUNTIME_ERROR;
                }
                push(Value(a.as_integer() < b.as_integer()));
                break;
            }
            
            case OpCode::OP_LESS_EQUAL: {
                if (stack_.size() < 2) {
                    runtime_error("Not enough values on stack for less or equal comparison");
                    return VMResult::RUNTIME_ERROR;
                }
                Value b = pop();
                Value a = pop();
                if (!a.is_integer() || !b.is_integer()) {
                    runtime_error("Less or equal comparison requires integer values");
                    return VMResult::RUNTIME_ERROR;
                }
                push(Value(a.as_integer() <= b.as_integer()));
                break;
            }
            
            case OpCode::OP_GREATER: {
                if (stack_.size() < 2) {
                    runtime_error("Not enough values on stack for greater than comparison");
                    return VMResult::RUNTIME_ERROR;
                }
                Value b = pop();
                Value a = pop();
                if (!a.is_integer() || !b.is_integer()) {
                    runtime_error("Greater than comparison requires integer values");
                    return VMResult::RUNTIME_ERROR;
                }
                push(Value(a.as_integer() > b.as_integer()));
                break;
            }
            
            case OpCode::OP_GREATER_EQUAL: {
                if (stack_.size() < 2) {
                    runtime_error("Not enough values on stack for greater or equal comparison");
                    return VMResult::RUNTIME_ERROR;
                }
                Value b = pop();
                Value a = pop();
                if (!a.is_integer() || !b.is_integer()) {
                    runtime_error("Greater or equal comparison requires integer values");
                    return VMResult::RUNTIME_ERROR;
                }
                push(Value(a.as_integer() >= b.as_integer()));
                break;
            }
            
            default: {
                runtime_error("Unknown opcode: " + std::to_string(static_cast<int>(instruction)));
                return VMResult::RUNTIME_ERROR;
            }
        }
    }
    
    return VMResult::OK;
}

Value VM::peek_stack_top() const {
    if (stack_.empty()) {
        throw std::runtime_error("Stack is empty");
    }
    return stack_.back();
}

void VM::reset() {
    stack_.clear();
    error_message_.clear();
}

void VM::push(const Value& value) {
    if (stack_.size() >= config_.max_stack_size) {
        runtime_error("Stack overflow");
        return;
    }
    stack_.push_back(value);
    debug_print("Pushed: " + value.to_string());
}

Value VM::pop() {
    if (stack_.empty()) {
        throw std::runtime_error("Stack underflow");
    }
    Value value = stack_.back();
    stack_.pop_back();
    debug_print("Popped: " + value.to_string());
    return value;
}

Value VM::peek(size_t distance) const {
    if (distance >= stack_.size()) {
        throw std::runtime_error("Stack peek out of bounds");
    }
    return stack_[stack_.size() - 1 - distance];
}

void VM::runtime_error(const std::string& message) {
    error_message_ = message;
    if (config_.debug_mode) {
        std::cerr << "Runtime error: " << message << std::endl;
    }
}

void VM::debug_print(const std::string& message) const {
    if (config_.debug_mode) {
        std::cout << "[VM] " << message << std::endl;
    }
}

void VM::debug_print_instruction(const Chunk& chunk, size_t offset) const {
    if (!config_.debug_mode) return;
    
    std::cout << "[VM] " << std::setw(4) << std::setfill('0') << offset << " ";
    
    OpCode instruction = static_cast<OpCode>(chunk.get_code()[offset]);
    switch (instruction) {
        case OpCode::OP_CONSTANT: {
            uint8_t constant_index = chunk.get_code()[offset + 1];
            std::cout << "OP_CONSTANT " << static_cast<int>(constant_index);
            if (constant_index < chunk.get_constants().size()) {
                std::cout << " (" << chunk.get_constant(constant_index).to_string() << ")";
            }
            break;
        }
        case OpCode::OP_RETURN:
            std::cout << "OP_RETURN";
            break;
        default:
            std::cout << "UNKNOWN_OP";
            break;
    }
    std::cout << std::endl;
}

void VM::debug_print_stack() const {
    if (!config_.debug_mode) return;
    
    std::cout << "[VM] Stack: ";
    if (stack_.empty()) {
        std::cout << "[]";
    } else {
        std::cout << "[";
        for (size_t i = 0; i < stack_.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << stack_[i].to_string();
        }
        std::cout << "]";
    }
    std::cout << std::endl;
}

std::string VM::stack_to_string() const {
    std::ostringstream oss;
    oss << "[";
    for (size_t i = 0; i < stack_.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << stack_[i].to_string();
    }
    oss << "]";
    return oss.str();
}

} // namespace dacite