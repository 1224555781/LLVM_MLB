#ifndef LEXER_H
#define LEXER_H
#endif

#include "llvm/ADT/StringRef.h"


namespace charinfo {
LLVM_READNONE inline bool IsWhiteSpace(char c) {
    return c == ' ' || 
           c == '\t'||
           c == '\n'||
           c == '\v'||
           c == '\f' || c == '\r';
  }
LLVM_READNONE inline bool IsDigit(char c) { return c >= '0' && c <= '9'; }

LLVM_READNONE inline bool IsLetter(char c) {return (c >= 'a' && c <= 'z') || (c >='A'&&c <= 'Z');}

}

class Lexer;

class Token {
  friend class Lexer;
public:
  enum TokenKind:unsigned short{
    eoi,
    unknown,
    ident,
    number,
    comma,
    colon,
    plus,
    minus,
    star,
    slash,
    l_paren,
    r_paren,
    KW_with
  };


  void SetKind(TokenKind K) { kind = K; }
  void SetText(llvm::StringRef T) { Text = T; }

private:
  TokenKind kind;
  llvm::StringRef Text;

public:
  TokenKind GetKind()const { return kind;}

  llvm::StringRef GetText() const { return Text; }

  [[nodiscard]] bool Is(TokenKind K)const { return GetKind() == K; }
  template<TokenKind K>
  [[nodiscard]] bool Is()const { return GetKind() == K; }
  
  template<typename ...Ts>
  [[nodiscard]] bool IsOneOf(TokenKind K1,Ts ...ts)const {
      if constexpr  (sizeof...(ts) > 0) {
        return Is(K1) || IsOneOf(ts...);
      } else if constexpr (sizeof...(ts) == 0) {
        return Is(K1);
      }
      return false;
  }

};


class Lexer {
  const char *BufferStart;
  const char *BufferPtr;

public:
  Lexer(const llvm::StringRef& Buffer) {
    BufferStart = Buffer.begin();
    BufferPtr = BufferStart;
  }

  void Next(Token &token);

private:
  void FromToken(Token &Result, char const *TokEnd, Token::TokenKind Kind);
};
