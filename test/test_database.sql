
CREATE TABLE IF NOT EXISTS employees(emp_no INTEGER,
                                     birth_date DATE NOT NULL,
                                     first_name VARCHAR(25) NOT NULL,
                                     last_name VARCHAR(50) NOT NULL,
                                     gender CHAR,
                                     hire_date DATE,
                                     PRIMARY KEY(emp_no));

CREATE TABLE IF NOT EXISTS salaries(emp_no INTEGER PRIMARY KEY, salary FLOAT CHECK(salary > 0.0), from_date DATE, to_date DATE);

CREATE TABLE IF NOT EXISTS departments(dept_no VARCHAR(4) PRIMARY KEY, dept_name CHAR(40));

CREATE TABLE IF NOT EXISTS dept_emp(emp_no INTEGER, dept_no CHAR(4), from_date DATE, to_date DATE, PRIMARY KEY(emp_no, dept_no));

CREATE TABLE IF NOT EXISTS dept_manager(dept_no CHAR(4), emp_no INTEGER, from_date DATE, to_date DATE, PRIMARY KEY(dept_no, emp_no));

CREATE MAPPING employees("employees\d*.csv");

CREATE MAPPING salaries("salaries\d*.csv");

CREATE MAPPING departments("departments\d*.csv");

CREATE MAPPING dept_emp("dept_emp\d*.csv");
