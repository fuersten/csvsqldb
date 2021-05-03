
# Changelog

## [0.2.0] - 2021-??-??

### Added

* Windows is now supported as plattform
* The project now uses github actions as CI
* Tons of unit tests have been added and the coverage is about 90%

### Changed

* Changed the SQL lexer to a generated re-flex lexer yielding huge performance improvements for lexing SQL statements
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

### Removed

* Removed all external thirdparty dependencies
* Removed boost

### Fixed

* Comparissions with NULL values will work correctly now

