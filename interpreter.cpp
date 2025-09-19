#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <cctype>
#include <stdexcept>
#include <sstream>

#ifdef __EMSCRIPTEN__
#include <emscripten/bind.h>
#include <emscripten.h>
#endif

// Forward declarations
class Value;
class Environment;

// VALUE SYSTEM 
class Value {
public:
    enum Type { NUMBER, STRING, BOOLEAN, NIL };
    
    //Store types and values 
    Value() : type_(NIL) {}
    Value(double num) : type_(NUMBER), number_(num) {}
    Value(const std::string& str) : type_(STRING), string_(str) {}
    Value(bool b) : type_(BOOLEAN), boolean_(b) {}
    

    Type getType() const { return type_; }
    double asNumber() const { return number_; }
    std::string asString() const { return string_; }
    bool asBoolean() const { return boolean_; }
    
    std::string toString() const {
        switch (type_) {
            case NUMBER: {
                // Format numbers nicely (remove trailing zeros)
                std::string result = std::to_string(number_);
                result.erase(result.find_last_not_of('0') + 1, std::string::npos);
                result.erase(result.find_last_not_of('.') + 1, std::string::npos);
                return result;
            }
            case STRING: return string_;
            case BOOLEAN: return boolean_ ? "true" : "false";
            case NIL: return "nil";
        }
        return "nil";
    }

private:
    Type type_;
    double number_ = 0;
    std::string string_;
    bool boolean_ = false;
};

// Tokenizer
enum TokenType {
    NUMBER, STRING, IDENTIFIER,
    TRUE, FALSE, NIL, LET,
    PLUS, MINUS, MULTIPLY, DIVIDE,
    ASSIGN,
    LEFT_PAREN, RIGHT_PAREN, LEFT_BRACE, RIGHT_BRACE,
    SEMICOLON,
    EOF_TOKEN, NEWLINE
};

struct Token {
    TokenType type;
    std::string lexeme; //Raw text
    Value literal; //Typed runtime repr.
    int line;
};

class Lexer {
private:
    std::string source_;
    std::vector<Token> tokens_;
    int start_ = 0;
    int current_ = 0;
    int line_ = 1;
    
    bool isAtEnd() { return current_ >= source_.length(); }
    char advance() { return source_[current_++]; }
    bool match(char expected) {
        if (isAtEnd()) return false;
        if (source_[current_] != expected) return false;
        current_++;
        return true;
    }
    char peek() { return isAtEnd() ? '\0' : source_[current_]; } //look for current token
    
    void addToken(TokenType type, Value literal = Value()) { //default is NIL
        std::string text = source_.substr(start_, current_ - start_);
        tokens_.push_back({type, text, literal, line_});
    }
    
    void string() {
        while (peek() != '"' && !isAtEnd()) {
            if (peek() == '\n') line_++;
            advance();
        }
        if (isAtEnd()) throw std::runtime_error("Unterminated string");
        advance();
        std::string value = source_.substr(start_ + 1, current_ - start_ - 2);
        addToken(STRING, Value(value));
    }
    
    void number() {
        while (std::isdigit(peek())) advance();
        if (peek() == '.' && std::isdigit(source_[current_ + 1])) {
            advance();
            while (std::isdigit(peek())) advance();
        }
        double value = std::stod(source_.substr(start_, current_ - start_));
        addToken(NUMBER, Value(value));
    }
    
    void identifier() {
        while (std::isalnum(peek()) || peek() == '_') advance();
        std::string text = source_.substr(start_, current_ - start_);
        TokenType type = IDENTIFIER;
        
        if (text == "true") type = TRUE;
        else if (text == "false") type = FALSE;
        else if (text == "nil") type = NIL;
        else if (text == "let") type = LET;
        
        addToken(type);
    }

public:
    std::vector<Token> scanTokens(const std::string& source) {
        source_ = source;
        tokens_.clear();
        start_ = 0;
        current_ = 0;
        line_ = 1;
        
        while (!isAtEnd()) {
            start_ = current_;
            scanToken();
        }
        
        tokens_.push_back({EOF_TOKEN, "", Value(), line_});
        return tokens_;
    }
    
    void scanToken() {
        char c = advance();
        switch (c) {
            case '(': addToken(LEFT_PAREN); break;
            case ')': addToken(RIGHT_PAREN); break;
            case '{': addToken(LEFT_BRACE); break;
            case '}': addToken(RIGHT_BRACE); break;
            case ';': addToken(SEMICOLON); break;
            case '+': addToken(PLUS); break;
            case '-': addToken(MINUS); break;
            case '*': addToken(MULTIPLY); break;
            case '/': addToken(DIVIDE); break;
            case '=': addToken(match('=') ? ASSIGN : ASSIGN); break;
            case ' ': case '\r': case '\t': break;
            case '\n': line_++; addToken(NEWLINE); break;
            case '"': string(); break;
            default:
                if (std::isdigit(c)) number();
                else if (std::isalpha(c) || c == '_') identifier();
                else throw std::runtime_error("Unexpected character: " + std::string(1, c));
                break;
        }
    }
};

// AST
class Expr {
public:
    virtual ~Expr() = default;
    virtual Value evaluate(Environment& env) = 0;
};

class LiteralExpr : public Expr {
    Value value_;
public:
    LiteralExpr(Value value) : value_(value) {}
    Value evaluate(Environment& env) override { return value_; }
};

class VariableExpr : public Expr {
public:
    std::string name_;
    VariableExpr(const std::string& name) : name_(name) {}
    Value evaluate(Environment& env) override;
};

class BinaryExpr : public Expr {
    std::unique_ptr<Expr> left_;
    Token operator_;
    std::unique_ptr<Expr> right_;
public:
    BinaryExpr(std::unique_ptr<Expr> left, Token op, std::unique_ptr<Expr> right)
        : left_(std::move(left)), operator_(op), right_(std::move(right)) {}
    Value evaluate(Environment& env) override;
};

class AssignExpr : public Expr {
    std::string name_;
    std::unique_ptr<Expr> value_;
public:
    AssignExpr(const std::string& name, std::unique_ptr<Expr> value)
        : name_(name), value_(std::move(value)) {}
    Value evaluate(Environment& env) override;
};

class Stmt {
public:
    virtual ~Stmt() = default;
    virtual void execute(Environment& env) = 0;
};

class ExpressionStmt : public Stmt {
    std::unique_ptr<Expr> expression_;
public:
    ExpressionStmt(std::unique_ptr<Expr> expr) : expression_(std::move(expr)) {}
    void execute(Environment& env) override { expression_->evaluate(env); }
};

class PrintStmt : public Stmt {
    std::unique_ptr<Expr> expression_;
    std::string* output_;
public:
    PrintStmt(std::unique_ptr<Expr> expr, std::string* output = nullptr) 
        : expression_(std::move(expr)), output_(output) {}
    void execute(Environment& env) override {
        Value result = expression_->evaluate(env);
        if (output_) {
            *output_ += result.toString() + "\n";
        } else {
            std::cout << result.toString() << std::endl;
        }
    }
};

class LetStmt : public Stmt {
    std::string name_;
    std::unique_ptr<Expr> initializer_;
public:
    LetStmt(const std::string& name, std::unique_ptr<Expr> init)
        : name_(name), initializer_(std::move(init)) {}
    void execute(Environment& env) override;
};

class BlockStmt : public Stmt {
    std::vector<std::unique_ptr<Stmt>> statements_;
public:
    BlockStmt(std::vector<std::unique_ptr<Stmt>> statements)
        : statements_(std::move(statements)) {}
    void execute(Environment& env) override;
};

// ENV
class Environment {
private:
    std::map<std::string, Value> values_;
    Environment* enclosing_;
    
public:
    Environment() : enclosing_(nullptr) {}
    Environment(Environment* enclosing) : enclosing_(enclosing) {}
    
    void define(const std::string& name, Value value) {
        values_[name] = value;
    }
    
    Value get(const std::string& name) {
        auto it = values_.find(name);
        if (it != values_.end()) return it->second;
        if (enclosing_) return enclosing_->get(name);
        throw std::runtime_error("Undefined variable '" + name + "'");
    }
    
    void assign(const std::string& name, Value value) {
        auto it = values_.find(name);
        if (it != values_.end()) {
            it->second = value;
            return;
        }
        if (enclosing_) {
            enclosing_->assign(name, value);
            return;
        }
        throw std::runtime_error("Undefined variable '" + name + "'");
    }
};

// ==== IMPLEMENTATIONS ====
Value VariableExpr::evaluate(Environment& env) {
    return env.get(name_);
}

Value BinaryExpr::evaluate(Environment& env) {
    Value left = left_->evaluate(env);
    Value right = right_->evaluate(env);
    
    switch (operator_.type) {
        case PLUS:
            if (left.getType() == Value::NUMBER && right.getType() == Value::NUMBER) {
                return Value(left.asNumber() + right.asNumber());
            }
            if (left.getType() == Value::STRING || right.getType() == Value::STRING) {
                return Value(left.toString() + right.toString());
            }
            break;
        case MINUS: return Value(left.asNumber() - right.asNumber());
        case MULTIPLY: return Value(left.asNumber() * right.asNumber());
        case DIVIDE: return Value(left.asNumber() / right.asNumber());
    }
    throw std::runtime_error("Invalid binary operation");
}

Value AssignExpr::evaluate(Environment& env) {
    Value value = value_->evaluate(env);
    env.assign(name_, value);
    return value;
}

void LetStmt::execute(Environment& env) {
    Value value;
    if (initializer_) {
        value = initializer_->evaluate(env);
    }
    env.define(name_, value);
}

void BlockStmt::execute(Environment& env) {
    Environment blockEnv(&env);
    for (auto& stmt : statements_) {
        stmt->execute(blockEnv);
    }
}

// Parser
class Parser {
private:
    std::vector<Token> tokens_;
    int current_ = 0;
    std::string* output_;
    
    bool isAtEnd() { return peek().type == EOF_TOKEN; }
    Token peek() { return tokens_[current_]; }
    Token previous() { return tokens_[current_ - 1]; }
    bool check(TokenType type) { return !isAtEnd() && peek().type == type; }
    Token advance() { if (!isAtEnd()) current_++; return previous(); }
    
    bool match(std::vector<TokenType> types) {
        for (TokenType type : types) {
            if (check(type)) {
                advance();
                return true;
            }
        }
        return false;
    }
    
    Token consume(TokenType type, const std::string& message) {
        if (check(type)) return advance();
        throw std::runtime_error(message);
    }
    
    void skipNewlines() { while (match({NEWLINE})) {} }

public:
    std::vector<std::unique_ptr<Stmt>> parse(const std::vector<Token>& tokens, std::string* output = nullptr) {
        tokens_ = tokens;
        current_ = 0;
        output_ = output;
        
        std::vector<std::unique_ptr<Stmt>> statements;
        while (!isAtEnd()) {
            skipNewlines();
            if (!isAtEnd()) {
                statements.push_back(statement());
            }
        }
        return statements;
    }
    
    std::unique_ptr<Stmt> statement() {
        if (match({LET})) return letStatement();
        if (match({LEFT_BRACE})) return blockStatement();
        
        if (check(IDENTIFIER) && peek().lexeme == "print") {
            advance();
            consume(LEFT_PAREN, "Expected '(' after 'print'");
            auto expr = expression();
            consume(RIGHT_PAREN, "Expected ')' after expression");
            match({SEMICOLON, NEWLINE});
            return std::make_unique<PrintStmt>(std::move(expr), output_);
        }
        
        return expressionStatement();
    }
    
    std::unique_ptr<Stmt> letStatement() {
        Token name = consume(IDENTIFIER, "Expected variable name");
        std::unique_ptr<Expr> initializer = nullptr;
        if (match({ASSIGN})) {
            initializer = expression();
        }
        match({SEMICOLON, NEWLINE});
        return std::make_unique<LetStmt>(name.lexeme, std::move(initializer));
    }
    
    std::unique_ptr<Stmt> blockStatement() {
        std::vector<std::unique_ptr<Stmt>> statements;
        while (!check(RIGHT_BRACE) && !isAtEnd()) {
            skipNewlines();
            if (!check(RIGHT_BRACE)) {
                statements.push_back(statement());
            }
        }
        consume(RIGHT_BRACE, "Expected '}' after block");
        return std::make_unique<BlockStmt>(std::move(statements));
    }
    
    std::unique_ptr<Stmt> expressionStatement() {
        auto expr = expression();
        match({SEMICOLON, NEWLINE});
        return std::make_unique<ExpressionStmt>(std::move(expr));
    }
    
    std::unique_ptr<Expr> expression() { return assignment(); }
    
    std::unique_ptr<Expr> assignment() {
        auto expr = term();
        if (match({ASSIGN})) {
            auto value = assignment();
            if (auto varExpr = dynamic_cast<VariableExpr*>(expr.get())) {
                std::string name = varExpr->name_;
                expr.release();
                return std::make_unique<AssignExpr>(name, std::move(value));
            }
            throw std::runtime_error("Invalid assignment target");
        }
        return expr;
    }
    
    std::unique_ptr<Expr> term() {
        auto expr = factor();
        while (match({MINUS, PLUS})) {
            Token op = previous();
            auto right = factor();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }
    
    std::unique_ptr<Expr> factor() {
        auto expr = primary();
        while (match({DIVIDE, MULTIPLY})) {
            Token op = previous();
            auto right = primary();
            expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
        }
        return expr;
    }
    
    std::unique_ptr<Expr> primary() {
        if (match({TRUE})) return std::make_unique<LiteralExpr>(Value(true));
        if (match({FALSE})) return std::make_unique<LiteralExpr>(Value(false));
        if (match({NIL})) return std::make_unique<LiteralExpr>(Value());
        if (match({NUMBER, STRING})) {
            return std::make_unique<LiteralExpr>(previous().literal);
        }
        if (match({IDENTIFIER})) {
            return std::make_unique<VariableExpr>(previous().lexeme);
        }
        if (match({LEFT_PAREN})) {
            auto expr = expression();
            consume(RIGHT_PAREN, "Expected ')' after expression");
            return expr;
        }
        throw std::runtime_error("Expected expression");
    }
};

// Interpreter 
class Interpreter {
private:
    Environment global_;
    Lexer lexer_;
    Parser parser_;
    
public:
    std::string interpret(const std::string& source) {
        std::string output;
        try {
            auto tokens = lexer_.scanTokens(source);
            auto statements = parser_.parse(tokens, &output);
            for (auto& stmt : statements) {
                stmt->execute(global_);
            }
        } catch (const std::exception& e) {
            output += "Error: " + std::string(e.what()) + "\n";
        }
        return output;
    }
    
    void reset() {
        global_ = Environment();
    }
};

// Global interpreter instance for WASM
static Interpreter* globalInterpreter = nullptr;

std::string runCode(const std::string& code) {
    if (!globalInterpreter) {
        globalInterpreter = new Interpreter();
    }
    return globalInterpreter->interpret(code);
}

void resetInterpreter() {
    if (globalInterpreter) {
        globalInterpreter->reset();
    }
}

#ifdef __EMSCRIPTEN__
// Emscripten bindings
EMSCRIPTEN_BINDINGS(interpreter) {
    emscripten::function("runCode", &runCode);
    emscripten::function("resetInterpreter", &resetInterpreter);
}
#endif

// Main function for native compilation
#ifndef __EMSCRIPTEN__
int main() {
    Interpreter interpreter;
    std::cout << "Simple Interpreter - Enter your code (type 'exit' to quit):\n";
    
    std::string line;
    while (std::getline(std::cin, line)) {
        if (line == "exit") break;
        if (!line.empty()) {
            std::cout << interpreter.interpret(line);
        }
    }
    return 0;
}
#endif

