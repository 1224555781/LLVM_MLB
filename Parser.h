#pragma once


#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Lexer.h"

#include "llvm/Support/raw_ostream.h"


class Parser {
  Lexer lex_;
  Token token;
  bool HasError;

  void ReportError() {
    llvm::errs() << "Error: Unexpected token: " << token.GetText() << "\n";
    HasError = true;
  }

  void Advance() {
    lex_.Next(token);
  }

  bool Expect(Token::TokenKind Kind) {
    if (token.Is(Kind)) {
      ReportError();
      return true;
    }
    return false;
  }

  bool Consume(Token::TokenKind Kind) {
    if (Expect(Kind)) {
      return true;
    }
    Advance();
    return false;
  }
  
  
public:
  Parser(Lexer const& lexer)
    :lex_(lexer),HasError(false) {
    Advance();
  }

  AST* Parse();
};

#endif
