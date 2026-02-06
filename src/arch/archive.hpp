#ifndef _ARCHIVE_H
#define _ARCHIVE_H

struct _Section {
    uint64_t d_Offset;
    uint64_t d_Size;
    uint64_t v_Address;
    uint64_t v_Size;
};

class Archive {
    public:
        virtual int CheckFile(char *file_name) = 0;
        virtual int OpenFile(char *file_name) = 0;
        virtual void CloseFile(void) = 0;
        virtual char *GetFileName(void) = 0;
        virtual int Is32(void) = 0;
        virtual uint8_t *GetMemory(uint64_t addr,uint64_t size, uint64_t *read) = 0;
        virtual int ValidMemory(uint64_t addr) = 0;
        virtual int IsImportFunction (uint64_t addr, char **lib, char **func) = 0;
        virtual int IsSymbolFunction (uint64_t addr, char **func) = 0;
        virtual int IsSymbolObject (uint64_t addr, char **name) = 0;
        virtual struct _Section *GetSections(int *count) = 0;
};

#endif // _ARCHIVE_H