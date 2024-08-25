#pragma once
#include "main.hpp"
#include <cstddef>
#include <cstdio>
#include <fstream>
#include <string>

std::string reformat(std::vector<Token> tokens);
class Lexer {
public:
  Lexer(std::string file) : file(file) {}

  Token next_token() {
    while (cursor < file.size()) {
      if (vector_has(&whitespace, (unsigned char)file[cursor])) {
        Token token;
        token.kind = TOKEN_WHITESPACE;
        token.value = {file[cursor]};
        token.length = 1;
        cursor++;
        return token;
      }
      if (file[cursor] == '/') {
        if (file[cursor + 1] == '/') {
          return line_comment();
        }
        // broken i dont know why
        // if (file[cursor + 1] == '*') {
        //   return block_comment();
        // }
      }
      if (file[cursor] == '"') {
        return quoted_string();
      }
      if (file[cursor] == '\'') {
        return char_or_lifetime();
      }
      if (is_ident_char(file[cursor])) {
        return ident_or_unknown();
      }

      auto punctor_token = vector_has(file.size(), cursor, file, punctor);
      if (punctor_token.has_value()) {
        Token token;
        token.kind = punctor_token->second;
        token.value = {file[cursor]};
        token.length = 1;
        cursor++;
        return token;
      }

      Token token;
      token.kind = TOKEN_UNKNOWN;
      token.length = 1;
      token.value = {file[cursor]};
      cursor++;
      return token;
    }
    Token token;
    token.kind = TOKEN_EOF;
    return token;
  }

private:
  std::string file;
  size_t cursor = 0;

  // not spec but this shit would be so hard to parse utf8
  bool is_ident_char(char c) {
    if (('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_')
      return true;
    return false;
  }
  Token ident_or_unknown() {
    Token ret;
    ret.value = std::string(1, file[cursor]);
    cursor++;
    ret.length++;
    ret.kind = TOKEN_IDENT;
    while (is_ident_char(file[cursor])) {
      ret.value.push_back(file[cursor]);
      ret.length++;
      cursor++;
    }
    if (file[cursor] == '#' || file[cursor] == '"' || file[cursor] == '\'') {
      ret.value.push_back(file[cursor]);
      ret.length++;
      ret.kind = TOKEN_UNKNOWN_PREFIX;
      cursor++;
    }
    // acording to the source im blantly taking this from it could also be a
    // fake ident but no clue what that is
    return ret;
  }

  Token line_comment() {
    Token ret;
    ret.value = std::string(1, file[cursor]);
    cursor++;
    ret.length++;
    ret.kind = TOKEN_LINE_COMMENT;

    while (file[cursor] != '\n') {
      ret.value.push_back(file[cursor]);
      ret.length++;
      cursor++;
    }

    ret.value.push_back(file[cursor]);
    ret.length++;
    cursor++;
    return ret;
  }
  Token block_comment() {
    Token ret;
    ret.value.push_back(file[cursor]);
    ret.value.push_back(file[cursor + 1]);
    cursor += 2;
    ret.length += 2;
    ret.kind = TOKEN_BLOCK_COMMENT;

    /* okay */
    while (file[cursor] != '/' && file[cursor - 1] != '*') {
      // if (file[cursor] == 0) {
      //   break;
      // }
      ret.value.push_back(file[cursor]);
      ret.length++;
      cursor++;
    }

    ret.value.push_back(file[cursor]);
    ret.length++;
    cursor++;

    return ret;
  }

  Token quoted_string() {
    Token ret;
    ret.value = std::string(1, file[cursor]);
    cursor++;
    ret.length++;
    ret.kind = TOKEN_LITERAL;

    while (file[cursor] != 0) {
      if (file[cursor] == '"') {
        break;
      }
      if (file[cursor] == '\\' && file[cursor + 1] == '"') {
        ret.value.push_back(file[cursor]);
        ret.length++;
        cursor++;
      }

      ret.value.push_back(file[cursor]);
      ret.length++;
      cursor++;
    }
    ret.value.push_back(file[cursor]);
    ret.length++;
    cursor++;
    return ret;
  }

  Token char_or_lifetime() {
    Token ret;
    ret.value = std::string(1, file[cursor]);
    cursor++;
    ret.length++;
    ret.kind = TOKEN_LITERAL;

    if (!(file[cursor + 1] == '\'' || file[cursor + 2] == '\'')) {
      ret.kind = TOKEN_LIFETIME;
      return ret;
    }

    while (file[cursor] != 0) {
      ret.value.push_back(file[cursor]);
      ret.length++;
      cursor++;
      if (file[cursor] == '\'') {
        break;
      }
      if (file[cursor] == '\\' && file[cursor + 1] == '\'') {
        ret.value.push_back(file[cursor]);
        ret.length++;
        cursor++;
      }
    }
    ret.value.push_back(file[cursor]);
    ret.length++;
    cursor++;
    return ret;
  }
};
