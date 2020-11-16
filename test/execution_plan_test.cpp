//
//  csvsqldb test
//
//  BSD 3-Clause License
//  Copyright (c) 2015, Lars-Christian Fürstenberg
//  All rights reserved.
//
//  Redistribution and use in source and binary forms, with or without modification, are permitted
//  provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice, this list of
//  conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice, this list of
//  conditions and the following disclaimer in the documentation and/or other materials provided
//  with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors may be used to
//  endorse or promote products derived from this software without specific prior written
//  permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR
//  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
//  AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
//  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
//  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
//  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
//  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
//  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
//  POSSIBILITY OF SUCH DAMAGE.
//


#include "libcsvsqldb/execution_plan_creator.h"
#include "libcsvsqldb/operatornode_factory.h"
#include "libcsvsqldb/sql_parser.h"
#include "libcsvsqldb/validation_visitor.h"

#include <boost/filesystem.hpp>

#include <catch2/catch.hpp>

#include <fstream>

namespace fs = boost::filesystem;


TEST_CASE("Execution Plan Test", "[engine]")
{
  SECTION("plan")
  {
    csvsqldb::FunctionRegistry functions;
    csvsqldb::SQLParser parser(functions);

    fs::path tempDir = fs::temp_directory_path();
    if (!fs::exists(tempDir)) {
      fs::create_directories(tempDir);
    }

    csvsqldb::ASTNodePtr node = parser.parse(
      "CREATE TABLE employees(emp_no INTEGER,birth_date DATE NOT NULL,first_name VARCHAR(25) NOT NULL,last_name VARCHAR(50) "
      "NOT NULL,gender CHAR,hire_date DATE,PRIMARY KEY(emp_no))");
    REQUIRE(node);
    REQUIRE(std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node));
    csvsqldb::ASTCreateTableNodePtr createNode = std::dynamic_pointer_cast<csvsqldb::ASTCreateTableNode>(node);

    csvsqldb::TableData tabledata = csvsqldb::TableData::fromCreateAST(createNode);
    csvsqldb::StringVector files;
    files.push_back((tempDir / "employees.csv").string());
    csvsqldb::FileMapping::Mappings mappings;
    mappings.push_back({"employees.csv->employees", ',', false});
    csvsqldb::FileMapping mapping;
    mapping.initialize(mappings);

    csvsqldb::Database database(tempDir.string(), mapping);
    database.addTable(tabledata);

    node = parser.parse(
      "SELECT emp_no,CAST((emp_no * 2) as real) as double_emp,emp.first_name as firstname,emp.last_name,birth_date,hire_date "
      "FROM employees AS emp WHERE birth_date between DATE'1960-01-01' and  DATE'1970-12-31' and gender IS NOT NULL;");
    REQUIRE(node);

    csvsqldb::SymbolTablePtr symbolTable = node->symbolTable();
    //        symbolTable->dump();
    //        std::cout << std::endl;

    node->typeSymbolTable(database);

    //        symbolTable->dump();
    //        std::cout << std::endl;

    std::fstream dataFile((tempDir / "employees.csv").string(), std::ios_base::trunc | std::ios_base::out);
    CHECK(dataFile);

    dataFile << (R"(emp_no,birth_date,first_name,last_name,gender,hire_date
47291,1960-09-09,Ulf,Flexer,M,2000-01-12
60134,1964-04-21,Seshu,Rathonyi,F,2000-01-02
72329,1953-02-09,Randi,Luit,F,2000-01-02
108201,1955-04-14,Mariangiola,Boreale,M,2000-01-01
205048,1960-09-12,Ennio,Alblas,F,2000-01-06
222965,1959-08-07,Volkmar,Perko,F,2000-01-13
226633,1958-06-10,Xuejun,Benzmuller,F,2000-01-04
227544,1954-11-17,Shahab,Demeyer,M,2000-01-08
422990,1953-04-09,Jaana,Verspoor,F,2000-01-11
424445,1953-04-27,Jeong,Boreale,M,2000-01-03
428377,1957-05-09,Yucai,Gerlach,M,2000-01-23
463807,1964-06-12,Bikash,Covnot,,2000-01-28
499553,1954-05-06,Hideyuki,Delgrande,F,2000-01-22
)");
    dataFile.close();

    csvsqldb::ExecutionPlan execPlan;
    csvsqldb::BlockManager manager;
    std::stringstream output;
    csvsqldb::OperatorContext context(database, functions, manager, files);
    csvsqldb::ASTValidationVisitor validationVisitor(database);
    node->accept(validationVisitor);
    csvsqldb::ExecutionPlanVisitor<csvsqldb::OperatorNodeFactory> execVisitor(context, execPlan, output);
    node->accept(execVisitor);

    CHECK(3 == execPlan.execute());

    std::string expected = R"(#EMP_NO,DOUBLE_EMP,FIRSTNAME,EMP.LAST_NAME,BIRTH_DATE,HIRE_DATE
47291,94582.000000,'Ulf','Flexer',1960-09-09,2000-01-12
60134,120268.000000,'Seshu','Rathonyi',1964-04-21,2000-01-02
205048,410096.000000,'Ennio','Alblas',1960-09-12,2000-01-06
)";

    CHECK(expected == output.str());
  }
}
