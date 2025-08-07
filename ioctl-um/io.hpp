#pragma once
#include "windows.h"
#include "iostream"
#include "vector"

namespace n_interface
{
	class c_interface {
	private:
		HANDLE g_handle;
		INT32 g_pid;
		uintptr_t g_pml4e;
		uintptr_t g_image_base;
		uintptr_t g_eprocess;
	public:
		bool create_handle();
		bool read_physical(PVOID address, PVOID buffer, DWORD size);
		bool write_physical(PVOID address, PVOID buffer, DWORD size);
		uintptr_t get_eprocess(INT32 pid);
		uintptr_t get_directory_table_base(INT32 pid);
		uintptr_t get_image_base(INT32 pid);

		auto attach_vm(const std::wstring& proc_name) -> bool;

		INT32 get_process_id(const std::wstring& proc_name);

		template <typename T>
		T read(uint64_t address) {
			T buffer{ };
			this->read_physical((PVOID)address, &buffer, sizeof(T));
			return buffer;
		}

		template <typename T>
		T write(uint64_t address, T buffer) {
			this->write_physical((PVOID)address, &buffer, sizeof(T));
			return buffer;
		}

	public:
		#define code_read_phys CTL_CODE(FILE_DEVICE_UNKNOWN, 0xff13, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
		#define code_write_phys CTL_CODE(FILE_DEVICE_UNKNOWN, 0xff14, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
		#define code_base_address CTL_CODE(FILE_DEVICE_UNKNOWN, 0xff15, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
		#define code_eprocess CTL_CODE(FILE_DEVICE_UNKNOWN, 0xff17, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
		#define code_directory_table_base CTL_CODE(FILE_DEVICE_UNKNOWN, 0xff16, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
	private:
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

		typedef struct _translate_invoke {
			uintptr_t virtual_address;
			uintptr_t directory_base;
			void* physical_address;
		} translate_invoke, * ptranslate_invoke;

		typedef struct _dtb_invoke {
			uint32_t        pid;
			uintptr_t       cr3;
		} dtb_invoke, * pdtb_invoke;

	};	
} inline auto io = std::make_unique<n_interface::c_interface>();
