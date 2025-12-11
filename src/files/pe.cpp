// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format

#include "pe.hpp"

struct _PE *GetPE (char *name) {
FILE *f;
struct _PE *pe;
long l,lsections,lmax;
int b;

	f = fopen(name,"r");
	if (f!= NULL) {
		pe = (struct _PE *) malloc(sizeof(struct _PE));
		if (pe != NULL) {
			// FILE
			pe->f = f;
			// ms_dos_stub
			l = fread(&pe->ms_dos_stub,1,sizeof(struct _ms_dos_stub),f);
			if (l == sizeof(struct _ms_dos_stub)) {
				// go to signature
				if (fseek(f,pe->ms_dos_stub.off_signature,SEEK_SET) != -1) {
					// signature
					l = fread(&pe->signature,1,4,f);
					if (l == 4) {
						if (pe->signature == 0x00004550) { //"PE\0\0"
							// COFF File Header
							l = fread(&pe->COFF_File_Header,1,sizeof(struct _COFF_File_Header),f);
							if (l == sizeof(struct _COFF_File_Header)) {
								pe->num_sections = pe->COFF_File_Header.NumberOfSections;
								lsections = ftell(f) + pe->COFF_File_Header.SizeOfOptionalHeader;
								// Optional Header Standard Fields
								l = fread(&pe->Optional_Header_Standard_Fields,1,sizeof(struct _Optional_Header_Standard_Fields)-4,f);
								if (l == sizeof(struct _Optional_Header_Standard_Fields)-4) {
									pe->pe32 = (pe->Optional_Header_Standard_Fields.Magic == MAGIC_PE32);
									if (pe->pe32) {
										l = fread(&pe->Optional_Header_Standard_Fields.BaseOfData,1,4,f);
										if (l != 4) {
											printf("GetPE error: fread _Optional_Header_Standard_Fields.BaseOfData\n");
											fclose(f);
											free(pe);
											return (NULL);
										}
									}
									// Optional Header Windows Specific Fields
									if (pe->pe32) {
										l = fread(&pe->Optional_Header_Windows_Specific_Fields.pe32,1,sizeof(struct _Optional_Header_Windows_Specific_Fields_PE32),f);
										b = (l == sizeof(struct _Optional_Header_Windows_Specific_Fields_PE32));
									} else {
										l = fread(&pe->Optional_Header_Windows_Specific_Fields.pe32plus,1,sizeof(struct _Optional_Header_Windows_Specific_Fields_PE32PLUS),f);
										b = (l == sizeof(struct _Optional_Header_Windows_Specific_Fields_PE32PLUS));
									}
									if (b) {
										// Directories
										pe->num_directories = (pe->pe32)?pe->Optional_Header_Windows_Specific_Fields.pe32.NumberOfRvaAndSizes:pe->Optional_Header_Windows_Specific_Fields.pe32plus.NumberOfRvaAndSizes;
										l = fread(&pe->Directories,1,sizeof(struct _IMAGE_DATA_DIRECTORY)*pe->num_directories,f);
										if (l == (sizeof(struct _IMAGE_DATA_DIRECTORY)*pe->num_directories)) {
											// sections
											fseek(f,lsections,SEEK_SET);
											lmax = pe->num_sections*sizeof(struct _Section_Header);
											l = fread(&pe->Sections,1,lmax,f);
											if (l == lmax) {
												return (pe);
											} else {
												printf("GetPE error: fread Sections\n");
											}
										} else {
											printf("GetPE error: fread _IMAGE_DATA_DIRECTORY\n");
										}
									} else {
										printf("GetPE error: fread _Optional_Header_Windows_Specific_Fields\n");
									}
								} else {
									printf("GetPE error: fread _Optional_Header_Standard_Fields\n");
								}
							} else {
								printf("GetPE error: fread _COFF_File_Header\n");
							}
						} else {
							printf("GetPE error: signature doesn't match\n");
						}
					} else {
						printf("GetPE error: read signature\n");
					}
				} else {
					printf("GetPE error: go to signature\n");
				}
			} else {
				printf("GetPE error: fread _ms_dos_stub\n");
			}
			free(pe);	
		} else {
			printf("GetPE error: fopen\n");
		}
		fclose(f);
	}
	return (NULL);
}

void FreePE (struct _PE *pe) {
	if (pe != NULL) {
		fclose(pe->f);
		free(pe);
	}
}

uint64_t GetImageBase (struct _PE *pe) {
	if (pe->Optional_Header_Standard_Fields.Magic == MAGIC_PE32) {
		return (pe->Optional_Header_Windows_Specific_Fields.pe32.ImageBase);
	}
	return (pe->Optional_Header_Windows_Specific_Fields.pe32plus.ImageBase);
}

uint8_t *GetMemoryPE (struct _PE *pe, int64_t addr, int64_t size) {
int n;
uint64_t base, start, end, offset;
uint8_t *m;
long l;

	for (n=0;n<pe->COFF_File_Header.NumberOfSections;n++) {
		base = GetImageBase(pe);
		start = base+pe->Sections[n].VirtualAddress;
		end = start+pe->Sections[n].VirtualSize;
		if ((addr >= start) && (addr < end) && (size <= (end-start))) {
			m = (uint8_t *) malloc(size);
			if (m != NULL) {
				// TODO: check SizeOfRawData overflow
				offset = pe->Sections[n].PointerToRawData+(addr-start);
				l = fseek(pe->f,offset,SEEK_SET);
				if (l != -1) {
					l = fread(m,1,size,pe->f);
					if (l == size) {
						return (m);
					} else {
						printf("GetMemoryPE error: fread\n");
					}
				} else {
					printf("GetMemoryPE error: fseek\n");
				}
				free(m);		
			}	
			break;
		}
	}
	return (NULL);
}
