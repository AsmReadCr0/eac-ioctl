#include "core/wdk.hpp"

#include "dependencies/nt/nt.hpp"
#include "core/io/io.hpp"
#include "core/driver/driver.hpp"


NTSTATUS DriverEntry(void* image, void* size)
{
	if (!image || !size)
	{
		nt::dbg_print("Failed passing Parameters\n");
	}
	nt::dbg_print("ioctl example - github.com/AsmReadCr0\n");

	if (!ioctl::create_driver(io::entrypoint))
	{
		nt::dbg_print("Failed to Manually Create DriverObject!\n");
	}

	nt::dbg_print("Successfully launched!\n");

	return STATUS_SUCCESS;
}