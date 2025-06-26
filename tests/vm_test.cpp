#include <iostream>
#include <cassert>
#include <string>
#include "../src/value.h"
#include "../src/chunk.h"
#include "../src/vm.h"
#include "../src/compiler.h"
#include "../src/parser.h"
#include "../src/lexer.h"

// Simple test framework (consistent with existing tests)
#define TEST(name) void test_##name()
#define ASSERT_EQ(a, b) assert((a) == (b))
#define ASSERT_TRUE(a) assert(a)
#define ASSERT_FALSE(a) assert(!(a))
#define ASSERT_NOT_NULL(a) assert((a) != nullptr)
#define RUN_TEST(name) do { \
    std::cout << "Running " #name "... "; \
    test_##name(); \
    std::cout << "PASSED" << std::endl; \
} while(0)

using namespace dacite;

// === Value Tests ===

TEST(value_nil) {
    Value nil_value;
    ASSERT_TRUE(nil_value.is_nil());
    ASSERT_FALSE(nil_value.is_integer());
    ASSERT_EQ(nil_value.get_type(), ValueType::NIL);
    ASSERT_EQ(nil_value.to_string(), "nil");
}

TEST(value_integer) {
    Value int_value(42);
    ASSERT_FALSE(int_value.is_nil());
    ASSERT_TRUE(int_value.is_integer());
    ASSERT_EQ(int_value.get_type(), ValueType::INTEGER);
    ASSERT_EQ(int_value.as_integer(), 42);
    ASSERT_EQ(int_value.to_string(), "42");
}

TEST(value_equality) {
    Value nil1, nil2;
    Value int1(10), int2(10), int3(20);
    
    ASSERT_TRUE(nil1 == nil2);
    ASSERT_TRUE(int1 == int2);
    ASSERT_FALSE(int1 == int3);
    ASSERT_FALSE(nil1 == int1);
}

// === Chunk Tests ===

TEST(chunk_empty) {
    Chunk chunk;
    ASSERT_TRUE(chunk.empty());
    ASSERT_EQ(chunk.size(), 0);
    ASSERT_EQ(chunk.get_code().size(), 0);
    ASSERT_EQ(chunk.get_constants().size(), 0);
}

TEST(chunk_write_bytes) {
    Chunk chunk;
    chunk.write_byte(123);
    chunk.write_opcode(OpCode::OP_RETURN);
    
    ASSERT_FALSE(chunk.empty());
    ASSERT_EQ(chunk.size(), 2);
    ASSERT_EQ(chunk.get_code()[0], 123);
    ASSERT_EQ(chunk.get_code()[1], static_cast<uint8_t>(OpCode::OP_RETURN));
}

TEST(chunk_constants) {
    Chunk chunk;
    Value val1(42);
    Value val2(100);
    
    size_t idx1 = chunk.add_constant(val1);
    size_t idx2 = chunk.add_constant(val2);
    
    ASSERT_EQ(idx1, 0);
    ASSERT_EQ(idx2, 1);
    ASSERT_EQ(chunk.get_constants().size(), 2);
    ASSERT_EQ(chunk.get_constant(0).as_integer(), 42);
    ASSERT_EQ(chunk.get_constant(1).as_integer(), 100);
}

TEST(chunk_clear) {
    Chunk chunk;
    chunk.write_byte(42);
    chunk.add_constant(Value(10));
    
    ASSERT_FALSE(chunk.empty());
    chunk.clear();
    ASSERT_TRUE(chunk.empty());
    ASSERT_EQ(chunk.get_constants().size(), 0);
}

// === VM Tests ===

TEST(vm_empty_chunk) {
    VM vm;
    Chunk chunk;
    
    VMResult result = vm.run(chunk);
    ASSERT_EQ(result, VMResult::OK);
    ASSERT_TRUE(vm.is_stack_empty());
}

TEST(vm_constant_instruction) {
    VM vm;
    Chunk chunk;
    
    // Create chunk: OP_CONSTANT 0, where constant 0 = 42
    size_t const_idx = chunk.add_constant(Value(42));
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(static_cast<uint8_t>(const_idx));
    
    VMResult result = vm.run(chunk);
    ASSERT_EQ(result, VMResult::OK);
    ASSERT_FALSE(vm.is_stack_empty());
    ASSERT_EQ(vm.get_stack_size(), 1);
    ASSERT_EQ(vm.peek_stack_top().as_integer(), 42);
}

TEST(vm_return_instruction) {
    VM vm;
    Chunk chunk;
    
    // Create chunk: OP_CONSTANT 0, OP_RETURN (return the constant)
    size_t const_idx = chunk.add_constant(Value(3));
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(static_cast<uint8_t>(const_idx));
    chunk.write_opcode(OpCode::OP_RETURN);
    
    VMResult result = vm.run(chunk);
    ASSERT_EQ(result, VMResult::OK);
    ASSERT_FALSE(vm.is_stack_empty());
    ASSERT_EQ(vm.peek_stack_top().as_integer(), 3);
}

TEST(vm_multiple_constants) {
    VM vm;
    Chunk chunk;
    
    // Push multiple constants
    chunk.add_constant(Value(10));
    chunk.add_constant(Value(20));
    chunk.add_constant(Value(30));
    
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(0);
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(1);
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(2);
    
    VMResult result = vm.run(chunk);
    ASSERT_EQ(result, VMResult::OK);
    ASSERT_EQ(vm.get_stack_size(), 3);
}

TEST(vm_runtime_error_invalid_constant) {
    VM vm;
    Chunk chunk;
    
    // Try to load constant at invalid index
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(99); // Invalid index
    
    VMResult result = vm.run(chunk);
    ASSERT_EQ(result, VMResult::RUNTIME_ERROR);
    ASSERT_FALSE(vm.get_error_message().empty());
}

TEST(vm_runtime_error_return_empty_stack) {
    VM vm;
    Chunk chunk;
    
    // Try to return with empty stack
    chunk.write_opcode(OpCode::OP_RETURN);
    
    VMResult result = vm.run(chunk);
    ASSERT_EQ(result, VMResult::RUNTIME_ERROR);
    ASSERT_FALSE(vm.get_error_message().empty());
}

TEST(vm_reset) {
    VM vm;
    Chunk chunk;
    
    // Put something on the stack
    chunk.add_constant(Value(42));
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(0);
    
    vm.run(chunk);
    ASSERT_FALSE(vm.is_stack_empty());
    
    vm.reset();
    ASSERT_TRUE(vm.is_stack_empty());
    ASSERT_TRUE(vm.get_error_message().empty());
}

TEST(vm_debug_mode) {
    VMConfig config;
    config.debug_mode = true;
    VM vm(config);
    Chunk chunk;
    
    // Simple program that should produce debug output
    chunk.add_constant(Value(42));
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(0);
    chunk.write_opcode(OpCode::OP_RETURN);
    
    VMResult result = vm.run(chunk);
    ASSERT_EQ(result, VMResult::OK);
}

// === Integration Test ===

TEST(vm_basic_function_return) {
    // This simulates the bytecode for: fun main() i32 { return 3; }
    VM vm;
    Chunk chunk;
    
    // Create the constant 3
    size_t const_idx = chunk.add_constant(Value(3));
    
    // Generate bytecode: load constant 3, then return
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(static_cast<uint8_t>(const_idx));
    chunk.write_opcode(OpCode::OP_RETURN);
    
    VMResult result = vm.run(chunk);
    ASSERT_EQ(result, VMResult::OK);
    ASSERT_EQ(vm.peek_stack_top().as_integer(), 3);
}

// === Compiler Tests ===

// Helper function to tokenize and parse source code
std::unique_ptr<Program> parse_source(const std::string& source) {
    Lexer lexer(source);
    auto tokens = lexer.tokenize_all();
    if (lexer.has_errors()) {
        return nullptr;
    }
    
    Parser parser(std::move(tokens));
    auto program = parser.parse();
    if (parser.has_errors()) {
        return nullptr;
    }
    
    return program;
}

TEST(compiler_basic_function) {
    std::string source = "package main; fn main() i32 { return 3; }";
    auto program = parse_source(source);
    ASSERT_NOT_NULL(program);
    
    Compiler compiler;
    Chunk chunk;
    
    CompileResult result = compiler.compile(*program, chunk);
    ASSERT_EQ(result, CompileResult::OK);
    ASSERT_FALSE(compiler.has_errors());
    ASSERT_FALSE(chunk.empty());
    
    // Check the generated bytecode
    const auto& code = chunk.get_code();
    ASSERT_EQ(code.size(), 3); // OP_CONSTANT, index, OP_RETURN
    ASSERT_EQ(static_cast<OpCode>(code[0]), OpCode::OP_CONSTANT);
    ASSERT_EQ(code[1], 0); // constant index
    ASSERT_EQ(static_cast<OpCode>(code[2]), OpCode::OP_RETURN);
    
    // Check constants
    const auto& constants = chunk.get_constants();
    ASSERT_EQ(constants.size(), 1);
    ASSERT_EQ(constants[0].as_integer(), 3);
}

TEST(compiler_different_integer) {
    std::string source = "package main; fn main() i32 { return 42; }";
    auto program = parse_source(source);
    ASSERT_NOT_NULL(program);
    
    Compiler compiler;
    Chunk chunk;
    
    CompileResult result = compiler.compile(*program, chunk);
    ASSERT_EQ(result, CompileResult::OK);
    
    // Verify the constant value
    const auto& constants = chunk.get_constants();
    ASSERT_EQ(constants.size(), 1);
    ASSERT_EQ(constants[0].as_integer(), 42);
}

TEST(compiler_error_no_functions) {
    std::string source = "package main;";
    auto program = parse_source(source);
    ASSERT_NOT_NULL(program);
    
    Compiler compiler;
    Chunk chunk;
    
    CompileResult result = compiler.compile(*program, chunk);
    ASSERT_EQ(result, CompileResult::ERROR);
    ASSERT_TRUE(compiler.has_errors());
}

// === End-to-End Integration Test ===

TEST(end_to_end_basic_example) {
    std::cout << "\n=== End-to-End Test: package main; fn main() i32 { return 3; } ===" << std::endl;
    
    std::string source = "package main; fn main() i32 { return 3; }";
    
    // Step 1: Parse the source code
    std::cout << "1. Parsing source code..." << std::endl;
    auto program = parse_source(source);
    ASSERT_NOT_NULL(program);
    
    // Step 2: Compile AST to bytecode
    std::cout << "2. Compiling to bytecode..." << std::endl;
    CompilerConfig compiler_config;
    compiler_config.debug_mode = true;
    Compiler compiler(compiler_config);
    Chunk chunk;
    
    CompileResult compile_result = compiler.compile(*program, chunk);
    ASSERT_EQ(compile_result, CompileResult::OK);
    ASSERT_FALSE(compiler.has_errors());
    
    std::cout << "Generated bytecode:\n" << chunk.to_string() << std::endl;
    
    // Step 3: Execute bytecode in VM
    std::cout << "3. Executing in VM..." << std::endl;
    VMConfig vm_config;
    vm_config.debug_mode = true;
    VM vm(vm_config);
    
    VMResult vm_result = vm.run(chunk);
    ASSERT_EQ(vm_result, VMResult::OK);
    ASSERT_FALSE(vm.is_stack_empty());
    
    // Step 4: Verify result
    std::cout << "4. Verifying result..." << std::endl;
    Value result = vm.peek_stack_top();
    ASSERT_TRUE(result.is_integer());
    ASSERT_EQ(result.as_integer(), 3);
    
    std::cout << "✅ End-to-end test successful! Program returned: " << result.to_string() << std::endl;
}

// === Expression Tests ===

TEST(value_boolean) {
    // Test boolean value creation and operations
    Value true_val(true);
    Value false_val(false);
    
    ASSERT_TRUE(true_val.is_boolean());
    ASSERT_TRUE(false_val.is_boolean());
    
    ASSERT_TRUE(true_val.as_boolean());
    ASSERT_FALSE(false_val.as_boolean());
    
    ASSERT_EQ(true_val.to_string(), "true");
    ASSERT_EQ(false_val.to_string(), "false");
    
    // Test equality
    ASSERT_EQ(true_val, Value(true));
    ASSERT_EQ(false_val, Value(false));
    ASSERT_FALSE(true_val == false_val);
}

TEST(vm_arithmetic_add) {
    VM vm;
    Chunk chunk;
    
    // Create constants 5 and 3
    size_t const_idx1 = chunk.add_constant(Value(5));
    size_t const_idx2 = chunk.add_constant(Value(3));
    
    // Generate bytecode: load 5, load 3, add
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(static_cast<uint8_t>(const_idx1));
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(static_cast<uint8_t>(const_idx2));
    chunk.write_opcode(OpCode::OP_ADD);
    chunk.write_opcode(OpCode::OP_RETURN);
    
    VMResult result = vm.run(chunk);
    ASSERT_EQ(result, VMResult::OK);
    ASSERT_EQ(vm.peek_stack_top().as_integer(), 8);
}

TEST(vm_arithmetic_multiply) {
    VM vm;
    Chunk chunk;
    
    // Create constants 4 and 7
    size_t const_idx1 = chunk.add_constant(Value(4));
    size_t const_idx2 = chunk.add_constant(Value(7));
    
    // Generate bytecode: load 4, load 7, multiply
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(static_cast<uint8_t>(const_idx1));
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(static_cast<uint8_t>(const_idx2));
    chunk.write_opcode(OpCode::OP_MULTIPLY);
    chunk.write_opcode(OpCode::OP_RETURN);
    
    VMResult result = vm.run(chunk);
    ASSERT_EQ(result, VMResult::OK);
    ASSERT_EQ(vm.peek_stack_top().as_integer(), 28);
}

TEST(vm_comparison_equal) {
    VM vm;
    Chunk chunk;
    
    // Create constants 5 and 5
    size_t const_idx1 = chunk.add_constant(Value(5));
    size_t const_idx2 = chunk.add_constant(Value(5));
    
    // Generate bytecode: load 5, load 5, equal
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(static_cast<uint8_t>(const_idx1));
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(static_cast<uint8_t>(const_idx2));
    chunk.write_opcode(OpCode::OP_EQUAL);
    chunk.write_opcode(OpCode::OP_RETURN);
    
    VMResult result = vm.run(chunk);
    ASSERT_EQ(result, VMResult::OK);
    Value result_val = vm.peek_stack_top();
    ASSERT_TRUE(result_val.is_boolean());
    ASSERT_TRUE(result_val.as_boolean());
}

TEST(vm_comparison_less_than) {
    VM vm;
    Chunk chunk;
    
    // Create constants 3 and 7
    size_t const_idx1 = chunk.add_constant(Value(3));
    size_t const_idx2 = chunk.add_constant(Value(7));
    
    // Generate bytecode: load 3, load 7, less than
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(static_cast<uint8_t>(const_idx1));
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(static_cast<uint8_t>(const_idx2));
    chunk.write_opcode(OpCode::OP_LESS);
    chunk.write_opcode(OpCode::OP_RETURN);
    
    VMResult result = vm.run(chunk);
    ASSERT_EQ(result, VMResult::OK);
    Value result_val = vm.peek_stack_top();
    ASSERT_TRUE(result_val.is_boolean());
    ASSERT_TRUE(result_val.as_boolean());
}

TEST(vm_division_by_zero) {
    VM vm;
    Chunk chunk;
    
    // Create constants 5 and 0
    size_t const_idx1 = chunk.add_constant(Value(5));
    size_t const_idx2 = chunk.add_constant(Value(0));
    
    // Generate bytecode: load 5, load 0, divide
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(static_cast<uint8_t>(const_idx1));
    chunk.write_opcode(OpCode::OP_CONSTANT);
    chunk.write_byte(static_cast<uint8_t>(const_idx2));
    chunk.write_opcode(OpCode::OP_DIVIDE);
    
    VMResult result = vm.run(chunk);
    ASSERT_EQ(result, VMResult::RUNTIME_ERROR);
}

TEST(end_to_end_arithmetic_expression) {
    std::string source = "package main; fn main() i32 { return 2 + 3 * 4; }";
    auto program = parse_source(source);
    ASSERT_NOT_NULL(program);
    
    Compiler compiler;
    Chunk chunk;
    
    CompileResult result = compiler.compile(*program, chunk);
    ASSERT_EQ(result, CompileResult::OK);
    
    VM vm;
    VMResult vm_result = vm.run(chunk);
    ASSERT_EQ(vm_result, VMResult::OK);
    
    Value result_value = vm.peek_stack_top();
    ASSERT_TRUE(result_value.is_integer());
    ASSERT_EQ(result_value.as_integer(), 14); // 2 + (3 * 4) = 2 + 12 = 14
}

TEST(end_to_end_comparison_expression) {
    std::string source = "package main; fn main() i32 { return 5 > 3; }";
    auto program = parse_source(source);
    ASSERT_NOT_NULL(program);
    
    Compiler compiler;
    Chunk chunk;
    
    CompileResult result = compiler.compile(*program, chunk);
    ASSERT_EQ(result, CompileResult::OK);
    
    VM vm;
    VMResult vm_result = vm.run(chunk);
    ASSERT_EQ(vm_result, VMResult::OK);
    
    Value result_value = vm.peek_stack_top();
    ASSERT_TRUE(result_value.is_boolean());
    ASSERT_TRUE(result_value.as_boolean()); // 5 > 3 is true
}

int main() {
    std::cout << "Running VM Tests..." << std::endl;
    
    // Value tests
    RUN_TEST(value_nil);
    RUN_TEST(value_integer);
    RUN_TEST(value_boolean);
    RUN_TEST(value_equality);
    
    // Chunk tests
    RUN_TEST(chunk_empty);
    RUN_TEST(chunk_write_bytes);
    RUN_TEST(chunk_constants);
    RUN_TEST(chunk_clear);
    
    // VM tests
    RUN_TEST(vm_empty_chunk);
    RUN_TEST(vm_constant_instruction);
    RUN_TEST(vm_return_instruction);
    RUN_TEST(vm_multiple_constants);
    RUN_TEST(vm_runtime_error_invalid_constant);
    RUN_TEST(vm_runtime_error_return_empty_stack);
    RUN_TEST(vm_reset);
    RUN_TEST(vm_debug_mode);
    
    // Arithmetic and comparison VM tests
    RUN_TEST(vm_arithmetic_add);
    RUN_TEST(vm_arithmetic_multiply);
    RUN_TEST(vm_comparison_equal);
    RUN_TEST(vm_comparison_less_than);
    RUN_TEST(vm_division_by_zero);
    
    // Integration test
    RUN_TEST(vm_basic_function_return);
    
    // Compiler tests
    RUN_TEST(compiler_basic_function);
    RUN_TEST(compiler_different_integer);
    RUN_TEST(compiler_error_no_functions);
    
    // End-to-end integration tests
    RUN_TEST(end_to_end_basic_example);
    RUN_TEST(end_to_end_arithmetic_expression);
    RUN_TEST(end_to_end_comparison_expression);
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}