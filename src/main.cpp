#include <iostream>
#include <fstream>
#include "lexer.h"

int main(int argc, char* argv[]) {
    // Default test source code if no file is provided
    std::string source = R"(
// This is a comment
/*
This
is
a
multiline
comment
*/

package main;

fn main() void {
    print("Hello, world!");
    
    return 0;
}
)";

    // If a file is provided as argument, read it
    if (argc > 1) {
        std::ifstream file(argv[1]);
        if (file.is_open()) {
            source = std::string((std::istreambuf_iterator<char>(file)),
                                 std::istreambuf_iterator<char>());
            file.close();
            std::cout << "Lexing file: " << argv[1] << std::endl;
        } else {
            std::cerr << "Error: Could not open file " << argv[1] << std::endl;
            return 1;
        }
    } else {
        std::cout << "Lexing default source code:" << std::endl;
    }

    std::cout << "Source:" << std::endl;
    std::cout << source << std::endl;
    std::cout << std::endl;

    // Create lexer with debug mode enabled
    dacite::LexerConfig config;
    config.debug_mode = true;
    config.emit_comments = true;

    dacite::Lexer lexer(source, config);

    std::cout << "Tokens:" << std::endl;
    
    // Tokenize and display all tokens
    auto tokens = lexer.tokenize_all();
    
    if (lexer.has_errors()) {
        std::cout << "\nErrors:" << std::endl;
        for (const auto& error : lexer.get_errors()) {
            std::cout << "Error at line " << error.span.start.line 
                      << ", column " << error.span.start.column 
                      << ": " << error.message << std::endl;
        }
    }

    // Dump token information
    std::cout << "\nToken dump:" << std::endl;
    std::cout << lexer.dump_tokens() << std::endl;

    return 0;
}