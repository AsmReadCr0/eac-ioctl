#pragma once
#include "cstdint"
#include "ntdef.h"

inline UNICODE_STRING name;
inline UNICODE_STRING link;

typedef struct _read_invoke {
	INT32 process_id;
	ULONGLONG address;
	ULONGLONG buffer;
	ULONGLONG size;
} read_invoke, * pread_invoke;

typedef struct _write_invoke {
	INT32 process_id;
	ULONGLONG address;
	ULONGLONG buffer;
	ULONGLONG size;
} write_invoke, * pwrite_invoke;

typedef struct _image_invoke {
	INT32 process_id;
	ULONGLONG* address;
} image_invoke, * pimage_invoke;

typedef struct _eprocess_invoke {
	INT32 process_id;
	ULONGLONG* address;
} eprocess_invoke, * peprocess_invoke;

typedef struct _dtb_invoke {
	uint32_t        pid;
	uintptr_t       cr3;
} dtb_invoke, * pdtb_invoke;

typedef struct _translate_invoke {
	uintptr_t virtual_address;
	uintptr_t directory_base;
	void* physical_address;
} translate_invoke, * ptranslate_invoke;