#include "Lexer.h"

int main()
{
    Token T;
    T.SetKind(Token::ident);
    bool bIs = T.IsOneOf(Token::ident, Token::comma);

    
    system("pause");
    return 0; 
}