# SQLite Clone

A lightweight, educational implementation of a relational database engine built from scratch in C++. This project demonstrates fundamental database concepts including B+ tree storage, page-based file management, and SQL-like command processing.

## Features

- **B+ Tree Storage Engine**: Efficient data storage and retrieval using B+ tree data structures
- **Page-based File Management**: 4KB pages with proper serialization/deserialization
- **SQL-like Interface**: Support for basic `INSERT` and `SELECT` operations
- **Persistent Storage**: Data persists to disk between sessions
- **Memory Management**: Efficient page caching and memory allocation
- **Comprehensive Testing**: Full test suite using Google Test framework

## Architecture

### Core Components

- **Pager**: Manages page-based file I/O and memory caching
- **B+ Tree Implementation**: Handles data organization with leaf and internal nodes
- **SQL Parser**: Processes INSERT and SELECT statements
- **Row Serialization**: Efficient binary storage format
- **Cursor System**: Provides navigation through the data structure

### Data Model

The database currently supports a simple user table with the following schema:
```sql
CREATE TABLE users (
    id INTEGER PRIMARY KEY,
    username VARCHAR(32),
    email VARCHAR(255)
);
```

## Prerequisites

- **CMake** 3.8 or higher
- **C++17** compatible compiler (GCC, Clang, or MSVC)
- **Git** (for fetching Google Test dependency)

## Building the Project

### Quick Start

```bash
# Clone the repository
git clone <repository-url>
cd sqlite-clone

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake ..

# Build the project
make

# Run tests
./DatabaseTest
```

### Build Targets

- `sqlite` - Main database executable
- `DatabaseTest` - Test suite executable

### CMake Configuration

The project uses modern CMake practices with:
- Automatic dependency management (Google Test via FetchContent)
- Cross-platform compatibility (Windows, macOS, Linux)
- Export of compile commands for IDE integration
- Comprehensive compiler warnings enabled

## Usage

### Starting the Database

```bash
./sqlite database_name.db
```

### Supported Commands

#### Data Manipulation
```sql
-- Insert a new record
insert 1 john john@example.com

-- Select all records
select
```

#### Meta Commands
```bash
-- Exit the database
.exit

-- View B+ tree structure
.btree

-- Display internal constants
.constants
```

### Example Session

```bash
$ ./sqlite test.db
db > insert 1 alice alice@example.com
Executed.
db > insert 2 bob bob@example.com
Executed.
db > select
(1, alice, alice@example.com)
(2, bob, bob@example.com)
Executed.
db > .btree
Tree:
- leaf (size 2)
  - 1
  - 2
db > .exit
```

## Testing

The project includes comprehensive integration tests that verify:

- **Basic CRUD Operations**: Insert and select functionality
- **Data Persistence**: Data survives database restarts
- **Input Validation**: Proper handling of invalid inputs
- **String Length Limits**: Username (32 chars) and email (255 chars) validation
- **Duplicate Key Detection**: Primary key constraint enforcement
- **B+ Tree Structure**: Correct tree organization and display

### Running Tests

```bash
# Run all tests
./DatabaseTest

# Tests are automatically run after build
make
```

### Test Coverage

- ✅ Insert and retrieve operations
- ✅ Maximum string length validation
- ✅ Negative ID rejection
- ✅ Data persistence across sessions
- ✅ Duplicate key detection
- ✅ B+ tree structure validation
- ✅ Constants verification

## File Structure

```
├── CMakeLists.txt          # CMake build configuration
├── src/
│   └── main.cpp           # Main database implementation
├── tests/
│   └── executable_tests.cpp # Integration test suite
├── include/               # Header files (if any)
└── lib/                   # External libraries (if any)
```

## Implementation Details

### Storage Format

- **Page Size**: 4096 bytes
- **Node Types**: Leaf nodes (data) and internal nodes (routing)
- **Row Format**: Binary serialized with fixed-width fields
- **File Format**: Pages stored sequentially with metadata headers

### Memory Management

- **Page Cache**: In-memory cache for frequently accessed pages
- **Lazy Loading**: Pages loaded on-demand from disk
- **Write-through**: Changes immediately persisted to disk

### Limitations

Current implementation limitations (educational purposes):

- Single table support
- Fixed schema (id, username, email)
- No concurrent access
- Limited SQL command set
- No indexing beyond primary key
- No transaction support
- No query optimization

## Development

### Adding New Features

1. **New Commands**: Extend the meta-command parser in `do_meta_command()`
2. **SQL Operations**: Add cases to `prepare_statement()` and corresponding execute functions
3. **Data Types**: Modify row structure and serialization functions
4. **Storage**: Enhance pager for advanced features

### Contributing

1. Fork the repository
2. Create a feature branch
3. Add comprehensive tests for new functionality
4. Ensure all tests pass
5. Submit a pull request

## Educational Value

This project demonstrates:

- **Database Internals**: How databases store and retrieve data
- **B+ Tree Implementation**: Balanced tree data structures
- **File System Interface**: Low-level file I/O operations
- **Memory Management**: Page-based caching strategies
- **Parser Design**: Command-line interface parsing
- **Testing Strategies**: Integration testing for databases

## License

This project is designed for educational purposes. Please refer to the license file for usage terms.

## References

- [Database Internals by Alex Petrov](https://www.databass.dev/)
- [SQLite Architecture Documentation](https://www.sqlite.org/arch.html)
- [Let's Build a Simple Database](https://cstack.github.io/db_tutorial/)

---

**Note**: This is an educational implementation and should not be used in production environments. For production use cases, consider established database systems like SQLite, PostgreSQL, or MySQL.