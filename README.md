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
│   ├── token.h    # Token definitions
│   ├── token.cpp  # Token utilities
│   └── source_span.h # Source position tracking
├── tests/         # Test files
│   ├── lexer_test.cpp # Unit tests
│   └── test_*.dt      # Test source files
├── docs/          # Documentation
│   └── lexer.md   # Lexer documentation
└── examples/      # Example programs
    └── overview.dt # Language overview
```