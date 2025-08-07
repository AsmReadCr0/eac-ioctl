#include "io.hpp"
#include "..\wdk.hpp"

#include "..\functions\process\process.hpp"
#include "..\functions\rwx\rwx.hpp"
#include "..\functions\phys\phys.hpp"

#include "..\..\dependencies\nt\nt.hpp"

#include "intrin.h"

#define code_read_phys CTL_CODE(FILE_DEVICE_UNKNOWN, 0xff13, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_write_phys CTL_CODE(FILE_DEVICE_UNKNOWN, 0xff14, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_base_address CTL_CODE(FILE_DEVICE_UNKNOWN, 0xff15, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_directory_table_base CTL_CODE(FILE_DEVICE_UNKNOWN, 0xff16, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_eprocess CTL_CODE(FILE_DEVICE_UNKNOWN, 0xff17, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#define code_translate CTL_CODE(FILE_DEVICE_UNKNOWN, 0xff18, METHOD_BUFFERED, FILE_SPECIAL_ACCESS)



NTSTATUS io::entrypoint(PDRIVER_OBJECT driver_object, PUNICODE_STRING registry_path)
{
	NTSTATUS status = STATUS_SUCCESS;

	PDEVICE_OBJECT device_object = 0;

	nt::rtl_init_unicode_string(&name, (L"\\Device\\GithubExample"));
	nt::rtl_init_unicode_string(&link, (L"\\DosDevices\\GithubExample"));

	status = nt::io_create_device(driver_object, 0, &name, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &device_object);
	if (!NT_SUCCESS(status))
	{
		return status;
	}

	status = nt::io_create_symbolic_link(&link, &name);

	if (!NT_SUCCESS(status))
	{
		nt::io_delete_device(device_object);
		return status;
	}

	for (int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
		driver_object->MajorFunction[i] = &unsupported_request;
	}

	device_object->Flags |= DO_BUFFERED_IO;

	driver_object->MajorFunction[IRP_MJ_CREATE] = &ioctl_handler;
	driver_object->MajorFunction[IRP_MJ_CLOSE] = &ioctl_handler;
	driver_object->MajorFunction[IRP_MJ_CLEANUP] = &ioctl_handler;

	driver_object->MajorFunction[IRP_MJ_DEVICE_CONTROL] = &ioctl_handler;
	driver_object->DriverUnload = 0;

	device_object->Flags &= ~DO_DEVICE_INITIALIZING;

	return status;
}

NTSTATUS io::ioctl_handler(PDEVICE_OBJECT device_obj, PIRP irp)
{
	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(irp);
    NTSTATUS status = { };

	ULONG code = stack->Parameters.DeviceIoControl.IoControlCode;
	ULONG size = stack->Parameters.DeviceIoControl.InputBufferLength;
    ULONG bytes = { };

	switch (stack->MajorFunction) {
	case IRP_MJ_CREATE:
		break;

	case IRP_MJ_CLOSE:
		break;

	case IRP_MJ_CLEANUP:
		break;

	case IRP_MJ_DEVICE_CONTROL:
		switch (code)
		{
		case code_read_phys:
			if (size == sizeof(_read_invoke)) {
				pread_invoke request = (pread_invoke)(irp->AssociatedIrp.SystemBuffer);

				status = read_memory(request);
				bytes = sizeof(_read_invoke);
			}
			else
			{
				status = STATUS_INFO_LENGTH_MISMATCH;
				bytes = 0;
			}

			break;

		case code_write_phys:
			if (size == sizeof(_write_invoke)) {
				pwrite_invoke request = (pwrite_invoke)(irp->AssociatedIrp.SystemBuffer);

				status = write_memory(request);
				bytes = sizeof(_write_invoke);
			}
			else
			{
				status = STATUS_INFO_LENGTH_MISMATCH;
				bytes = 0;
			}

			break;

		case code_base_address:
			if (size == sizeof(_image_invoke)) {
				pimage_invoke request = (pimage_invoke)(irp->AssociatedIrp.SystemBuffer);

				status = get_section_base_address(request);
				bytes = sizeof(_image_invoke);
			}
			else
			{
				status = STATUS_INFO_LENGTH_MISMATCH;
				bytes = 0;
			}

			break;

		case code_directory_table_base:
			if (size == sizeof(_dtb_invoke)) {
				pdtb_invoke request = (pdtb_invoke)(irp->AssociatedIrp.SystemBuffer);

				status = get_directory_table_base(request);
				bytes = sizeof(_dtb_invoke);
			}
			else
			{
				status = STATUS_INFO_LENGTH_MISMATCH;
				bytes = 0;
			}

			break;
		case code_eprocess:
			if (size == sizeof(_eprocess_invoke)) {
				peprocess_invoke request = (peprocess_invoke)(irp->AssociatedIrp.SystemBuffer);

				status = get_eprocess(request);
				bytes = sizeof(_eprocess_invoke);
			}
			else
			{
				status = STATUS_INFO_LENGTH_MISMATCH;
				bytes = 0;
			}
			break;

		case code_translate:
			if (size == sizeof(_translate_invoke)) {
				ptranslate_invoke request = (ptranslate_invoke)(irp->AssociatedIrp.SystemBuffer);

				status = translate_address(request);
				bytes = sizeof(_translate_invoke);
			}
			else
			{
				status = STATUS_INFO_LENGTH_MISMATCH;
				bytes = 0;
			}
			break;
		default:
			status = STATUS_INVALID_DEVICE_REQUEST;
			break;
		}
	}

    irp->IoStatus.Status = status;
    irp->IoStatus.Information = bytes;
    IofCompleteRequest(irp, IO_NO_INCREMENT);

    return status;
}

NTSTATUS io::unsupported_request(PDEVICE_OBJECT device_obj, PIRP irp)
{
	UNREFERENCED_PARAMETER(device_obj);
	irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
	IofCompleteRequest(irp, IO_NO_INCREMENT);
	return irp->IoStatus.Status;
}

NTSTATUS io::get_section_base_address(pimage_invoke request)
{
	if (!request->process_id)
		return STATUS_UNSUCCESSFUL;

	auto section_base_address = process::get_section_base_address(request->process_id);

	memcpy(request->address, &section_base_address, sizeof(section_base_address));
	ObfDereferenceObject(section_base_address);

	return STATUS_SUCCESS;
}

NTSTATUS io::read_memory(pread_invoke request)
{
	if (!request->process_id || !request->address)
		return STATUS_UNSUCCESSFUL;

	PEPROCESS process = NULL;
	nt::ps_lookup_process_by_process_id((HANDLE)request->process_id, &process);
	if (!process)
		return STATUS_UNSUCCESSFUL;

	ULONGLONG process_base = phys::get_directory_table_base(process);

	ObfReferenceObject(process);

	INT64 physical_address = process::translate_virtual(process_base, (ULONG64)request->address);

	ULONG64 final_size = process::find_minimum(0x1000 - (physical_address & 0xFFF), request->size);
	SIZE_T bytes_returned = NULL;

	rwx::read_phys_addr(PVOID(physical_address), (PVOID)((ULONG64)request->buffer), final_size, &bytes_returned);
	return STATUS_SUCCESS;
	
}

NTSTATUS io::write_memory(pwrite_invoke request)
{
	if (!request->process_id)
		return STATUS_UNSUCCESSFUL;

	PEPROCESS process = NULL;
	nt::ps_lookup_process_by_process_id((HANDLE)request->process_id, &process);

	ULONGLONG process_base = phys::get_directory_table_base(process);

	ObfReferenceObject(process);

	INT64 physical_address = process::translate_virtual(process_base, (ULONG64)request->address);

	if (!physical_address)
		return STATUS_UNSUCCESSFUL;

	ULONG64 final_size = process::find_minimum(0x1000 - (physical_address & 0xFFF), request->size);
	SIZE_T bytes_returned = NULL;

	rwx::write_phys_addr(PVOID(physical_address), (PVOID)((ULONG64)request->buffer), final_size, &bytes_returned);

	return STATUS_SUCCESS;
}

NTSTATUS io::translate_address(ptranslate_invoke request)
{
	if (!request->virtual_address || !request->directory_base)
		return STATUS_UNSUCCESSFUL;

	auto physical_address = process::translate_virtual(request->directory_base, request->virtual_address);
	if (!physical_address)
		return STATUS_UNSUCCESSFUL;

	memcpy(request->physical_address, &physical_address, sizeof(physical_address));

	return STATUS_SUCCESS;
}

UINT64 io::get_directory_table_base(pdtb_invoke request)
{
	if (!request->pid) {
		return STATUS_UNSUCCESSFUL;
	}

	PEPROCESS eprocess = 0;
	PsLookupProcessByProcessId((HANDLE)request->pid, &eprocess);

	if (!eprocess) {
		return STATUS_UNSUCCESSFUL;
	}

	phys::DirectoryTableBase = phys::get_directory_table_base((void*)nt::ps_get_process_section_base_address(eprocess));
	ObfDereferenceObject(eprocess);

	RtlCopyMemory((void*)request->cr3, &phys::DirectoryTableBase, sizeof(phys::DirectoryTableBase));

	return STATUS_SUCCESS;
}

NTSTATUS io::get_eprocess(peprocess_invoke request)
{
	if (!request->process_id)
		return STATUS_UNSUCCESSFUL;

	auto eprocess = process::get_eprocess(request->process_id);
	if (!eprocess)
		return STATUS_UNSUCCESSFUL;

	memcpy(request->address, &eprocess, sizeof(eprocess));

	return STATUS_SUCCESS;

	return NTSTATUS();
}
