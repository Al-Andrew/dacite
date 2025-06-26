# Dacite Programming Language

Dacite is a modern programming language with a focus on simplicity and performance.

## Components

### Lexer ✅

The lexer tokenizes dacite source code into a stream of tokens. Key features:

- **Complete token support**: Keywords, identifiers, literals, operators, punctuation
- **Multiple number formats**: Decimal, hexadecimal, binary, octal
- **String/character literals**: With full escape sequence support
- **Comments**: Single-line (`//`) and multi-line (`/* */`)
- **Source position tracking**: Line, column, and offset information
- **Debug mode**: Token stream visualization and error reporting
- **Comprehensive testing**: Full test suite with edge cases

See [docs/lexer.md](docs/lexer.md) for detailed documentation.

### Parser ✅

The parser converts tokens into an Abstract Syntax Tree (AST). Key features:

- **Basic language constructs**: Package declarations, function declarations, return statements
- **Expression parsing**: Integer literals (extensible for more complex expressions)
- **Error reporting**: Detailed error messages with source location information
- **Debug mode**: Step-by-step parsing visualization
- **AST visualization**: String representation of parsed AST nodes
- **Comprehensive testing**: Full test suite covering parsing scenarios

Example usage:
```cpp
// Tokenize source code
dacite::Lexer lexer(source);
auto tokens = lexer.tokenize_all();

// Parse tokens into AST
dacite::Parser parser(std::move(tokens));
auto program = parser.parse();

// Use the AST
std::cout << program->to_string() << std::endl;
```

## Building

```bash
# Configure
cmake --preset default

# Build
cmake --build --preset default

# Run lexer demo
./.bin/dacite [optional-file.dt]

# Run tests
./.bin/lexer_test
./.bin/parser_test
```

## Testing

The project includes comprehensive unit tests and continuous integration:

- **Local testing**: Run `./.bin/lexer_test` after building
- **CI/CD**: GitHub Actions automatically tests debug and release builds on every push and pull request
- **Coverage**: Tests cover all lexer functionality including error cases and edge conditions

## Language Example

```dacite
// Single line comment
/*
Multi-line
comment
*/

package main;

fn main() void {
    print("Hello, world!");
    return 0;
}
```

## Project Structure

```
dacite/
├── src/           # Source code
│   ├── main.cpp   # Demo application
│   ├── lexer.h    # Lexer interface
│   ├── lexer.cpp  # Lexer implementation
│   ├── parser.h   # Parser interface
│   ├── parser.cpp # Parser implementation
│   ├── ast.h      # AST node definitions
│   ├── token.h    # Token definitions
│   ├── token.cpp  # Token utilities
│   └── source_span.h # Source position tracking
├── tests/         # Test files
│   ├── lexer_test.cpp  # Lexer unit tests
│   ├── parser_test.cpp # Parser unit tests
│   └── test_*.dt       # Test source files
├── docs/          # Documentation
│   └── lexer.md   # Lexer documentation
└── examples/      # Example programs
    └── overview.dt # Language overview
```