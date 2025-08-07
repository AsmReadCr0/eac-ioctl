#pragma once
#include "..\..\wdk.hpp"
#include "..\..\..\dependencies\nt\nt.hpp"

namespace rwx
{
	bool read_phys_addr(PVOID addr, PVOID buffer, SIZE_T size, SIZE_T* bytes);

	bool write_phys_addr(PVOID addr, PVOID buffer, SIZE_T size, SIZE_T* bytes);
}