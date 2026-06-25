#include "executor.h"
#include "value_util.h"
#include <stdexcept>
#include <algorithm>
#include <cctype>
#include <limits>

namespace {

std::string lower(const std::string& s) {
    std::string r = s;
    std::transform(r.begin(), r.end(), r.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return r;
}

// Evaluate one WHERE condition against a row value.
bool evalCondition(const Value& cell, Comparator op, const Value& literal) {
    if (op == Comparator::LIKE) {
        return valueutil::like(valueutil::toString(cell), valueutil::toString(literal));
    }
    int c = valueutil::compare(cell, literal);
    switch (op) {
        case Comparator::EQ: return c == 0;
        case Comparator::NE: return c != 0;
        case Comparator::LT: return c < 0;
        case Comparator::GT: return c > 0;
        case Comparator::LE: return c <= 0;
        case Comparator::GE: return c >= 0;
        default:             return false;
    }
}

const char* aggName(AggFunc f) {
    switch (f) {
        case AggFunc::COUNT: return "COUNT";
        case AggFunc::SUM:   return "SUM";
        case AggFunc::AVG:   return "AVG";
        case AggFunc::MIN:   return "MIN";
        case AggFunc::MAX:   return "MAX";
        default:             return "?";
    }
}

} // namespace

size_t Executor::columnIndex(const Table& table, const std::string& name) {
    std::string target = lower(name);
    for (size_t i = 0; i < table.headers.size(); ++i)
        if (lower(table.headers[i]) == target) return i;
    throw std::runtime_error("Unknown column: " + name);
}

bool Executor::rowMatches(const QueryPlan& plan, const Table& table,
                          const std::vector<Value>& row) {
    if (plan.conditions.empty()) return true;

    // Evaluate left-to-right (no operator precedence, like a simple chain).
    bool acc = evalCondition(row[columnIndex(table, plan.conditions[0].column)],
                             plan.conditions[0].op, plan.conditions[0].literal);
    for (size_t i = 1; i < plan.conditions.size(); ++i) {
        bool next = evalCondition(row[columnIndex(table, plan.conditions[i].column)],
                                  plan.conditions[i].op, plan.conditions[i].literal);
        Logic link = plan.connectors[i - 1];
        acc = (link == Logic::AND) ? (acc && next) : (acc || next);
    }
    return acc;
}

ResultSet Executor::execute(const QueryPlan& plan, const Database& db) {
    if (!db.hasTable(plan.table))
        throw std::runtime_error("Table not found: " + plan.table);
    const Table& table = db.getTable(plan.table);

    // 1. WHERE: collect matching row indices.
    std::vector<size_t> matched;
    for (size_t r = 0; r < table.rows.size(); ++r)
        if (rowMatches(plan, table, table.rows[r])) matched.push_back(r);

    ResultSet result;

    // 2a. Aggregate query => single summary row.
    if (plan.hasAggregate) {
        for (const SelectItem& item : plan.items) {
            std::string label = std::string(aggName(item.func)) + "(" + item.column + ")";
            result.headers.push_back(label);

            if (item.func == AggFunc::COUNT) {
                result.rows.resize(1);
                result.rows[0].push_back(static_cast<int>(matched.size()));
                continue;
            }

            // SUM / AVG / MIN / MAX operate on a numeric column.
            size_t ci = columnIndex(table, item.column);
            bool any = false;
            double sum = 0.0;
            double mn = std::numeric_limits<double>::infinity();
            double mx = -std::numeric_limits<double>::infinity();
            for (size_t r : matched) {
                const Value& v = table.rows[r][ci];
                if (!valueutil::isNumeric(v))
                    throw std::runtime_error(std::string(aggName(item.func)) +
                        " requires a numeric column, but '" + item.column + "' is not numeric");
                double d = valueutil::toNumber(v);
                sum += d; mn = std::min(mn, d); mx = std::max(mx, d);
                any = true;
            }
            result.rows.resize(1);
            if (!any) { result.rows[0].push_back(std::string("NULL")); continue; }
            double out = (item.func == AggFunc::SUM) ? sum :
                         (item.func == AggFunc::AVG) ? sum / matched.size() :
                         (item.func == AggFunc::MIN) ? mn : mx;
            result.rows[0].push_back(out);
        }
        return result;
    }

    // 2b. Row query: determine output columns.
    std::vector<size_t> cols;
    if (plan.selectAll) {
        result.headers = table.headers;
        for (size_t i = 0; i < table.headers.size(); ++i) cols.push_back(i);
    } else {
        for (const SelectItem& item : plan.items) {
            size_t ci = columnIndex(table, item.column);
            cols.push_back(ci);
            result.headers.push_back(table.headers[ci]);
        }
    }

    // 3. ORDER BY (applied to matched row indices before projection).
    if (plan.orderBy.present) {
        size_t oc = columnIndex(table, plan.orderBy.column);
        bool desc = plan.orderBy.descending;
        std::stable_sort(matched.begin(), matched.end(),
            [&](size_t a, size_t b) {
                int cmp = valueutil::compare(table.rows[a][oc], table.rows[b][oc]);
                return desc ? (cmp > 0) : (cmp < 0);
            });
    }

    // 4. LIMIT + 5. projection.
    size_t count = matched.size();
    if (plan.limit >= 0) count = std::min(count, static_cast<size_t>(plan.limit));
    for (size_t k = 0; k < count; ++k) {
        const std::vector<Value>& src = table.rows[matched[k]];
        std::vector<Value> out;
        for (size_t ci : cols) out.push_back(src[ci]);
        result.rows.push_back(std::move(out));
    }
    return result;
}
