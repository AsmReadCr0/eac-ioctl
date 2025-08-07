#pragma once
#include "..\wdk.hpp"

#include "data/data.hpp"

namespace io
{
	NTSTATUS entrypoint(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path);

	NTSTATUS ioctl_handler(PDEVICE_OBJECT device_obj, PIRP irp);
	NTSTATUS unsupported_request(PDEVICE_OBJECT device_obj, PIRP irp);

	NTSTATUS get_section_base_address(pimage_invoke request);

	NTSTATUS read_memory(pread_invoke request);

	NTSTATUS write_memory(pwrite_invoke request);

	NTSTATUS translate_address(ptranslate_invoke request);

	UINT64 get_directory_table_base(pdtb_invoke request);

	NTSTATUS get_eprocess(peprocess_invoke request);
}
