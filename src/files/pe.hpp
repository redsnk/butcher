#ifndef _PE_H
#define _PE_H

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

struct _ms_dos_stub {
	unsigned char head[0x3c-1];
	int32_t off_signature;
};

struct _COFF_File_Header {
	uint16_t Machine;
	uint16_t NumberOfSections;
	uint32_t TimeDateStamp;
	uint32_t PointerToSymbolTable;
	uint32_t NumberOfSymbols;
	uint16_t SizeOfOptionalHeader;
	uint16_t Characteristics;
};

#define MAGIC_PE32		0x10b
#define MAGIC_PE32PLUS	0x20b

struct _Optional_Header_Standard_Fields {
	uint16_t Magic;
	uint8_t MajorLinkerVersion;
	uint8_t MinorLinkerVersion;
	uint32_t SizeOfCode;
	uint32_t SizeOfInitializedData;
	uint32_t SizeOfUninitializedData;
	uint32_t AddressOfEntryPoint;
	uint32_t BaseOfCode;
	// PE32 only
	uint32_t BaseOfData;
};

struct _Optional_Header_Windows_Specific_Fields_PE32 {
	uint32_t ImageBase;
	uint32_t SectionAlignament;
	uint32_t FileAlignament;
	uint16_t MajorOperartingSystemVersion;
	uint16_t MinorOperatingSystemVersion;
	uint16_t MajorImageVersion;
	uint16_t MinorImageVersion;
	uint16_t MajorSusbystemVersion;
	uint16_t MinorSubsystemVersion;
	uint32_t Win32VersionValue;
	uint32_t SizeOfImage;
	uint32_t SizeOfHeaders;
	uint32_t CheckSum;
	uint16_t Subsystem;
	uint16_t DllCharacteristics;
	uint32_t SizeOfStackReserve;
	uint32_t SizeOfStackCommit;
	uint32_t SizeOfHeapReserve;
	uint32_t SizeOfHeapCommit;
	uint32_t LoaderFlags;
	uint32_t NumberOfRvaAndSizes;
};

struct _Optional_Header_Windows_Specific_Fields_PE32PLUS {
	uint64_t ImageBase;
	uint32_t SectionAlignament;
	uint32_t FileAlignament;
	uint16_t MajorOperartingSystemVersion;
	uint16_t MinorOperatingSystemVersion;
	uint16_t MajorImageVersion;
	uint16_t MinorImageVersion;
	uint16_t MajorSusbystemVersion;
	uint16_t MinorSubsystemVersion;
	uint32_t Win32VersionValue;
	uint32_t SizeOfImage;
	uint32_t SizeOfHeaders;
	uint32_t CheckSum;
	uint16_t Subsystem;
	uint16_t DllCharacteristics;
	uint64_t SizeOfStackReserve;
	uint64_t SizeOfStackCommit;
	uint64_t SizeOfHeapReserve;
	uint64_t SizeOfHeapCommit;
	uint32_t LoaderFlags;
	uint32_t NumberOfRvaAndSizes;
};

union _Optional_Header_Windows_Specific_Fields {
	struct _Optional_Header_Windows_Specific_Fields_PE32 pe32;
	struct _Optional_Header_Windows_Specific_Fields_PE32PLUS pe32plus;
};

#define MAX_DATA_DIRECTORIES	256

struct _IMAGE_DATA_DIRECTORY {
	uint32_t VirtualAddress;
    uint32_t Size;
};

#define MAX_HEADERS		96	

struct _Section_Header {
	char Name[8];
	uint32_t VirtualSize;
	uint32_t VirtualAddress;
	uint32_t SizeOfRawData;
	uint32_t PointerToRawData;
	uint32_t PointerToRelocations;
	uint32_t PointerToLinenumbers;
	uint16_t NumberOfRelocations;
	uint16_t NumberOfLinenumbers;
	uint32_t Characteristics;
};

struct _PE {
	FILE *f;
	struct _ms_dos_stub ms_dos_stub;
	int32_t signature;
	struct _COFF_File_Header COFF_File_Header;
	struct _Optional_Header_Standard_Fields Optional_Header_Standard_Fields;
	int pe32;
	union _Optional_Header_Windows_Specific_Fields Optional_Header_Windows_Specific_Fields;
	int num_sections;
	struct _Section_Header Sections[MAX_HEADERS];
	int num_directories;
	struct _IMAGE_DATA_DIRECTORY Directories[MAX_DATA_DIRECTORIES];
};

struct _PE *GetPE (char *name);
void FreePE (struct _PE *pe);
uint8_t *GetMemoryPE (struct _PE *pe, int64_t addr, int64_t size);

#endif	// _PE_H
