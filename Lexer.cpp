#include "Lexer.h"

void MLB::Lexer::Next(Token &token) {
  while (*BufferPtr && charinfo::IsWhiteSpace(*BufferPtr)) {
    ++BufferPtr;
  }
  if (!*BufferPtr) {
    token.SetKind(Token::eoi);
    return;
  }

  if (charinfo::IsLetter(*BufferPtr)) {
    char const *end = BufferPtr + 1;
    while (charinfo::IsLetter(*end)) {
      ++end;
    }
    llvm::StringRef Name(BufferPtr, end - BufferPtr);
    Token::TokenKind kind = Name == "with" ? Token::KW_with : Token::ident;
    FromToken(token, end, kind);
    return;
  } else if (charinfo::IsDigit(*BufferPtr)) {
    char const *end = BufferPtr + 1;
    while (charinfo::IsDigit(*end)) {
      ++end;
    }
    FromToken(token, end, Token::number);
    return;
  } else {
    switch (*BufferPtr) {
#define CASE(ch, tok)                                                          \
case ch:                                                                     \
FromToken(token, BufferPtr + 1, tok);                                      \
break
      CASE('+', Token::plus);
      CASE('-', Token::minus);
      CASE('*', Token::star);
      CASE('/', Token::slash);
      CASE('(', Token::Token::l_paren);
      CASE(')', Token::Token::r_paren);
      CASE(':', Token::Token::colon);
      CASE(',', Token::Token::comma);
#undef CASE
    default:
      FromToken(token, BufferPtr + 1, Token::unknown);
    }
  }
}

void MLB::Lexer::FromToken(Token &Result, char const *TokEnd,
                           Token::TokenKind Kind) {
  Result.SetKind(Kind);
  Result.SetText(llvm::StringRef(BufferPtr, TokEnd - BufferPtr));
  BufferPtr = TokEnd;
}