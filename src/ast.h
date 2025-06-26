#pragma once

#include <memory>
#include <vector>
#include <string>
#include "source_span.h"

namespace dacite {

// Forward declarations
class ASTNode;
class Expression;
class Statement;
class Declaration;

using ASTNodePtr = std::unique_ptr<ASTNode>;
using ExpressionPtr = std::unique_ptr<Expression>;
using StatementPtr = std::unique_ptr<Statement>;
using DeclarationPtr = std::unique_ptr<Declaration>;

/// AST node types for visitor pattern
enum class ASTNodeType {
    PROGRAM,
    PACKAGE_DECLARATION,
    FUNCTION_DECLARATION,
    RETURN_STATEMENT,
    BLOCK_STATEMENT,
    INTEGER_LITERAL,
    TYPE
};

/// Base class for all AST nodes
class ASTNode {
public:
    ASTNodeType type;
    SourceSpan span;

    ASTNode(ASTNodeType type, const SourceSpan& span)
        : type(type), span(span) {}

    virtual ~ASTNode() = default;
    
    // For debugging
    virtual std::string to_string() const = 0;
};

/// Base class for expressions
class Expression : public ASTNode {
public:
    Expression(ASTNodeType type, const SourceSpan& span)
        : ASTNode(type, span) {}
};

/// Base class for statements
class Statement : public ASTNode {
public:
    Statement(ASTNodeType type, const SourceSpan& span)
        : ASTNode(type, span) {}
};

/// Base class for declarations
class Declaration : public ASTNode {
public:
    Declaration(ASTNodeType type, const SourceSpan& span)
        : ASTNode(type, span) {}
};

/// Represents a type in the type system
class Type : public ASTNode {
public:
    std::string name;

    Type(const std::string& name, const SourceSpan& span)
        : ASTNode(ASTNodeType::TYPE, span), name(name) {}

    std::string to_string() const override {
        return "Type(" + name + ")";
    }
};

/// Integer literal expression
class IntegerLiteral : public Expression {
public:
    std::string value;

    IntegerLiteral(const std::string& value, const SourceSpan& span)
        : Expression(ASTNodeType::INTEGER_LITERAL, span), value(value) {}

    std::string to_string() const override {
        return "IntegerLiteral(" + value + ")";
    }
};

/// Package declaration
class PackageDeclaration : public Declaration {
public:
    std::string package_name;

    PackageDeclaration(const std::string& package_name, const SourceSpan& span)
        : Declaration(ASTNodeType::PACKAGE_DECLARATION, span), package_name(package_name) {}

    std::string to_string() const override {
        return "PackageDeclaration(" + package_name + ")";
    }
};

/// Block statement containing a list of statements
class BlockStatement : public Statement {
public:
    std::vector<StatementPtr> statements;

    BlockStatement(const SourceSpan& span)
        : Statement(ASTNodeType::BLOCK_STATEMENT, span) {}

    void add_statement(StatementPtr statement) {
        statements.push_back(std::move(statement));
    }

    std::string to_string() const override {
        std::string result = "BlockStatement([";
        for (size_t i = 0; i < statements.size(); ++i) {
            if (i > 0) result += ", ";
            result += statements[i]->to_string();
        }
        result += "])";
        return result;
    }
};

/// Return statement
class ReturnStatement : public Statement {
public:
    ExpressionPtr expression;  // Can be null for bare return

    ReturnStatement(ExpressionPtr expression, const SourceSpan& span)
        : Statement(ASTNodeType::RETURN_STATEMENT, span), expression(std::move(expression)) {}

    std::string to_string() const override {
        std::string result = "ReturnStatement(";
        if (expression) {
            result += expression->to_string();
        } else {
            result += "void";
        }
        result += ")";
        return result;
    }
};

/// Function declaration
class FunctionDeclaration : public Declaration {
public:
    std::string function_name;
    std::vector<std::string> parameters;  // Simplified for now
    std::unique_ptr<Type> return_type;
    std::unique_ptr<BlockStatement> body;

    FunctionDeclaration(const std::string& function_name, 
                       std::unique_ptr<Type> return_type,
                       std::unique_ptr<BlockStatement> body,
                       const SourceSpan& span)
        : Declaration(ASTNodeType::FUNCTION_DECLARATION, span)
        , function_name(function_name)
        , return_type(std::move(return_type))
        , body(std::move(body)) {}

    std::string to_string() const override {
        std::string result = "FunctionDeclaration(" + function_name + ", ";
        if (return_type) {
            result += return_type->to_string();
        } else {
            result += "void";
        }
        result += ", ";
        if (body) {
            result += body->to_string();
        } else {
            result += "null";
        }
        result += ")";
        return result;
    }
};

/// Program root node
class Program : public ASTNode {
public:
    std::unique_ptr<PackageDeclaration> package_declaration;
    std::vector<DeclarationPtr> declarations;

    Program(const SourceSpan& span)
        : ASTNode(ASTNodeType::PROGRAM, span) {}

    void set_package_declaration(std::unique_ptr<PackageDeclaration> package_decl) {
        package_declaration = std::move(package_decl);
    }

    void add_declaration(DeclarationPtr declaration) {
        declarations.push_back(std::move(declaration));
    }

    std::string to_string() const override {
        std::string result = "Program(";
        if (package_declaration) {
            result += package_declaration->to_string();
        } else {
            result += "no_package";
        }
        result += ", [";
        for (size_t i = 0; i < declarations.size(); ++i) {
            if (i > 0) result += ", ";
            result += declarations[i]->to_string();
        }
        result += "])";
        return result;
    }
};

} // namespace dacite