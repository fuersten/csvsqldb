
#include "lex.yy.h"
#include "tokens.h"

#include <filesystem>
#include <fstream>


int process_stream(std::istream& stream)
{
  Lexer lexer{stream};
  auto token = lexer.lex();
  while(token._token != csvsqldb::TOK_EOI) {
    std::cout << csvsqldb::tokenToString(token._token) << " -> " << token._value << std::endl;
    token = lexer.lex();
  }
  return 0;
}

int main(int argc, char** argv)
{
  if (argc == 1) {
    std::stringstream ss;
    ss << "3 <> 4";

    return process_stream(ss);
  }
  std::ifstream file{std::filesystem::path{argv[1]}};

  return process_stream(file);
}
