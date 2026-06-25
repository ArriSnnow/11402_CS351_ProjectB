#include "query_parser.h"
#include <stdexcept>
#include <algorithm>
#include <cctype>

namespace {

// Uppercase copy, for case-insensitive keyword matching.
std::string upper(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return r;
}

bool isKeyword(const std::string& tok, const char* kw) {
    return upper(tok) == kw;
}

// Turn a literal token into a typed Value.
// 'quoted' or "quoted" => string; true/false => bool; int/double => numeric;
// anything else => string (bare word).
Value literalToValue(const std::string& tok) {
    if (tok.size() >= 2 &&
        ((tok.front() == '\'' && tok.back() == '\'') ||
         (tok.front() == '"'  && tok.back() == '"'))) {
        return tok.substr(1, tok.size() - 2);
    }
    std::string u = upper(tok);
    if (u == "TRUE")  return true;
    if (u == "FALSE") return false;

    try { size_t p; int i = std::stoi(tok, &p); if (p == tok.size()) return i; } catch (...) {}
    try { size_t p; double d = std::stod(tok, &p); if (p == tok.size()) return d; } catch (...) {}
    return tok; // bare word treated as string
}

} // namespace

std::vector<std::string> QueryParser::tokenize(const std::string& query) {
    std::vector<std::string> tokens;
    size_t i = 0, n = query.size();
    while (i < n) {
        char c = query[i];
        if (std::isspace(static_cast<unsigned char>(c))) { ++i; continue; }

        // Quoted string literal (single or double quotes), kept with its quotes.
        if (c == '\'' || c == '"') {
            char quote = c;
            std::string s(1, quote);
            ++i;
            while (i < n && query[i] != quote) s += query[i++];
            if (i >= n) throw std::runtime_error("Unterminated string literal");
            s += quote; ++i;
            tokens.push_back(s);
            continue;
        }

        // Multi-char and single-char operators.
        if (c == '<' || c == '>' || c == '!' || c == '=') {
            std::string op(1, c);
            if (i + 1 < n && query[i + 1] == '=') { op += '='; i += 2; }
            else ++i;
            if (op == "!") throw std::runtime_error("Invalid operator '!' (did you mean '!=' ?)");
            tokens.push_back(op);
            continue;
        }
        if (c == ',' || c == '(' || c == ')' || c == '*') {
            tokens.push_back(std::string(1, c));
            ++i;
            continue;
        }

        // Bare word: identifier, number, or keyword.
        std::string word;
        while (i < n) {
            char d = query[i];
            if (std::isspace(static_cast<unsigned char>(d)) ||
                d == ',' || d == '(' || d == ')' ||
                d == '<' || d == '>' || d == '!' || d == '=' ||
                d == '\'' || d == '"') break;
            word += d; ++i;
        }
        tokens.push_back(word);
    }
    return tokens;
}

QueryPlan QueryParser::parse(const std::string& query) {
    std::vector<std::string> tok = tokenize(query);
    if (tok.empty()) throw std::runtime_error("Empty query");

    QueryPlan plan;
    size_t pos = 0;
    auto peek = [&]() -> std::string { return pos < tok.size() ? tok[pos] : std::string(); };
    auto next = [&]() -> std::string {
        if (pos >= tok.size()) throw std::runtime_error("Unexpected end of query");
        return tok[pos++];
    };

    // --- SELECT ---
    if (!isKeyword(next(), "SELECT"))
        throw std::runtime_error("Query must start with SELECT");

    if (peek() == "*") {
        next();
        plan.selectAll = true;
    } else {
        while (true) {
            std::string t = next();
            std::string u = upper(t);
            if (u == "COUNT" || u == "SUM" || u == "AVG" || u == "MIN" || u == "MAX") {
                if (next() != "(") throw std::runtime_error("Expected '(' after " + u);
                std::string col = next();
                if (next() != ")") throw std::runtime_error("Expected ')' after column in " + u + "()");
                SelectItem item;
                item.func = (u == "COUNT") ? AggFunc::COUNT :
                            (u == "SUM")   ? AggFunc::SUM   :
                            (u == "AVG")   ? AggFunc::AVG   :
                            (u == "MIN")   ? AggFunc::MIN   : AggFunc::MAX;
                item.column = col;
                plan.items.push_back(item);
                plan.hasAggregate = true;
            } else {
                if (t == "(" || t == ")" || t == "*")
                    throw std::runtime_error("Unexpected token in SELECT list: " + t);
                SelectItem item;
                item.func = AggFunc::NONE;
                item.column = t;
                plan.items.push_back(item);
            }
            if (peek() == ",") { next(); continue; }
            break;
        }
        // Mixing aggregates and plain columns is not supported (no GROUP BY).
        bool anyPlain = false, anyAgg = false;
        for (auto& it : plan.items) {
            if (it.func == AggFunc::NONE) anyPlain = true; else anyAgg = true;
        }
        if (anyPlain && anyAgg)
            throw std::runtime_error("Cannot mix aggregate functions with plain columns");
    }

    // --- FROM ---
    if (!isKeyword(next(), "FROM"))
        throw std::runtime_error("Expected FROM clause");
    plan.table = next();
    if (plan.table.empty()) throw std::runtime_error("Expected table name after FROM");

    // --- optional WHERE ---
    if (isKeyword(peek(), "WHERE")) {
        next();
        while (true) {
            Condition cond;
            cond.column = next();

            std::string opTok = next();
            std::string opU = upper(opTok);
            if      (opTok == "=")  cond.op = Comparator::EQ;
            else if (opTok == "!=") cond.op = Comparator::NE;
            else if (opTok == "<")  cond.op = Comparator::LT;
            else if (opTok == ">")  cond.op = Comparator::GT;
            else if (opTok == "<=") cond.op = Comparator::LE;
            else if (opTok == ">=") cond.op = Comparator::GE;
            else if (opU  == "LIKE") cond.op = Comparator::LIKE;
            else throw std::runtime_error("Invalid comparison operator: " + opTok);

            cond.literal = literalToValue(next());
            plan.conditions.push_back(cond);

            std::string nx = upper(peek());
            if (nx == "AND") { next(); plan.connectors.push_back(Logic::AND); continue; }
            if (nx == "OR")  { next(); plan.connectors.push_back(Logic::OR);  continue; }
            break;
        }
    }

    // --- optional ORDER BY ---
    if (isKeyword(peek(), "ORDER")) {
        next();
        if (!isKeyword(next(), "BY"))
            throw std::runtime_error("Expected BY after ORDER");
        plan.orderBy.present = true;
        plan.orderBy.column = next();
        std::string dir = upper(peek());
        if (dir == "ASC")  { next(); plan.orderBy.descending = false; }
        else if (dir == "DESC") { next(); plan.orderBy.descending = true; }
    }

    // --- optional LIMIT ---
    if (isKeyword(peek(), "LIMIT")) {
        next();
        std::string n = next();
        try {
            size_t p;
            int lim = std::stoi(n, &p);
            if (p != n.size() || lim < 0) throw std::runtime_error("");
            plan.limit = lim;
        } catch (...) {
            throw std::runtime_error("LIMIT expects a non-negative integer, got: " + n);
        }
    }

    if (pos != tok.size())
        throw std::runtime_error("Unexpected trailing tokens starting at: " + tok[pos]);

    return plan;
}
