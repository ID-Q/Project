/*************************************************************************
	> File Name: program_master.cpp
	> Author: 
	> Mail: 
	> Created Time: 一 10/12 17:08:24 2020
 ************************************************************************/

#include <iostream>
#include <cstdio>
#include "program_master.h"

MasterChain *MasterChain::single = nullptr;

MasterChain::MasterChain() {
    this->p = new ProgramMaster::IProgramFactory();
    this->p->next = new IFMaster::IIFFactory();
    this->p->next->next = new ExprMaster::IExprFactory();
    this->p->next->next->next = new PrintMaster::IPrintFactory();
}

MasterChain::~MasterChain() {
    IFactory *next = this->p;
    while (next) {
        this->p = next->next;
        delete next;
        next = this->p;
    }
}

int MasterChain::valid(pANTLR3_BASE_TREE tree, Parameters *param) {
    IFactory *f = this->p;
    while (f) {
        if (f->valid(tree)) {
            IMaster *m = f->creator(param);
            int ret = m->run(tree);
            delete m;
            return ret;
        }
        f = f->next;
    }
    MasterChain s;
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    cout << "Unhandled token: #" << tok->type << '\n';
    throw runtime_error("There is no Master could take over The Tree!");
    return -1;
}

int Parameters::getVal(string name) {
    if (this->memory.find(name) != this->memory.end())
        return this->memory[name];
    if (this->next == nullptr) {
        throw runtime_error(name + " is not a varable!");
        return -1;
    }
    return this->next->getVal(name);
}

int Parameters::setVal(string name, int val) {
    if (this->memory.find(name) != this->memory.end())
        return this->memory[name] = val;
    if (this->next == nullptr) {
        throw runtime_error(name + " is not a varable!");
        return -1;
    }
    return this->next->setVal(name, val);
}

int Parameters::addVar(string name) {
    if (this->memory.find(name) == this->memory.end()) {
        this->memory[name] = 0;
        return 1;
    }
    throw runtime_error(name + " re-declared!");
    return 0;
}

int ProgramMaster::IProgramFactory::valid(pANTLR3_BASE_TREE tree) {
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    if (!tok) return true;
    switch (tok->type) {
        case WHILE:
        case DOWHILE:
        case FOR:
        case BLOCK:
            return true;
    }
    return false;
}

IMaster *ProgramMaster::IProgramFactory::creator(Parameters *param) {
    return new ProgramMaster(param);
}

int ProgramMaster::run(pANTLR3_BASE_TREE tree) {
    // 增加作用域
    this->param = new Parameters(this->param);
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    int r = 0;
    if (!tok) {
        int k = tree->getChildCount(tree);
        for(int i = 0; i < k; i++) {
            r = MasterChain::get()->valid(getChild(tree, i), this->param);
        }
    } else {
        switch(tok->type) {
            case BLOCK: {
                int k = tree->getChildCount(tree);
                for (int i = 0; i < k; i++) {
                    r = MasterChain::get()->valid(getChild(tree, i), this->param);
                }
                break;
            }
            case WHILE: {
                while (MasterChain::get()->valid(getChild(tree, 0), this->param)) {
                    MasterChain::get()->valid(getChild(tree, 1), this->param);
                }
                break;
            }
            case DOWHILE: {
                do {
                    MasterChain::get()->valid(getChild(tree, 1), this->param);
                } while (MasterChain::get()->valid(getChild(tree, 0), this->param));
                break;
            }
            case FOR: {
                MasterChain::get()->valid(getChild(tree, 0), this->param);
                while (MasterChain::get()->valid(getChild(tree, 1), this->param)) {
                    MasterChain::get()->valid(getChild(tree, 3), this->param);
                    MasterChain::get()->valid(getChild(tree, 2), this->param);
                }
                break;
            }
            default: {
                cout << "Unhandled token: #" << tok->type << '\n';
                r = -1;
                break;
            }
        }
    }
    // 删除作用域
    Parameters *delete_param = this->param;
    this->param = this->param->next;
    delete delete_param;
    return r;
}

int IFMaster::IIFFactory::valid(pANTLR3_BASE_TREE tree) {
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    if (!tok) return false;
    switch (tok->type) {
        case IF:
        case ELSE:
            return true;
    }
    return false;
}

IMaster *IFMaster::IIFFactory::creator(Parameters *param) {
    return new IFMaster(param);
}

int IFMaster::run(pANTLR3_BASE_TREE tree) {
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    switch (tok->type) {
        case IF: {
            int ret = MasterChain::get()->valid(getChild(tree, 0), this->param);
            if (ret) {
                return MasterChain::get()->valid(getChild(tree, 1), this->param);
            } else if (tree->getChildCount(tree) == 3) {
                return MasterChain::get()->valid(getChild(tree, 2), this->param);
            }
        }
        case ELSE:
            return MasterChain::get()->valid(getChild(tree, 0), this->param);
        default :
            cout << "Unhandled token: #" << tok->type << '\n';
            break;
    }
    return 0;
}

int ExprMaster::IExprFactory::valid(pANTLR3_BASE_TREE tree) {
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    if (!tok) return false;
    switch (tok->type) {
        case DEF:
        case INT:
        case PLUS:
        case MINUS:
        case TIMES:
        case AND:
        case ASSIGN:
        case EQ:
        case GE:
        case GT:
        case LE:
        case LITTLE:
        case NE:
        case OR:
        case ID:
        case DIV:
        case MOD:
            return true;
    }
    return false;
}

IMaster *ExprMaster::IExprFactory::creator(Parameters *param) {
    return new ExprMaster(param);
}

int ExprMaster::run(pANTLR3_BASE_TREE tree) {
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    switch (tok->type) {
        case DEF: {
            int k = tree->getChildCount(tree);
            int val = 0;
            for (int i = 0; i < k; i++) {
                string var(getText(getChild(tree, i)));
                this->param->addVar(var);
                if (tree->getChildCount(getChild(tree, i))) {
                    val = MasterChain::get()->valid(getChild(getChild(tree, i), 0), this->param);
                    this->param->setVal(var, val);
                }
            }
            return val;
        }
        case INT: {
            const char *s = getText(tree);
            return (s[0] == '-' ? -atoi(s) : atoi(s));
        }
        case PLUS: {
            int a = MasterChain::get()->valid(getChild(tree, 0), this->param);
            int b = MasterChain::get()->valid(getChild(tree, 1), this->param);
            return a + b;
        }
        case MINUS: {
            int a = MasterChain::get()->valid(getChild(tree, 0), this->param);
            int b = MasterChain::get()->valid(getChild(tree, 1), this->param);
            return a - b;
        }
        case TIMES: {
            int a = MasterChain::get()->valid(getChild(tree, 0), this->param);
            int b = MasterChain::get()->valid(getChild(tree, 1), this->param);
            return a * b;
        }
        case DIV: {
            int a = MasterChain::get()->valid(getChild(tree, 0), this->param);
            int b = MasterChain::get()->valid(getChild(tree, 1), this->param);
            return a / b;
        }
        case MOD: {
            int a = MasterChain::get()->valid(getChild(tree, 0), this->param);
            int b = MasterChain::get()->valid(getChild(tree, 1), this->param);
            return a % b;
        }
        case AND: {
            int a = MasterChain::get()->valid(getChild(tree, 0), this->param);
            int b = MasterChain::get()->valid(getChild(tree, 1), this->param);
            return (a && b);
        }
        case ASSIGN: {
            string var(getText(getChild(tree, 0)));
            int val = MasterChain::get()->valid(getChild(tree, 1), this->param);
            this->param->setVal(var, val);
            return val;
        }
        case EQ: {
            int a = MasterChain::get()->valid(getChild(tree, 0), this->param);
            int b = MasterChain::get()->valid(getChild(tree, 1), this->param);
            return (a == b);
        }
        case GE: {
            int a = MasterChain::get()->valid(getChild(tree, 0), this->param);
            int b = MasterChain::get()->valid(getChild(tree, 1), this->param);
            return (a >= b);
        }
        case GT: {
            int a = MasterChain::get()->valid(getChild(tree, 0), this->param);
            int b = MasterChain::get()->valid(getChild(tree, 1), this->param);
            return (a > b);
        }
        case LE: {
            int a = MasterChain::get()->valid(getChild(tree, 0), this->param);
            int b = MasterChain::get()->valid(getChild(tree, 1), this->param);
            return (a <= b);
        }
        case LITTLE: {
            int a = MasterChain::get()->valid(getChild(tree, 0), this->param);
            int b = MasterChain::get()->valid(getChild(tree, 1), this->param);
            return (a < b);
        }
        case NE: {
            int a = MasterChain::get()->valid(getChild(tree, 0), this->param);
            int b = MasterChain::get()->valid(getChild(tree, 1), this->param);
            return (a != b);
        }
        case OR: {
            int a = MasterChain::get()->valid(getChild(tree, 0), this->param);
            int b = MasterChain::get()->valid(getChild(tree, 1), this->param);
            return (a || b);
        }
        case ID: {
            string var(getText(tree));
            return this->param->getVal(var);
        }
        default :
            cout << "Unhandled token: #" << tok->type << '\n';
            break;
    }
}

int PrintMaster::IPrintFactory::valid(pANTLR3_BASE_TREE tree) {
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    if (!tok) return false;
    switch (tok->type) {
        case PRINT:
            return true;
    }
    return false;
}

IMaster *PrintMaster::IPrintFactory::creator(Parameters *param) {
    return new PrintMaster(param);
}

int PrintMaster::run(pANTLR3_BASE_TREE tree) {
    pANTLR3_COMMON_TOKEN tok = tree->getToken(tree);
    switch (tok->type) {
        case PRINT: {
            int k = tree->getChildCount(tree);
            int ret = 0;
            for (int i = 0; i < k; i++) {
                if (i) printf(" ");
                int val = MasterChain::get()->valid(getChild(tree, i), this->param);
                ret += printf("%d", val);
            }
            ret += printf("\n");
            return ret;
        }
        default:
            cout << "Unhandled token: #" << tok->type << '\n';
            break;
    }
    return 0;
}

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE tree, unsigned i) {
    assert(i < tree->getChildCount(tree));
    return (pANTLR3_BASE_TREE) tree->getChild(tree, i);
}

const char* getText(pANTLR3_BASE_TREE tree) {
    return (const char*) tree->getText(tree)->chars;
}
