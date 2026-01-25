#ifndef _LANG_C_H
#define _LANG_C_H
#include "language.hpp"

class Lang_C : public Language {
    public:
    void PrintHeader(Code *c);
    void PrintMainOpen(Code *c);
    void PrintMainClose(Code *c);
    void PrintSubMem(Code *c,int num);
};

#endif // _LANG_C_H