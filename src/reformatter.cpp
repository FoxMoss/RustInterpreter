#include "lexer.hpp"
#include "main.hpp"
#include <cstdio>
#include <string>
#include <sys/types.h>
#include <vector>

const std::string TAB = "  ";

std::string str_multiply(std::string a, uint times) {

  std::string ret;
  for (uint level = 0; level < times; level++) {
    ret += TAB;
  }
  return ret;
}

const uint FILE_WIDTH = 100;
std::string reformat(std::vector<Token> raw_tokens) {
  std::string ret;
  std::vector<Token> tokens;
  for (size_t iter = 0; iter < raw_tokens.size(); iter++) {
    auto token = raw_tokens[iter];
    if (token.kind == TOKEN_WHITESPACE &&
        raw_tokens[iter + 1].kind == TOKEN_WHITESPACE) {
      continue;
    }
    tokens.push_back(token);
  }

  uint x = 0;
  struct Level {
    std::vector<Level> children = {};
    uint size = 0;
  };
  std::vector<Level> levels = {{}};
  for (size_t iter = 0; iter < tokens.size(); iter++) {
    auto token = tokens[iter];
    if (token.kind == TOKEN_OPEN_BRACE || token.kind == TOKEN_OPEN_PAREN) {
      if (x == 0) {
        Level level;
        levels.push_back(level);
      } else {
        Level *curent_level = (levels.end() - 1).base();
        for (uint i = 1; i < x; i++) {
          curent_level = (curent_level->children.end() - 1).base();
        }
        Level level;
        curent_level->children.push_back(level);
      }
      x++;
    }
    if (token.kind == TOKEN_CLOSE_BRACE || token.kind == TOKEN_CLOSE_PAREN) {
      x--;
    }

    Level *curent_level = (levels.end() - 1).base();
    for (uint i = 1; i < x; i++) {
      curent_level = (curent_level->children.end() - 1).base();
    }
    curent_level->size += token.length;
  }

  uint indent_level = 0;
  std::vector<uint> y_stack = {0};
  for (size_t iter = 0; iter < tokens.size(); iter++) {
    auto token = tokens[iter];

    if (token.kind == TOKEN_OPEN_BRACE || token.kind == TOKEN_OPEN_PAREN) {
      y_stack[indent_level]++;
      indent_level++;
      y_stack.push_back(0);
    }
    if (token.kind == TOKEN_CLOSE_BRACE || token.kind == TOKEN_CLOSE_PAREN) {
      indent_level--;
      y_stack.pop_back();
    }

    Level *curent_level = (levels.begin() + y_stack[0]).base();
    for (uint i = 1; indent_level < x; i++) {
      curent_level = (curent_level->children.begin() + y_stack[i]).base();
    }

    if ((token.kind == TOKEN_PLUS || token.kind == TOKEN_EQ) &&
        tokens[iter + 1].kind == TOKEN_WHITESPACE &&
        tokens[iter - 1].kind == TOKEN_WHITESPACE) {
      ret += " " + token.value + " ";
    } else if ((token.kind == TOKEN_COMMA) &&
               tokens[iter + 1].kind == TOKEN_WHITESPACE) {
      ret += token.value + " ";
      if (curent_level->size > FILE_WIDTH) {
        ret += "\n" + str_multiply(TAB, indent_level);
      }
    } else if (token.kind == TOKEN_SEMI) {
      ret += ";\n" + str_multiply(TAB, indent_level);
    } else if (token.kind == TOKEN_LINE_COMMENT) {
      if (tokens[iter - 1].kind != TOKEN_LINE_COMMENT) {
        ret += "\n";
      }
      ret += str_multiply(TAB, indent_level) + token.value +
             str_multiply(TAB, indent_level);
    } else if (token.kind != TOKEN_WHITESPACE) {

      if (token.kind == TOKEN_CLOSE_BRACE) {
        if (curent_level->size > FILE_WIDTH) {
          ret += "\n" + str_multiply(TAB, indent_level);
        }
      }

      ret += token.value;
      if (token.kind == TOKEN_IDENT &&
          tokens[iter + 1].kind == TOKEN_WHITESPACE &&
          tokens[iter + 2].kind == TOKEN_IDENT) {
        ret += " ";
      }

      if (token.kind == TOKEN_CLOSE_BRACE &&
          tokens[iter + 1].kind != TOKEN_SEMI &&
          tokens[iter + 1].kind != TOKEN_COMMA) {
        if (curent_level->size > FILE_WIDTH) {
          ret += "\n" + str_multiply(TAB, indent_level);
        }
      }
    }

    if (token.kind == TOKEN_OPEN_BRACE) {
      if (curent_level->size > FILE_WIDTH) {
        ret += "\n" + str_multiply(TAB, indent_level);
      }
    }
  }

  return ret;
}
