#include "compiler.h"
#include <iostream>

namespace dacite {

Compiler::Compiler(const CompilerConfig& config) : config_(config) {}

CompileResult Compiler::compile(const Program& program, Chunk& chunk) {
    debug_print("=== Compilation ===");
    error_message_.clear();
    
    // For this basic implementation, we only support single function programs
    if (program.declarations.empty()) {
        compile_error("No functions to compile");
        return CompileResult::ERROR;
    }
    
    if (program.declarations.size() > 1) {
        compile_error("Multiple functions not yet supported");
        return CompileResult::ERROR;
    }
    
    // Cast to function declaration
    const auto* func_decl = dynamic_cast<const FunctionDeclaration*>(program.declarations[0].get());
    if (!func_decl) {
        compile_error("Expected function declaration");
        return CompileResult::ERROR;
    }
    
    debug_print("Compiling function: " + func_decl->function_name);
    return compile_function(*func_decl, chunk);
}

CompileResult Compiler::compile_function(const FunctionDeclaration& func, Chunk& chunk) {
    // For basic implementation, we expect function to have a body
    if (!func.body) {
        compile_error("Function has no body");
        return CompileResult::ERROR;
    }
    
    // Compile each statement in the function body
    for (const auto& stmt : func.body->statements) {
        if (compile_statement(*stmt, chunk) != CompileResult::OK) {
            return CompileResult::ERROR;
        }
    }
    
    return CompileResult::OK;
}

CompileResult Compiler::compile_statement(const Statement& stmt, Chunk& chunk) {
    switch (stmt.type) {
        case ASTNodeType::RETURN_STATEMENT: {
            const auto& return_stmt = static_cast<const ReturnStatement&>(stmt);
            debug_print("Compiling return statement");
            
            // Compile the return expression (if any)
            if (return_stmt.expression) {
                if (compile_expression(*return_stmt.expression, chunk) != CompileResult::OK) {
                    return CompileResult::ERROR;
                }
            } else {
                // Return void - push nil
                size_t nil_idx = chunk.add_constant(Value());
                chunk.write_opcode(OpCode::OP_CONSTANT);
                chunk.write_byte(static_cast<uint8_t>(nil_idx));
            }
            
            // Emit return instruction
            chunk.write_opcode(OpCode::OP_RETURN);
            return CompileResult::OK;
        }
        
        default:
            compile_error("Unsupported statement type");
            return CompileResult::ERROR;
    }
}

CompileResult Compiler::compile_expression(const Expression& expr, Chunk& chunk) {
    switch (expr.type) {
        case ASTNodeType::INTEGER_LITERAL: {
            const auto& int_literal = static_cast<const IntegerLiteral&>(expr);
            debug_print("Compiling integer literal: " + int_literal.value);
            
            // Parse the integer value
            try {
                int32_t value = std::stoi(int_literal.value);
                size_t const_idx = chunk.add_constant(Value(value));
                
                // Check if constant index fits in a byte
                if (const_idx > 255) {
                    compile_error("Too many constants");
                    return CompileResult::ERROR;
                }
                
                chunk.write_opcode(OpCode::OP_CONSTANT);
                chunk.write_byte(static_cast<uint8_t>(const_idx));
                return CompileResult::OK;
            } catch (const std::exception& e) {
                compile_error("Invalid integer literal: " + int_literal.value);
                return CompileResult::ERROR;
            }
        }
        
        default:
            compile_error("Unsupported expression type");
            return CompileResult::ERROR;
    }
}

void Compiler::compile_error(const std::string& message) {
    error_message_ = message;
    if (config_.debug_mode) {
        std::cerr << "[Compiler] Error: " << message << std::endl;
    }
}

void Compiler::debug_print(const std::string& message) const {
    if (config_.debug_mode) {
        std::cout << "[Compiler] " << message << std::endl;
    }
}

} // namespace dacite