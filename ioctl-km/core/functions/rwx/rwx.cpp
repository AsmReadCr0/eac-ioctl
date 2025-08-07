#include "rwx.hpp"

bool rwx::read_phys_addr(PVOID addr, PVOID buffer, SIZE_T size, SIZE_T* bytes)
{
	if (!addr || !buffer || !bytes) {
		return false;
	}

	if (size == 0) {
		*bytes = 0;
		return false;
	}

	MM_COPY_ADDRESS to_read = { 0 };

	to_read.PhysicalAddress.QuadPart = (LONGLONG)addr;

	nt::mm_copy_memory(buffer, to_read, size, 0x1, bytes);

	return true;
}

bool rwx::write_phys_addr(PVOID addr, PVOID buffer, SIZE_T size, SIZE_T* bytes)
{
	if (!addr || !buffer || !bytes)
		return false;

	if (size == 0) {
		*bytes = 0;
		return false;
	}

	PHYSICAL_ADDRESS to_write = { 0 };

	to_write.QuadPart = reinterpret_cast<LONGLONG>(addr);

	PVOID mapped_page = nt::map_io_space_ex(to_write, size, 0x04);
	if (!mapped_page)
		return false;

	RtlCopyMemory(mapped_page, buffer, size);
	*bytes = size;

	nt::mm_unmap_io_space(mapped_page, size);

	return true;
}
