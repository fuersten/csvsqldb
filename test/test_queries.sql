
explain ast select emp_no, (first_name || ' ' || last_name)as name,
((CURRENT_DATE - birth_date) / 365)as dt from employees where birth_date > DATE '1965-01-01' order by dt;

explain exec select emp_no, (first_name || ' ' || last_name)as name,
((CURRENT_DATE - birth_date) / 365)as dt from employees where birth_date > DATE '1965-01-01' order by dt;

select emp_no, (first_name || ' ' || last_name)as name, (CURRENT_DATE - birth_date) / 365 as dt from employees where last_name = 'Schmiedel';

select emp_no, first_name || ' ' || last_name as name, gender,
extract(year from birth_date) from employees where extract(year from birth_date) >= 1965;

select emp_no, (first_name || ' ' || last_name)as name,
CAST(CURRENT_DATE - birth_date as REAL) / 365.0 as dt from employees where last_name = 'Schmiedel';

select emp_no, first_name || ' ' || last_name as name, (CURRENT_DATE - birth_date) / 365 as dt,
CURRENT_DATE from employees where last_name = 'Schmiedel' and emp_no > 490000;

select emp_no no, emp_no as id from employees where last_name = 'Schmiedel' and emp_no > 490000;

select count(*) from employees where birth_date > DATE '1965-01-01';

SELECT*, CURRENT_DATE as dt FROM employees WHERE birth_date > DATE '1965-01-01';

SELECT*, CURRENT_DATE as dt FROM employees WHERE birth_date > DATE '1965-01-01' order by birth_date;

select count(*) from employees;

select* from employees;

select* from employees order by last_name;

SELECT emp.emp_no, emp.first_name, emp.last_name as name, birth_date, hire_date, gender FROM employees emp WHERE last_name = 'Tsukuda';

select* from employees where emp_no < 10500 UNION(select* from employees where emp_no >= 10500);

select* from employees where emp_no < 250000 UNION(select* from employees where emp_no >= 250000);

select* from salaries sal join employees emp on sal.emp_no = emp.emp_no;

select* from salaries sal join employees emp on sal.emp_no = emp.emp_no where last_name = 'Tsukuda';

select* from employees emp join salaries sal on sal.emp_no = emp.emp_no;

select* from employees emp join salaries sal on sal.emp_no = emp.emp_no where emp.last_name = 'Delgrande';

select max(emp_no) from employees where birth_date > DATE '1965-01-01';

SELECT emp.first_name, emp.last_name, dept_emp.dept_no, departments.dept_name FROM employees as emp JOIN dept_emp ON emp.emp_no =
                                                        dept_emp.emp_no JOIN departments ON dept_emp.dept_no = departments.dept_no;

SELECT* FROM salaries WHERE salary > 65000;
