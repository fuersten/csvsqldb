
# Roadmap

## Version 0.2.0

* detailed manual
* Windows expand (glob)
* Windows execution statistics
* interuptable operator nodes
* add format library
* allow quoted identifiers where allowed by standard
* rename ARBITRARY to ANY_VALUE
* VARCHAR with default length
* better error handling and reporting
* more optimizations

## Version 0.3.0

* extended projection step after aggregation operation in order to be able to calculate, group, and order with aggregation results
* replace own date, time, and timestamp with std implementation
* custom date, time, and timestamp formats
* support for time and timestamp milliseconds
* add TIMEZONE support to time, date and timestamp

## Version 0.4.0

* UTF-8 support
* stdin streaming of csv data
* test mapped files in csv import

## Version 0.5.0

* more built-in functions
* extraction of the schema from csv files
* replace LUA config with JSON config

## Version 0.6.0

* insert sort with limit
* ORDER BY field NULLS (LAST | FIRST) 
* CONVERT function (https://docs.microsoft.com/en-us/sql/t-sql/functions/cast-and-convert-transact-sql?view=sql-server-ver15)
* ORDER BY with expressions
* GROUP BY with expressions
* HAVING clause
* LUA UDFs
* more data types like INTERVAL, NUMERIC
* left and right join
* natural and outer join
* full join
* CASE WHEN expression

