[![CMake Build Matrix](https://github.com/fuersten/csvsqldb/workflows/CMake%20Build%20Matrix/badge.svg?branch=master)](https://github.com/fuersten/csvsqldb/actions)

# csvsqldb
A read only sql database that gets its data from supplied csv files.
The data is accessed via an sql interface.
Most of the standard sql is supported, including joins.
The schema has to be specified beforehand, in a later version it shall be possible to guess it from the csv files.
The output is streamed in csv format to the standard output.
The tool can be used to rearrange, filter and process csv files.
It is much easier to use than the classic grep, awk, sort, cut and sed commands.

# Features
- create table for schema creation
- schema will be stored in a directory
- specify on the command line what csv pattern matches what table (table mapping)
- interactive mode with extended editing and history
- SQL to query the csv data
- SQL support includes: filtering, join, self join, aggregation, grouping, sorting, limit, subqueries etc.

# Supported plattforms
- Linux
- Mac OS X
- Windows (currently not functional)

# Status
It is not ready yet, but it is definitely usefull.

Most important issues, I am working on:
- port to Windows
- extraction of the schema from csv files
- extended projection step after aggregation operation in order to be able to calculate with aggregation results
- ORDER BY with expressions
- GROUP BY with expressions
- HAVING clause
- stdin streaming of csv data
- more built-in functions
- LUA UDFs
- more data types like INTERVAL, NUMERIC
- natural and outer join
- CASE WHEN expression
- better error handling and reporting
- more optimizations

# Usage

```
csvsqldb tool version 0.1.6
©2015-2016 Lars Fürstenberg
Options:
  --help                    shows this help
  --version                 shows the version of the program
  -i [ --interactive ]      opens an interactive sql shell
  -v [ --verbose ]          output verbose statistics
  --show-header-line arg    if set to 'on' outputs a header line
  -p [ --datbase-path ] arg path to the database
  -c [ --command-file ] arg command file with sql commands to process
  -s [ --sql ] arg          sql commands to call
  -m [ --mapping ] arg      mapping from csv file to table
  -f [ --files ] arg        csv files to process
```

## Example:

```
csvsqldb --sql="SELECT emp_no,birth_date,last_name FROM employees WHERE birth_date >= '1965-01-01' AND \
               gender = 'M' ORDER BY emp_no LIMIT 100" --mapping="emp\d+.csv->employees" emp.csv

#EMP_NO,BIRTH_DATE,LAST_NAME
10095,1965-01-03,'Morton'
10122,1965-01-19,'Esposito'
10291,1965-01-23,'Seghrouchni'
10476,1965-01-01,'Iisaka'
10663,1965-01-09,'Noriega'
...
```

You can find a detailed example here [Getting started](https://github.com/fuersten/csvsqldb/wiki/Getting-started).

## Interactive shell
The interactive shell was implemented with the linenoise library and supports most of the readline default key bindings.
It also supports a command history. Typeing _help_ will show you all possible commands in interactive mode.

# SQL support
## Supported datatypes
- BOOL
- INT (64-bit signed integer)
- REAL (long double)
- CHAR
- VARCHAR
- DATE
- TIME
- TIMESTAMP

## DDL statements
- Create table
- Drop table
- Alter table
- Create mapping

## DML statements
- Explain ast/exec
- Select
