#pragma once
#include <cstddef>
#include <cstdio>
#include <optional>
#include <string>
#include <vector>
// super credit to
// https://github.com/rust-lang/rust/blob/master/compiler/rustc_lexer/src/lib.rs

const std::vector<unsigned char> whitespace = {0x09, 0x0A, 0x0B, 0x0C,
                                               0x0D, 0x20, 0x85};

enum TokenKind {
  TOKEN_LINE_COMMENT,
  TOKEN_BLOCK_COMMENT,
  TOKEN_WHITESPACE,
  TOKEN_IDENT,
  TOKEN_INVALID_IDENT,
  TOKEN_RAW_IDENT,
  TOKEN_UNKNOWN_PREFIX,
  TOKEN_INVALID_PREFIX,
  TOKEN_LITERAL,
  TOKEN_LIFETIME,

  // single char
  TOKEN_SEMI,
  TOKEN_COMMA,
  TOKEN_DOT,
  TOKEN_OPEN_PAREN,
  TOKEN_CLOSE_PAREN,
  TOKEN_OPEN_BRACE,
  TOKEN_CLOSE_BRACE,
  TOKEN_OPEN_BRACKET,
  TOKEN_CLOSE_BRACKET,
  TOKEN_AT,
  TOKEN_POUND,
  TOKEN_TILDE,
  TOKEN_QUESTION,
  TOKEN_COLON,
  TOKEN_DOLLAR,
  TOKEN_EQ,
  TOKEN_BANG,
  TOKEN_LT,
  TOKEN_GT,
  TOKEN_MINUS,
  TOKEN_AND,
  TOKEN_OR,
  TOKEN_PLUS,
  TOKEN_STAR,
  TOKEN_SLASH,
  TOKEN_CARET,
  TOKEN_PERCENT,
  TOKEN_UNKNOWN,
  TOKEN_EOF,
};

const std::vector<std::pair<std::string, TokenKind>> punctor = {
    {"+", TOKEN_PLUS},
    {"-", TOKEN_MINUS},
    {"*", TOKEN_STAR},
    {"/", TOKEN_SLASH},
    {"%", TOKEN_PERCENT},
    {"^", TOKEN_CARET},
    {"!", TOKEN_BANG},
    {"&", TOKEN_AND},
    {"|", TOKEN_OR},
    {"@", TOKEN_AT},
    {".", TOKEN_DOT},
    {",", TOKEN_COMMA},
    {";", TOKEN_SEMI},
    {":", TOKEN_COLON},
    {"#", TOKEN_POUND},
    {"$", TOKEN_DOLLAR},
    {"?", TOKEN_QUESTION},
    {"{", TOKEN_OPEN_BRACE},
    {"}", TOKEN_CLOSE_BRACE},
    {"[", TOKEN_OPEN_BRACKET},
    {"]", TOKEN_CLOSE_BRACKET},
    {"(", TOKEN_OPEN_PAREN},
    {")", TOKEN_CLOSE_PAREN},
    {"<", TOKEN_LT},
    {">", TOKEN_GT},
    {"~", TOKEN_TILDE},
    {"=", TOKEN_EQ},
};

//     "_", "&&", "||", "<<", ">>",
//     "+=", "-=", "*=", "/=", "%=", "^=", "&=", "|=", "<<=", ">>=",
//     "==", "!=", ">", "<", ">=", "<=", "..", "...", "..=",
//     "::", "->", "=>",

class Token {
public:
  std::string value = "";
  size_t length = 0;
  TokenKind kind = TOKEN_UNKNOWN;
};

template <typename T> bool vector_has(const std::vector<T> *vec, T val) {
  for (T possible_val : *vec)
    if (val == possible_val)
      return true;
  return false;
}

static std::optional<std::pair<std::string, TokenKind>>
vector_has(size_t size, size_t index, std::string str,
           std::vector<std::pair<std::string, TokenKind>> matches) {
  std::vector<std::pair<std::string, TokenKind>> actual_matches;

  for (auto match : matches) {
    if (str.size() < index + match.first.size())
      continue;

    bool matches = true;
    for (size_t match_index = 0; match_index < match.first.size();
         match_index++) {
      if (match.first[match_index] != str[index + match_index]) {
        matches = false;
      }
    }
    if (matches)
      actual_matches.push_back(match);
  }

  if (actual_matches.empty())
    return {};

  std::pair<std::string, TokenKind> longest_match;
  for (auto match : actual_matches) {
    if (longest_match.first.size() < match.first.size())
      longest_match = match;
  }

  return longest_match;
}
