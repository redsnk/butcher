/void __kernel32_dll_MultiByteToWideChar([^{]*{/a \
char *pcode = "CP1252";\
int l;\
char *str;\
\
    uint32_t code = _get_dword_ptr(_esp);\
    uint32_t flags = _get_dword_ptr(_esp+4);\
    uint32_t in = _get_dword_ptr(_esp+8);\
    int32_t len = _get_dword_ptr(_esp+12);\
    uint32_t out = _get_dword_ptr(_esp+16);\
    uint32_t olen = _get_dword_ptr(_esp+20);\
    if (len == -1) {\
        str = get_str_ptr(cpu,in);\
        len = strlen(str)+1;\
    }\
    else {\
        str = (char * )malloc(len+1);\
        get_mem (cpu,in,len,(uint8_t *) str);\
        str[len] = 0;\
    }\
    if (code == 65001) {\
        pcode = "UTF8";\
    }\
    if (olen == 0) {\
        out = 0;\
    }\
    l = set_unicode_ptr_code_len(cpu,out,str,pcode,len);\
    free(str);\
    _eax = l/2;\
    _esp += 4*6;\
    return;
