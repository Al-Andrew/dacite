#include <iostream>
#include <fstream>
#include "lexer.h"
#include "parser.h"

int main(int argc, char* argv[]) {
    // Default test source code if no file is provided
    std::string source = R"(package main;

fn main() i32 { return 5; })";

    // If a file is provided as argument, read it
    if (argc > 1) {
        std::ifstream file(argv[1]);
        if (file.is_open()) {
            source = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
            file.close();
            std::cout << "Processing file: " << argv[1] << std::endl;
        } else {
            std::cerr << "Error: Could not open file " << argv[1] << std::endl;
            return 1;
        }
    } else {
        std::cout << "Processing default source code:" << std::endl;
    }

    std::cout << "Source:" << std::endl;
    std::cout << source << std::endl;
    std::cout << std::endl;

    // Create lexer with debug mode enabled
    dacite::LexerConfig lexer_config;
    lexer_config.debug_mode = false;
    lexer_config.emit_comments = true;

    dacite::Lexer lexer(source, lexer_config);

    std::cout << "=== LEXER OUTPUT ===" << std::endl;
    
    // Tokenize
    auto tokens = lexer.tokenize_all();
    
    if (lexer.has_errors()) {
        std::cout << "Lexer Errors:" << std::endl;
        for (const auto& error : lexer.get_errors()) {
            std::cout << "Error at line " << error.span.start.line 
                      << ", column " << error.span.start.column 
                      << ": " << error.message << std::endl;
        }
        return 1;
    }

    std::cout << "Tokens:" << std::endl;
    for (const auto& token : tokens) {
        std::cout << "  [" << token.span.start.line << ":" << token.span.start.column << "] ";
        std::cout << dacite::token_type_to_string(token.type);
        if (!token.value.empty()) {
            std::cout << "(\"" << token.value << "\")";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    std::cout << "=== PARSER OUTPUT ===" << std::endl;

    // Create parser
    dacite::ParserConfig parser_config;
    parser_config.debug_mode = false; // Set to true for detailed parsing steps
    dacite::Parser parser(std::move(tokens), parser_config);

    // Parse
    auto program = parser.parse();

    if (parser.has_errors()) {
        std::cout << "Parser Errors:" << std::endl;
        for (const auto& error : parser.get_errors()) {
            std::cout << "Error at line " << error.span.start.line
                      << ", column " << error.span.start.column
                      << ": " << error.message << std::endl;
        }
        return 1;
    }

    std::cout << "Parsing successful!" << std::endl;

    // Print AST
    if (program) {
        std::cout << "AST:" << std::endl;
        std::cout << program->to_string() << std::endl;
    }

    return 0;
}