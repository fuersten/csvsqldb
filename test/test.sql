
SELECT emp_no,CAST((emp_no * 2) as real) as double_emp FROM employees;

SELECT * from salaries;

SELECT emp.*,emp.emp_no as id,(first_name || ' ' || last_name) as name,birth_date birthday, 7 * 5 / 4 as calc FROM employees emp WHERE emp_no BETWEEN 100 AND 99999 AND emp.birth_date > DATE'1960-01-01';

SELECT emp.first_name as first_name,emp.last_name,birth_date as bd,hire_date FROM employees AS emp WHERE birth_date > DATE'1960-01-01';

SELECT emp_no,CAST((emp_no * 2) as real) as double_emp,emp.first_name as firstname,emp.last_name,birth_date,hire_date FROM employees AS emp WHERE birth_date between DATE'1960-01-01' and  DATE'1970-12-31';

SELECT emp.first_name,emp.last_name,sal.salary 
    FROM employees as emp 
        JOIN salaries as sal 
            ON emp.emp_no = sal.emp_no;

SELECT emp.first_name,emp.last_name,dept_emp.dept_no,departments.dept_name
    FROM employees as emp
        JOIN dept_emp
            ON emp.emp_no = dept_emp.emp_no
        JOIN departments
            ON dept_emp.dept_no = departments.dept_no;


