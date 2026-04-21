/def System__GetMem(/a \
    cpu._eax = cpu.alloc_mem(cpu._eax)\
    return
