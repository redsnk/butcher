#ifndef _LANGUAGE_H
#define _LANGUAGE_H

#include "../code.hpp"

class Language {
    public:
    virtual void PrintHeader(Code *c) = 0;
    virtual void PrintMainOpen(Code *c) = 0;
    virtual void PrintMainClose(Code *c) = 0;
    virtual void PrintSubMem(Code *c,int num) = 0;
};

#endif // _LANGUAGE_H