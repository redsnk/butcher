/def __kernel32_dll_WideCharToMultiByte(/a \
    in = cpu.get_dword_ptr(_esp+8)\
    len = cpu.get_dword_ptr(_esp+12)\
    out = cpu.get_dword_ptr(_esp+16)\
    olen = cpu.get_dword_ptr(_esp+20)\
    if olen != 0:\
        p = cpu.get_unicode_ptr(in)\
        cpu.set_mem(out,len,p)\
    cpu._eax = len\
    cpu._esp += 4*8\
    return
