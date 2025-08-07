#pragma once
#include "..\..\wdk.hpp"

/*

Credits: Interpreter - github.com/paidtoomuch/hv.sol-fortnite/tree/main/fortnite-driver

*/
namespace phys
{
	inline void* MmPfnDatabase;
	inline ULONGLONG DirectoryTableBase;

	PVOID split_memory(PVOID SearchBase, SIZE_T SearchSize, const void* Pattern, SIZE_T PatternSize);
	NTSTATUS get_mmpfndatabase();
	uintptr_t get_directory_table_base(void* process_base);
}