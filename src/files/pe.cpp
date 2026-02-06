// https://learn.microsoft.com/en-us/windows/win32/debug/pe-format

#include "pe.hpp"

struct _PE *GetPE (char *name) {
FILE *f;
struct _PE *pe;
long l,lsections,lmax;
int b,n;
uint32_t align;

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
												pe->name = strdup(name);
												// Fix virtual size to SectionAlignment 
												align = (pe->pe32)?pe->Optional_Header_Windows_Specific_Fields.pe32.SectionAlignament:pe->Optional_Header_Windows_Specific_Fields.pe32plus.SectionAlignament;
												for (n=0;n<pe->num_sections;n++) {
													pe->Sections[n].VirtualSize = (((pe->Sections[n].VirtualSize)/align)*align)+align;
												}
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
						fprintf(stderr,"GetPE error: read signature\n");
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
		free(pe->name);
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

uint8_t *GetMemoryPE (struct _PE *pe, uint64_t addr, uint64_t size, uint64_t *read) {
int n;
uint8_t *m;
uint64_t base;

	for (n=0;n<pe->COFF_File_Header.NumberOfSections;n++) {
		base = GetImageBase(pe);
		m = GetMemoryFile(pe->f,addr,size,base+pe->Sections[n].VirtualAddress,pe->Sections[n].VirtualSize,pe->Sections[n].PointerToRawData,pe->Sections[n].SizeOfRawData,read);
        if (m != NULL) {
            return (m);
        }
		
	}
	return (NULL);
}

int GetImportFunctionPE (struct _PE *pe,uint64_t addr,char **lib, char **func) {
uint64_t read, *value, entry, iat, vad;
int b,m,n;
uint8_t *name,*name2;

	uint64_t base = GetImageBase(pe);
	struct _import_directory_table *table = (struct _import_directory_table *) GetMemoryPE (pe,base+pe->Directories[IMPORT_TABLE].VirtualAddress,pe->Directories[IMPORT_TABLE].Size,&read);
	if (table != NULL) {
		if (read == pe->Directories[IMPORT_TABLE].Size) {
			n = 0;
			while (table->entry[n].import_lookup_table) {
				name = GetMemoryPE (pe,base+table->entry[n].name,MAX_IMPORT_NAME-1,&read);
				if (name != NULL) {
					//printf("%s\n",name);
					m = 0;
					b = pe->pe32?4:8;
					iat = base+table->entry[n].import_address_table;
					while (true) {
						entry = iat+(m*b);
						value = (uint64_t *) GetMemoryPE (pe,entry,b,&read);
						if (value == NULL) {
							break;
						}
						if ((read != b) || (*value == 0)) {
							free(value);
							break;
						}
						if (entry == addr) {
							// found
							//strcpy (in->lib_name,(const char *) name);
							vad = (*value) & 0xffffffff;
							name2 = GetMemoryPE (pe,base+vad,MAX_IMPORT_NAME-1,&read);
							if (name2 != NULL) {
								//printf("%s\n",name2+2);
								//strcpy (in->func_name,(const char *)(func+2));
								*lib = strdup((char *)name);
								*func = strdup((char *) name2+2);
								free(name);
								free(name2);
								free(value);
								//free(name);
								free(table);
								return (true);
							}
							free(*lib);
						}
						free(value);
						m++;
					}
					free(name);	
				}
				n++;
			}
		}
		free(table);
	}
	return (false);
}
