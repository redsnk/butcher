/def System__ReallocMem(/a \
    addr = cpu.get_dword_ptr(cpu._eax)\
    if addr != 0:\
        cpu.set_dword_ptr(cpu._eax,cpu.realloc_mem(addr,cpu._edx))\
    else:\
        cpu.set_dword_ptr(cpu._eax,cpu.alloc_mem(cpu._edx))\
    return
