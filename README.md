# Project B - CSV Mini Database and Query Engine

## Overview

This project implements a compact, lightweight CSV-based database and query engine written in C++. The system provides an in-memory database solution that can load, parse, and query CSV files using SQL-like syntax without requiring an external database management system (DBMS).

The core functionality revolves around efficient data storage and retrieval, supporting basic relational database operations adapted for CSV data sources.

## Key Features

| Feature Category | Description | Benefits |
|------------------|-------------|----------|
| **Data Loading** | Load and parse CSV files with automatic header detection and type inference for values | Handles various CSV formats seamlessly |
| **In-Memory Storage** | Store entire datasets in memory using optimized data structures | Provides fast query execution and data access |
| **Query Engine** | Execute SQL-like queries with support for multiple clauses | Familiar syntax for database operations |
| **Expression Support** | Handle comparisons, logical operations, and aggregations | Enables complex filtering and data analysis |
| **Command-Line Interface** | Interactive CLI for executing queries on CSV datasets | Easy-to-use interface for data exploration |

## Supported Query Operations

The query engine supports the following SQL-like operations:

| Operation | Syntax | Description | Example |
|-----------|--------|-------------|---------|
| **SELECT** | `SELECT column1, column2, ...` | Specify columns to retrieve | `SELECT name, age, city` |
| **FROM** | `FROM table` | Specify the data source | `FROM employees` |
| **WHERE** | `WHERE condition` | Filter rows based on conditions | `WHERE age > 25 AND city = 'NYC'` |
| **ORDER BY** | `ORDER BY column [ASC|DESC]` | Sort results by column | `ORDER BY salary DESC` |
| **LIMIT** | `LIMIT n` | Restrict number of results | `LIMIT 100` |

## Supported Data Types and Operations

| Data Type | Description | Supported Operations |
|-----------|-------------|---------------------|
| **String** | Text values | Equality (=, !=), Pattern matching (LIKE) |
| **Integer** | Whole numbers | Comparisons (<, >, <=, >=, =, !=) |
| **Float** | Decimal numbers | Comparisons (<, >, <=, >=, =, !=), Arithmetic |
| **Boolean** | True/False values | Logical operations (AND, OR, NOT) |

## Aggregation Functions

| Function | Description | Example |
|----------|-------------|---------|
| **COUNT** | Count rows or non-null values | `SELECT COUNT(*) FROM table` |
| **SUM** | Sum numeric values | `SELECT SUM(salary) FROM employees` |
| **AVG** | Calculate average | `SELECT AVG(age) FROM users` |
| **MIN/MAX** | Find minimum/maximum values | `SELECT MIN(price), MAX(price) FROM products` |

## Building and Installation

### Prerequisites
- C++ compiler (g++ recommended)
- Standard C++ libraries

### Compilation
Use the provided build task in VS Code or compile manually:

```bash
g++ -g main.cpp -o csv_db
```

### Usage
1. Load a CSV file into the database
2. Execute queries through the command-line interface
3. View results in tabular format

## Example Usage

```sql
-- Load data
LOAD employees.csv

-- Simple query
SELECT name, department FROM employees WHERE salary > 50000

-- Aggregated query
SELECT department, COUNT(*), AVG(salary) FROM employees GROUP BY department ORDER BY AVG(salary) DESC LIMIT 5
```

## Architecture

The system consists of:
- **CSV Parser**: Handles file reading and data type inference
- **In-Memory Store**: Optimized data structures for fast access
- **Query Parser**: Interprets SQL-like syntax
- **Execution Engine**: Processes queries and returns results
- **CLI Interface**: User interaction layer

## Limitations

- In-memory only (data must fit in RAM)
- No persistent storage
- Limited to single-table operations
- Basic aggregation support
- No JOIN operations

## Future Enhancements

- Multi-table support with JOIN operations
- Persistent storage options
- Advanced aggregation functions
- Query optimization
- Export results to various formats

---

*This project is part of CS351 coursework, focusing on database systems implementation.*