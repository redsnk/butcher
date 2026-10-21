/void __kernel32_dll_WideCharToMultiByte([^{]*{/a \
char *pcode = "CP1252";\
int size;\
\
    uint32_t code = _get_dword_ptr(_esp);\
    uint32_t flags = _get_dword_ptr(_esp+4);\
    uint32_t in = _get_dword_ptr(_esp+8);\
    int32_t len = _get_dword_ptr(_esp+12);\
    uint32_t out = _get_dword_ptr(_esp+16);\
    uint32_t olen = _get_dword_ptr(_esp+20);\
    if (code == 65001) {\
        pcode = "UTF8";\
    }\
    if (len == -1) {\
        len = get_unicode_length(cpu,in);\
    }\
    char *p = get_unicode_ptr_code_len(cpu,in,pcode,len*2,&size);\
    if (olen) {\
        set_mem(cpu,out,olen,p);\
    }\
    free(p);\
    _eax = size;\
    _esp += 4*8;\
    return;
