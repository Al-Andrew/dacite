#pragma once

#include <memory>
#include "ast.h"
#include "chunk.h"
#include "value.h"

namespace dacite {

/// Result of compilation
enum class CompileResult {
    OK,
    ERROR
};

/// Configuration for the compiler
struct CompilerConfig {
    bool debug_mode = false;
};

/// Compiler that converts AST to bytecode
class Compiler {
public:
    /// Constructor with optional configuration
    explicit Compiler(const CompilerConfig& config = {});
    
    /// Compile a program AST to bytecode
    CompileResult compile(const Program& program, Chunk& chunk);
    
    /// Get any compilation error message
    const std::string& get_error_message() const { return error_message_; }
    
    /// Check if compilation had errors
    bool has_errors() const { return !error_message_.empty(); }

private:
    CompilerConfig config_;
    std::string error_message_;
    
    // Compilation methods for different AST nodes
    CompileResult compile_function(const FunctionDeclaration& func, Chunk& chunk);
    CompileResult compile_statement(const Statement& stmt, Chunk& chunk);
    CompileResult compile_expression(const Expression& expr, Chunk& chunk);
    
    // Error handling
    void compile_error(const std::string& message);
    
    // Debug output
    void debug_print(const std::string& message) const;
};

} // namespace dacite