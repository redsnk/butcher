/void System__ReallocMem([^{]*{/a \
    uint64_t addr = _get_dword_ptr(_eax);\
    if (addr) {\
        _set_dword_ptr(_eax,realloc_mem(cpu,addr,_edx));\
    }\
    else {\
        _set_dword_ptr(_eax,alloc_mem(cpu,_edx));\
    }\
    return;
