
#include "AST.h"
#include "Parser.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/InitLLVM.h"

static llvm::cl::opt<std::string> Input(llvm::cl::Positional,llvm::cl::desc("input expression"),llvm::cl::init(""));

int main(int argc,const char ** argv) {
  llvm::InitLLVM X(argc,argv);
  llvm::cl::ParseCommandLineOptions(argc,argv,"calc -theexpression compiler\n");

  MLB::Lexer lex(Input);
  Parser parse(lex);
  MLB::AST* Tree = parse.Parse();
  if (!Tree || parse.HasError()) {
    llvm::errs()<<"Syntax errors occured\n";
    return 1;
  }
  MLB::Sema seme;
  if (seme.Semantic(Tree)) {
    llvm::errs()<<"Semantic errors occured\n";
    return 1;
  }
  MLB::CodeGen cg;
  cg.Compile(Tree);
  return 0;
}