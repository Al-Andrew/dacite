# Dacite Lexer Documentation

## Overview

The Dacite Lexer is a comprehensive tokenization component for the dacite programming language. It converts source code text into a stream of tokens that can be consumed by a parser.

## Features

### Supported Token Types

#### Keywords
- `package` - Package declaration
- `fn` - Function definition
- `void` - Void type
- `return` - Return statement
- `if`, `else` - Conditional statements
- `while`, `for` - Loop constructs
- `true`, `false` - Boolean literals

#### Literals
- **Integer literals**: Decimal (123), hexadecimal (0xFF), binary (0b1010), octal (0755)
- **Float literals**: Decimal floating-point numbers (3.14, 0.5)
- **String literals**: Text enclosed in double quotes with escape sequence support
- **Character literals**: Single characters enclosed in single quotes with escape sequences
- **Boolean literals**: `true` and `false`

#### Operators
- **Arithmetic**: `+`, `-`, `*`, `/`, `%`
- **Assignment**: `=`, `+=`, `-=`, `*=`, `/=`, `%=`
- **Comparison**: `==`, `!=`, `<`, `<=`, `>`, `>=`
- **Logical**: `&&`, `||`, `!`
- **Arrow**: `->` (for function types)

#### Punctuation
- **Delimiters**: `;`, `,`, `.`, `:`
- **Parentheses**: `(`, `)`
- **Braces**: `{`, `}`
- **Brackets**: `[`, `]`

#### Comments
- **Single-line**: `// comment text`
- **Multi-line**: `/* comment text */`

### Escape Sequences
The lexer supports the following escape sequences in string and character literals:
- `\n` - Newline
- `\t` - Tab
- `\r` - Carriage return
- `\\` - Backslash
- `\"` - Double quote
- `\'` - Single quote
- `\0` - Null character

## API Reference

### LexerConfig

Configuration options for the lexer:

```cpp
struct LexerConfig {
    bool emit_comments = false;      // Include comment tokens in output
    bool emit_whitespace = false;    // Include whitespace tokens in output
    bool debug_mode = false;         // Print tokens as they are lexed
    bool verbose_mode = false;       // Include extra debug information
};
```

### Lexer Class

Main lexer class for tokenizing source code:

```cpp
class Lexer {
public:
    // Constructor
    explicit Lexer(std::string_view source, const LexerConfig& config = {});
    
    // Token retrieval
    Token next_token();              // Get next token
    Token peek_token();              // Peek at next token without consuming
    bool at_end() const;             // Check if at end of input
    
    // Batch operations
    std::vector<Token> tokenize_all();  // Get all tokens
    std::string dump_tokens();          // Debug output of all tokens
    
    // Error handling
    const std::vector<LexerError>& get_errors() const;
    bool has_errors() const;
};
```

### Token Structure

Each token contains:

```cpp
struct Token {
    TokenType type;        // Type of the token
    std::string value;     // String value (for literals, identifiers, etc.)
    SourceSpan span;       // Source location information
};
```

### Source Position Tracking

The lexer tracks source positions for debugging and error reporting:

```cpp
struct SourcePosition {
    size_t line;    // Line number (1-based)
    size_t column;  // Column number (1-based)
    size_t offset;  // Character offset (0-based)
};

struct SourceSpan {
    SourcePosition start;  // Start position
    SourcePosition end;    // End position
};
```

## Usage Examples

### Basic Usage

```cpp
#include "lexer.h"

std::string source = R"(
package main;

fn main() void {
    print("Hello, world!");
    return 0;
}
)";

dacite::Lexer lexer(source);

while (!lexer.at_end()) {
    auto token = lexer.next_token();
    if (token.type == dacite::TokenType::EOF_TOKEN) break;
    
    std::cout << dacite::token_type_to_string(token.type);
    if (!token.value.empty()) {
        std::cout << "(" << token.value << ")";
    }
    std::cout << std::endl;
}
```

### Debug Mode

```cpp
dacite::LexerConfig config;
config.debug_mode = true;
config.emit_comments = true;

dacite::Lexer lexer(source, config);
auto tokens = lexer.tokenize_all();
```

### Error Handling

```cpp
dacite::Lexer lexer("\"unterminated string");

auto token = lexer.next_token();
if (lexer.has_errors()) {
    for (const auto& error : lexer.get_errors()) {
        std::cout << "Error at line " << error.span.start.line 
                  << ": " << error.message << std::endl;
    }
}
```

### Token Stream Dump

```cpp
dacite::Lexer lexer(source);
std::cout << lexer.dump_tokens() << std::endl;
```

## Testing

The lexer includes comprehensive unit tests covering:
- All token types
- Error conditions
- Source position tracking
- Configuration options
- Edge cases

Run tests with:
```bash
cmake --build --preset default
./.bin/lexer_test
```

## Error Handling

The lexer provides detailed error messages for:
- Unterminated string literals
- Unterminated multi-line comments
- Invalid escape sequences
- Unexpected characters
- Empty character literals

All errors include precise source location information for debugging.