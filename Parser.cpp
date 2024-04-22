#include "Parser.h"

#include <llvm/TargetParser/ARMTargetParser.h>

MLB::AST * Parser::Parse() {
  MLB::AST *Result = ParseCalc();
  Expect(MLB::Token::eoi);

  return Result;
}

MLB::AST * Parser::ParseCalc() {
  MLB::Expr* E = nullptr;
  llvm::SmallVector<llvm::StringRef,8> Vars;
  if (token.Is(MLB::Token::KW_with)) {
    Advance();
    if (Expect(MLB::Token::ident)) {
      goto _error;
    }
    Vars.push_back(token.GetText());
    Advance();
    while (token.Is(MLB::Token::comma)) {
      Advance();
      if (Expect(MLB::Token::ident)) {
        goto _error;  // NOLINT(hicpp-avoid-goto)
      }
      Vars.push_back(token.GetText());
      Advance();
    }
    if (Consume(MLB::Token::colon)) {
      goto _error;  // NOLINT(cppcoreguidelines-avoid-goto)
    }
  }
  E = ParseExpr();
  if (Vars.empty()) {
    return E;
  }


  _error:
    while (!token.Is(MLB::Token::eoi)) {
      Advance();
    }
  return nullptr;
}


MLB::Expr * Parser::ParseExpr() {
  MLB::Expr* Left = ParseTerm();
  while (token.IsOneOf(MLB::Token::plus,MLB::Token::minus)) {
    MLB::BinaryOperator::Operator Op = token.Is(MLB::Token::plus) ? MLB::BinaryOperator::Plus : MLB::BinaryOperator::Minus;
    Advance();
    MLB::Expr* Right = ParseTerm();
    Left = new MLB::BinaryOperator(Op,Left, Right);
  }

  return Left;
}

MLB::Expr * Parser::ParseTerm() {
  MLB::Expr* Left = ParseFactory();
  while (token.IsOneOf(MLB::Token::star,MLB::Token::slash)) {
    MLB::BinaryOperator::Operator Op = token.Is(MLB::Token::star) ? MLB::BinaryOperator::Mul : MLB::BinaryOperator::Div;
    Advance();
    MLB::Expr* Right = ParseFactory();
    Left = new MLB::BinaryOperator(Op,Left,Right);
  }

  return Left;
}

MLB::Expr * Parser::ParseFactory() {
  MLB::Expr* Result = nullptr;

  switch (token.GetKind()) {
  case MLB::Token::number:
    Result = new MLB::Factor(MLB::Factor::Number,token.GetText());
    Advance();
    break;
    case MLB::Token::ident:
      Result = new MLB::Factor(MLB::Factor::Ident,token.GetText());
      Advance();
      break;
  case MLB::Token::l_paren:
    Advance();
    Result = ParseExpr();
    if (!Consume(MLB::Token::r_paren)) {
      break;
    }
  
  default:
      if (!Result) {
        ReportError();
      }
    while (!token.IsOneOf(MLB::Token::r_paren,MLB::Token::star,MLB::Token::plus,MLB::Token::minus,MLB::Token::slash,MLB::Token::eoi)) {
      Advance();
    }

    return Result;
  }

  return Result;
}
