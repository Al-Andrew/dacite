#pragma once

#include <cstddef>

namespace dacite {

/// Represents a position in source code
struct SourcePosition {
    size_t line;
    size_t column;
    size_t offset;

    SourcePosition(size_t line = 1, size_t column = 1, size_t offset = 0)
        : line(line), column(column), offset(offset) {}

    bool operator==(const SourcePosition& other) const {
        return line == other.line && column == other.column && offset == other.offset;
    }
};

/// Represents a span of source code with start and end positions
struct SourceSpan {
    SourcePosition start;
    SourcePosition end;

    SourceSpan() = default;
    SourceSpan(const SourcePosition& start, const SourcePosition& end)
        : start(start), end(end) {}
    SourceSpan(const SourcePosition& position)
        : start(position), end(position) {}

    bool operator==(const SourceSpan& other) const {
        return start == other.start && end == other.end;
    }
};

} // namespace dacite