#pragma once


#ifndef PARSER_H
#define PARSER_H

#include "AST.h"
#include "Lexer.h"
#include "llvm/Support/raw_ostream.h"


class Parser {
  MLB::Lexer lex_;
  MLB::Token token;
  unsigned char bHasError:1 ;

  void ReportError() {
    llvm::errs() << "Error: Unexpected token: " << token.GetText() << "\n";
    bHasError = true;
  }

  void Advance() {
    lex_.Next(token);
  }

  bool Expect(MLB::Token::TokenKind Kind) {
    if (token.Is(Kind)) {
      ReportError();
      return true;
    }
    return false;
  }

  bool Consume(MLB::Token::TokenKind Kind) {
    if (Expect(Kind)) {
      return true;
    }
    Advance();
    return false;
  }

  MLB::AST* ParseCalc();
  MLB::Expr* ParseExpr();
  MLB::Expr* ParseTerm();
  MLB::Expr* ParseFactory();
  
public:
  Parser(MLB::Lexer const& lexer)
    :lex_(lexer),bHasError(false) {
    Advance();
  }
bool HasError(){return bHasError;}
  MLB::AST* Parse();
};

#endif
