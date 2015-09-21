
select* from employees into outfile '/Users/lfg/temp/csvDB/employees11.csv' fields terminated by ',' lines terminated by '\n';

select* from salaries into outfile '/Users/lfg/temp/csvDB/salaries11.csv' fields terminated by ',' lines terminated by '\n';

select* from dept_emp into outfile '/Users/lfg/temp/csvDB/dept_emp11.csv' fields terminated by ',' lines terminated by '\n';

select* from departments into outfile '/Users/lfg/temp/csvDB/departments11.csv' fields terminated by ',' lines terminated by '\n';
