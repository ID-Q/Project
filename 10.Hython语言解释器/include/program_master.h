/*************************************************************************
	> File Name: program_master.h
	> Author: 
	> Mail: 
	> Created Time: 一 10/12 17:07:09 2020
 ************************************************************************/

#ifndef _PROGRAM_MASTER_H
#define _PROGRAM_MASTER_H

#include "hlLexer.h"
#include "hlParser.h"
#include <cassert>
#include <map>
#include <string>
#include <iostream>
#include <stdexcept>

using std::map;
using std::string;
using std::cout;
using std::runtime_error;

class MasterChain;
class Parameters;

class IMaster;
class ProgramMaster;
class IFMaster;
class ExprMaster;
class IProgramFactory;
class IIFFactory;
class IExprFactory;

class IFactory {
public:
    virtual int valid(pANTLR3_BASE_TREE) = 0;
    virtual IMaster *creator(Parameters *) = 0;
    IFactory *next;
    virtual ~IFactory() {}
};

class MasterChain {
public :
    static MasterChain *get() {
        if (MasterChain::single) return MasterChain::single;
        MasterChain::single = new MasterChain();
        return MasterChain::single;
    }
    static void destroy() {
        if (MasterChain::single == nullptr) return ;
        MasterChain::get()->~MasterChain();
        MasterChain::single = nullptr;
    }
    int valid(pANTLR3_BASE_TREE, Parameters *);
private:
    static MasterChain *single;
    MasterChain();
    ~MasterChain();
    IFactory *p;
};

class Parameters {
public :
    Parameters(Parameters *next) : next(next) {}
    int getVal(string);
    int setVal(string, int);
    int addVar(string);
    Parameters *next;
private:
    map<string, int> memory;
};

class IMaster {
public:
    IMaster(Parameters *param) : param(param) {}
    virtual ~IMaster() {}
    virtual int run(pANTLR3_BASE_TREE) = 0;
protected:
    Parameters *param;
};

class ProgramMaster : public IMaster {
public:
    class IProgramFactory : public IFactory {
    public :
        int valid(pANTLR3_BASE_TREE);
        IMaster *creator(Parameters *);
    };
    ProgramMaster() : IMaster(nullptr) {}
    ProgramMaster(Parameters *param) : IMaster(param) {}
    int run(pANTLR3_BASE_TREE);
};

class IFMaster : public IMaster {
public:
    class IIFFactory : public IFactory {
    public :
        int valid(pANTLR3_BASE_TREE);
        IMaster *creator(Parameters *);
    };
    IFMaster() : IMaster(nullptr) {}
    IFMaster(Parameters *param) : IMaster(param) {}
    int run(pANTLR3_BASE_TREE);
};

class ExprMaster : public IMaster {
public:
    class IExprFactory : public IFactory {
    public :
        int valid(pANTLR3_BASE_TREE);
        IMaster *creator(Parameters *);
    };
    ExprMaster() : IMaster(nullptr) {}
    ExprMaster(Parameters *param) : IMaster(param) {}
    int run(pANTLR3_BASE_TREE);
};

class PrintMaster : public IMaster {
public:
    class IPrintFactory : public IFactory {
    public :
        int valid(pANTLR3_BASE_TREE);
        IMaster *creator(Parameters *);
    };
    PrintMaster() : IMaster(nullptr) {}
    PrintMaster(Parameters *param) : IMaster(param) {}
    int run(pANTLR3_BASE_TREE);
};

pANTLR3_BASE_TREE getChild(pANTLR3_BASE_TREE, unsigned);
const char* getText(pANTLR3_BASE_TREE tree);

#endif
