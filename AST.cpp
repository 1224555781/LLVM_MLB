#include "AST.h"
namespace MLB {
void CodeGen::Compile(AST *Tree) {
  llvm::LLVMContext Ctx;
  llvm::Module* M = new llvm::Module("calc.expr",Ctx);
  TOIRVisitor ToIR(M);
  ToIR.Run(Tree);
  M->print(llvm::outs(),nullptr);
}

void TOIRVisitor::Run(AST *Tree) {
  llvm::FunctionType* MainFty = llvm::FunctionType::get(llvm::Type::getVoidTy(M->getContext()), false);
  llvm::Function* MainFunction = llvm::Function::Create(MainFty, llvm::Function::ExternalLinkage, "main", M);
  llvm::BasicBlock* Entry = llvm::BasicBlock::Create(M->getContext(), "entry", MainFunction);
  Builder.SetInsertPoint(Entry);
  Tree->Accept(*this);
  Builder.CreateRetVoid();
}

void TOIRVisitor::Visit(WithDecl& Node) {
  llvm::FunctionType* ReadTy = llvm::FunctionType::get(Int32Ty, (PtrTy),false);
  llvm::Function* ReadFn = llvm::Function::Create(ReadTy, llvm::Function::ExternalLinkage, "read", M);
  for (auto I = Node.begin(),E=Node.end();I!=E;++I) {
    llvm::StringRef Var = *I;
    llvm::Constant* StrText = llvm::ConstantDataArray::getString(M->getContext(),Var);
    llvm::GlobalVariable* Str = new llvm::GlobalVariable(*M, StrText->getType(), true, llvm::GlobalValue::PrivateLinkage, StrText, llvm::Twine(Var).concat(".str"));
    llvm::CallInst* Call = Builder.CreateCall(ReadTy,ReadFn, Str);
    NameMap[Var] = Call;
  }
  Node.GetExpr()->Accept(*this);
}

void TOIRVisitor::Visit(Factor& Node) {
  if (Node.GetKind() == Factor::Ident) {
    V = NameMap[Node.GetVal()];
  } else {
    int IntVal;
    Node.GetVal().getAsInteger(10,IntVal);
    V = llvm::ConstantInt::get(Int32Ty,IntVal,true);
  }
}

void TOIRVisitor::Visit(BinaryOperator& Node) {
  Node.GetLeft()->Accept(*this);
  llvm::Value* Left = V;
  Node.GetRight()->Accept(*this);
  llvm::Value* Right = V;
  switch (Node.GetOp()) {
  case BinaryOperator::Plus:
    V = Builder.CreateNSWAdd(Left,Right);
    break;
  case BinaryOperator::Minus:
    V= Builder.CreateNSWSub(Left,Right);
    break;
  case BinaryOperator::Mul:
    V = Builder.CreateNSWMul(Left,Right);
    break;
  case BinaryOperator::Div:
    V = Builder.CreateSDiv(Left,Right);
    break;
  }
}

}
