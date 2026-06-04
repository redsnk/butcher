/def __kernel32_dll_WideCharToMultiByte(/a \
    cin = cpu.get_dword_ptr(cpu._esp+8)\
    len = cpu.get_dword_ptr(cpu._esp+12)\
    out = cpu.get_dword_ptr(cpu._esp+16)\
    olen = cpu.get_dword_ptr(cpu._esp+20)\
    if olen != 0:\
        p = cpu.get_unicode_ptr(cin,len)\
        cpu.set_mem(out,p)\
    cpu._eax = len\
    cpu._esp += 4*8\
    return
