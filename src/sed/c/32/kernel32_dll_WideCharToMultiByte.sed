/void __kernel32_dll_WideCharToMultiByte([^{]*{/a \
    uint32_t in = _get_dword_ptr(_esp+8);\
    uint32_t len = _get_dword_ptr(_esp+12);\
    uint32_t out = _get_dword_ptr(_esp+16);\
    uint32_t olen = _get_dword_ptr(_esp+20);\
    if (olen) {\
        char *p = get_unicode_ptr(cpu,in);\
        set_mem(cpu,out,len,p);\
        free(p);\
    }\
    _eax = len;\
    _esp += 4*8;\
    return;
