
# Changelog

## [0.2.0] - 2021-??-??

### Added

* Windows is now supported as plattform
* The project now uses github actions as CI
* Tons of unit tests have been added and the coverage is about 90%
* The number of maximum active blocks can now be configured interactively and by command line
* The cast function can now also cast from real, bool, date, integer, date, time, timestamp to varchar
* New system tables for database meta data (like tables, columns, functions, and mappings) where added
* Better memory block handlng. The memory block generation adapts to the block consumption and uses significantly less memory.
* Added gcc as mac os x compiler

### Changed

* Changed the SQL lexer to a generated re-flex lexer yielding huge performance improvements for lexing SQL statements
* Replaced CSV parser with a much cleaner and optimized version yielding significant performance improvements
* Restructured the project and split files into more logical units
* Updated the code to use C++17
* Migrated tests to catch2
* Replaced own any implementation with std::any
* Replaced own JSON implementation with nlohmann json
* Replaced boost::program_options with CLI11
* Replaced boost::filesystem with std::filesystem
* Replaced boost::regex with std::regex
* Lots of small refactorings and cleanups
* All thridparty dependencies are now handled via submodules or are fetched while configuring
* Uses fast_float library for parsing floating point numbers
* More compiler warnings activated

### Removed

* Removed all external thirdparty dependencies
* Removed boost

### Fixed

* Comparissions with NULL values will work correctly now
* Fixed race condition in table scan operator
* Fixed race condition in block manager and other operators
* Fixed exception handling concerning exceeding the maximum allowed number of blocks

