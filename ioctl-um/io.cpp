#include "io.hpp"
#include "tlhelp32.h"

namespace n_interface
{
	bool c_interface::create_handle()
	{
		g_handle = CreateFileW((L"\\\\.\\GithubExample"), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, 3, 0, 0);

		if ((g_handle == INVALID_HANDLE_VALUE))
		{
			std::printf(("failed to find driver image allocation"));
			std::cin.get();
			exit(0);
			return false;
		}
	}

	bool c_interface::read_physical(PVOID address, PVOID buffer, DWORD size) {
		_read_invoke data = { 0 };

		data.address = (ULONGLONG)address;
		data.buffer = (ULONGLONG)buffer;
		data.size = size;
		data.process_id = this->g_pid;

		DeviceIoControl(this->g_handle, code_read_phys, &data, sizeof(data), nullptr, NULL, NULL, NULL);

		return true;
	}

	bool c_interface::write_physical(PVOID address, PVOID buffer, DWORD size) {
		_write_invoke data = { 0 };

		data.address = (ULONGLONG)address;
		data.buffer = (ULONGLONG)buffer;
		data.size = size;
		data.process_id = this->g_pid;

		DeviceIoControl(this->g_handle, code_write_phys, &data, sizeof(data), nullptr, NULL, NULL, NULL);

		return true;
	}

	uintptr_t c_interface::get_eprocess(INT32 pid)
	{
		_eprocess_invoke data = { 0 };

		data.process_id = this->g_pid;
		data.address = &this->g_eprocess;

		DeviceIoControl(this->g_handle, code_eprocess, &data, sizeof(data), nullptr, NULL, NULL, NULL);

		return g_eprocess;
	}

	uintptr_t c_interface::get_directory_table_base(INT32 pid) {
		_dtb_invoke data = { 0 };

		data.pid = this->g_pid;
		data.cr3 = (uintptr_t)&this->g_pml4e;

		DeviceIoControl(this->g_handle, code_directory_table_base, &data, sizeof(data), nullptr, NULL, NULL, NULL);

		return g_pml4e;
	}

	uintptr_t c_interface::get_image_base(INT32 pid) {
		_image_invoke data = { NULL };

		data.process_id = this->g_pid;
		data.address = (ULONGLONG*)&this->g_image_base;

		DeviceIoControl(this->g_handle, code_base_address, &data, sizeof(data), nullptr, NULL, NULL, NULL);

		return g_image_base;
	}

	auto c_interface::attach_vm(const std::wstring& proc_name) -> bool
	{
		if (proc_name.empty()) {
			std::printf(("[-] invalid process name.\n"));
			std::cin.get();
			return false;
		}

		g_pid = this->get_process_id(proc_name.c_str());
		if (!g_pid) {
			std::printf(("[-] invalid process\n"));
			std::cin.get();
		}

		auto communication_handler = this->create_handle();

		if (!communication_handler) {
			std::printf(("[-] failed to create communication handler.\n"));
			std::cin.get();
			return false;
		}

		g_eprocess = this->get_eprocess(g_pid);

		g_image_base = this->get_image_base(g_pid);
		if (!g_image_base) {
			std::printf(("[-] invalid image_base. %I64d\n"), g_image_base);
			std::cin.get();
		}

		g_pml4e = this->get_directory_table_base(g_pid);
		if (!g_pml4e) {
			std::printf(("[-] invalid cr3\n"));
			std::cin.get();
		}

		printf("[+] image base -> 0x%llx\n", g_image_base);
		printf("[+] pml4e -> 0x%llx\n", g_pml4e);

		return true;

	}

	INT32 c_interface::get_process_id(const std::wstring& proc_name)
	{
		PROCESSENTRY32W proc_info;
		proc_info.dwSize = sizeof(proc_info);

		HANDLE proc_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (proc_snapshot == INVALID_HANDLE_VALUE) {
			return 0;
		}

		Process32FirstW(proc_snapshot, &proc_info);
		if (!wcscmp(proc_info.szExeFile, proc_name.c_str())) {
			CloseHandle(proc_snapshot);
			return proc_info.th32ProcessID;
		}

		while (Process32NextW(proc_snapshot, &proc_info)) {
			if (!wcscmp(proc_info.szExeFile, proc_name.c_str())) {
				CloseHandle(proc_snapshot);
				return proc_info.th32ProcessID;
			}
		}

		CloseHandle(proc_snapshot);
		return 0;
	}

	
	
}