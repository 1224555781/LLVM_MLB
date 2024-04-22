#pragma once
#include "../llvm-dwarfdump/llvm-dwarfdump.h"

#include "llvm/ADT/StringSet.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"


namespace MLB {
class AST;
class Expr;
class Factor;
class BinaryOperator;
class WithDecl;

class ASTVisitor {
public:
  virtual ~ASTVisitor() = default;
  virtual void Visit(AST&)=0;
  virtual void Visit(Expr&)=0;
  virtual void Visit(Factor&)=0;
  virtual void Visit(BinaryOperator&)=0;
  virtual void Visit(WithDecl&)=0;
};

class AST {
public:
  virtual ~AST()=default;
  virtual void Accept(ASTVisitor&)=0;
};

class Expr :public AST {
public:
  Expr() {
    
  }
};

class Factor : public Expr{
public:
  enum ValueKind {Ident,Number};

private:
  ValueKind Kind;
  llvm::StringRef Val;
public:
  Factor(ValueKind K, llvm::StringRef V):Kind(K),Val(V) {}
  ValueKind GetKind(){return Kind;}
  llvm::StringRef GetVal() {return Val;}
  virtual void Accept(ASTVisitor& V) override {
    V.Visit(*this);
  }
  
};

class BinaryOperator final : public Expr {
public:
  enum Operator {Plus,Minus,Mul,Div  };
private:
  Expr* Left;
  Expr* Right;
  Operator Op;

public:
  BinaryOperator(Operator O, Expr* L, Expr* R): Left(L), Right(R), Op(O) {}

  virtual void Accept(ASTVisitor &V) override {
    V.Visit(*this);
  }
  auto GetOp(){return Op;}
  Expr* GetLeft() {return Left;}
  Expr* GetRight() {return Right;}
};

class WithDecl : public  AST {
public:
  using StringVector = llvm::SmallVector<llvm::StringRef,8>;

  StringVector Vars;
  Expr* E;

public:
  WithDecl(StringVector Vars,Expr* E):Vars(Vars),E(E) {}
  auto begin() {
    return Vars.begin();
  }

  auto end() {
    return  Vars.end();
  }

  Expr* GetExpr() {
    return E;
  }
  virtual void Accept(ASTVisitor& visitor) override {
    visitor.Visit(*this);
  }
  
  
};

class Sema final {
public:
  bool Semantic(AST* Tree);
};

class DeclChecker : public ASTVisitor {
  llvm::StringSet<> Scope;
  unsigned char bHasError:1;

  enum ErrorType {Twice,Not };
  void ReportError(ErrorType ET,llvm::StringRef const& V) {
    llvm::errs()<<"Variable "<<V<<" "<<(ET==Twice?"already":"not")<<"declared\n";
    bHasError = true;
  }
public:
  DeclChecker():bHasError(false) {
  }

  bool HasError()const{return bHasError;}

  virtual void Visit(MLB::Factor& F) override
  {
    if (F.GetKind() == Factor::Ident) {
      if (Scope.find(F.GetVal())==Scope.end()) {
        ReportError(Not,F.GetVal());
      }
    }
  }

  virtual void Visit(BinaryOperator & Node) override {
    if (Node.GetLeft()) {
      Node.GetLeft()->Accept(*this);
    } else {
      bHasError = true;
    }
    if (Node.GetRight()) {
      Node.GetRight()->Accept((*this));
    } else {
      bHasError = true;
    }
  }

  virtual  void Visit(WithDecl& Node) override {
    for (auto I = Node.begin(),E = Node.end();I!=E;++I) {
      if (!Scope.insert(*I).second) {
        ReportError(Twice,*I);
      }
      if (Node.GetExpr()) {
        Node.GetExpr()->Accept(*this);
      } else {
        bHasError = true;
      }
    }
  }

  virtual void Visit(Expr &) override{}
  virtual void Visit(AST &) override{}
};

inline bool Sema::Semantic(AST *Tree) {
  DeclChecker DC;
  Tree->Accept(DC);
  return DC.HasError();
}


class CodeGen {
public:
  void Compile(AST* Tree);
};

class TOIRVisitor : public ASTVisitor {
  llvm::Module* M;
  llvm::IRBuilder<> Builder;
  llvm::Type* VoidTy;
  llvm::Type* Int32Ty;
  llvm::PointerType* PtrTy;
  llvm::Constant* Int32Zero;
  llvm::Value* V;
  llvm::StringMap<llvm::Value*> NameMap;

public:
  explicit TOIRVisitor(llvm::Module* M)
    : M(M), Builder(M->getContext()), V(nullptr) {
    VoidTy = Builder.getVoidTy();
    Int32Ty = Builder.getInt32Ty();
    PtrTy = llvm::PointerType::getUnqual(M->getContext());
    Int32Zero = Builder.getInt32(0);
  }
  void Run(AST* Tree);
  virtual void Visit(WithDecl &) override;
  virtual void Visit(Factor&) override;
  virtual void Visit(BinaryOperator &) override;
  virtual void Visit(Expr &) override{}
  virtual void Visit(AST &) override{}
};
}


