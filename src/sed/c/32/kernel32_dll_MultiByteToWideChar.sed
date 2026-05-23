/void __kernel32_dll_MultiByteToWideChar([^{]*{/a \
    uint32_t in = _get_dword_ptr(_esp+8);\
    uint32_t len = _get_dword_ptr(_esp+12);\
    uint32_t out = _get_dword_ptr(_esp+16);\
    uint32_t olen = _get_dword_ptr(_esp+20);\
    if (olen) {\
        char *str = (char * )malloc(len+1);\
        get_mem (cpu,in,len,(uint8_t *) str);\
        str[len] = 0;\
        set_unicode_ptr(cpu,out,str);\
        free(str);\
    }\
    //_eax = (len+1)*2;\
    _eax = len;\
    _esp += 4*6;\
    return;
