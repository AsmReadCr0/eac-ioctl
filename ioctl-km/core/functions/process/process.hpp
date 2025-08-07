#pragma once
#include "..\..\wdk.hpp"

namespace process
{
	void* get_section_base_address(INT32 pid);
	uintptr_t get_eprocess(INT32 pid);
	uintptr_t translate_virtual(uintptr_t dtb, uintptr_t va);

	ULONG64 find_minimum(INT32 g, SIZE_T f);
}