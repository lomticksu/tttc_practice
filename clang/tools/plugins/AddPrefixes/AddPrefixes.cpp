//
// Clang plugin that adds prefixes to variables based on their scope:
// global_, static_, local_, param_
//
//===----------------------------------------------------------------------===//

#include "clang/Frontend/FrontendPluginRegistry.h"
#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Sema/Sema.h"
#include "llvm/Support/raw_ostream.h"
using namespace clang;

namespace {

class AddPrefixesVisitor : public RecursiveASTVisitor<AddPrefixesVisitor> {
    ASTContext &Context;

public:
    AddPrefixesVisitor(ASTContext &Context) : Context(Context) {}

    bool VisitVarDecl(VarDecl *VD) {
        if (!VD->getIdentifier() || VD->getName().empty())
            return true;

        std::string Prefix;
        if (VD->isLocalVarDecl()) {
            Prefix = VD->isStaticLocal() ? "static_" : "local_";
        }
        else if (VD->isFileVarDecl()) {
            Prefix = VD->getStorageClass() == SC_Static ? "static_" : "global_";
        }

        if (!Prefix.empty()) {
            std::string NewName = Prefix + VD->getNameAsString();
            VD->setDeclName(&Context.Idents.get(NewName));
        }

        return true;
    }

    bool VisitParmVarDecl(ParmVarDecl *PVD) {
        if (!PVD->getIdentifier() || PVD->getName().empty())
            return true;

        std::string NewName = "param_" + PVD->getNameAsString();
        PVD->setDeclName(&Context.Idents.get(NewName));

        return true;
    }
};

class AddPrefixesConsumer : public ASTConsumer {
public:
    explicit AddPrefixesConsumer(CompilerInstance &Instance) {}

    void HandleTranslationUnit(ASTContext &Context) override {
        AddPrefixesVisitor Visitor(Context);
        Visitor.TraverseDecl(Context.getTranslationUnitDecl());
    }
};

class AddPrefixesAction : public PluginASTAction {
protected:
    std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance &CI,
                                                llvm::StringRef) override {
        return std::make_unique<AddPrefixesConsumer>(CI);
    }

    bool ParseArgs(const CompilerInstance &CI,
                const std::vector<std::string> &args) override {
        return true;
    }
};

} // namespace

static FrontendPluginRegistry::Add<AddPrefixesAction>
    X("add-prefixes", "add prefixes to variables");
