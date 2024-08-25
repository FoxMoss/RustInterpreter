#include "main.hpp"
#include "lexer.hpp"
#include <argparse/argparse.hpp>
#include <cstddef>
#include <cstdio>
#include <exception>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char *argv[]) {
  argparse::ArgumentParser parser("rustxx", "",
                                  argparse::default_arguments::help);
  parser.add_argument("file").required();

  try {
    parser.parse_args(argc, argv);
  } catch (std::exception e) {
    printf("rustxx: %s", e.what());
  }

  std::string name = parser.get<std::string>("file");
  std::ifstream file(name);

  std::stringstream full_file_stream;
  std::string line;
  while (std::getline(file, line)) {
    full_file_stream << line << "\n";
  }

  Lexer lexer(full_file_stream.str());
  Token next_token;
  std::vector<Token> tokens;
  while ((next_token = lexer.next_token()).kind != TOKEN_EOF) {
    tokens.push_back(next_token);
  }
  printf("%s\n", reformat(tokens).c_str());

  return 0;
}
