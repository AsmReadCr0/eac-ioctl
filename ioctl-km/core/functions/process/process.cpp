#include "process.hpp"
#include "..\..\..\dependencies\nt\nt.hpp"

#include "..\..\..\dependencies\ia32.hpp"

#include "..\rwx\rwx.hpp"

void* process::get_section_base_address(INT32 pid)
{
	PEPROCESS eprocess = 0;

	auto process = nt::ps_lookup_process_by_process_id((HANDLE)pid, &eprocess);

	auto section_base_address = nt::ps_get_process_section_base_address(eprocess); // eprocess + 0x520

	ObfDereferenceObject(eprocess);

	return section_base_address;
}

uintptr_t process::get_eprocess(INT32 pid)
{
    auto process_list_head = nt::ps_active_process_head();
    if (!process_list_head) {
        return 0;
    }

    const auto link_va =
        reinterpret_cast<addr_t>(process_list_head) -
        nt::ps_initial_system_process();
    if (!link_va) {
        return 0;
    }

    for (auto flink = process_list_head->Flink; flink; flink = flink->Flink)
    {
        const auto next_eprocess = reinterpret_cast<addr_t>(flink) - link_va;
        if (!next_eprocess) {
            continue;
        }

        const auto next_process_id = nt::ps_get_process_id(next_eprocess);
        if (next_process_id == pid) {
            return next_eprocess;
        }
    }
}

uintptr_t process::translate_virtual(uintptr_t dtb, uintptr_t va)
{
    struct a_cache {
        uintptr_t addr;
        MMPTE value;
    };

    a_cache cached_pml4e[512];
    a_cache cached_pdpte[512];
    a_cache cached_pde[512];
    a_cache cached_pte[512];

    virt_addr_t virtual_address{};

    virtual_address.value = PVOID(va);

    SIZE_T size = 0;

    if (cached_pml4e[virtual_address.pml4_index].addr != dtb + 8 * virtual_address.pml4_index || !cached_pml4e[virtual_address.pml4_index].value.u.Hard.Valid) {
        cached_pml4e[virtual_address.pml4_index].addr = dtb + 8 * virtual_address.pml4_index;
        rwx::read_phys_addr((PVOID)cached_pml4e[virtual_address.pml4_index].addr, reinterpret_cast<PVOID>(&cached_pml4e[virtual_address.pml4_index].value), 8, &size);
    }

    if (!cached_pml4e[virtual_address.pml4_index].value.u.Hard.Valid)
        return 0;

    if (cached_pdpte[virtual_address.pdpt_index].addr != (cached_pml4e[virtual_address.pml4_index].value.u.Hard.PageFrameNumber << 12) + 8 * virtual_address.pdpt_index || !cached_pdpte[virtual_address.pdpt_index].value.u.Hard.Valid) {
        cached_pdpte[virtual_address.pdpt_index].addr = (cached_pml4e[virtual_address.pml4_index].value.u.Hard.PageFrameNumber << 12) + 8 * virtual_address.pdpt_index;
        rwx::read_phys_addr((PVOID)cached_pdpte[virtual_address.pdpt_index].addr, reinterpret_cast<PVOID>(&cached_pdpte[virtual_address.pdpt_index].value), 8, &size);
    }

    if (!cached_pdpte[virtual_address.pdpt_index].value.u.Hard.Valid)
        return 0;

    if (cached_pde[virtual_address.pd_index].addr != (cached_pdpte[virtual_address.pdpt_index].value.u.Hard.PageFrameNumber << 12) + 8 * virtual_address.pd_index || !cached_pde[virtual_address.pd_index].value.u.Hard.Valid) {
        cached_pde[virtual_address.pd_index].addr = (cached_pdpte[virtual_address.pdpt_index].value.u.Hard.PageFrameNumber << 12) + 8 * virtual_address.pd_index;
        rwx::read_phys_addr((PVOID)cached_pde[virtual_address.pd_index].addr, reinterpret_cast<PVOID>(&cached_pde[virtual_address.pd_index].value), 8, &size);
    }

    if (!cached_pde[virtual_address.pd_index].value.u.Hard.Valid)
        return 0;

    if (cached_pte[virtual_address.pt_index].addr != (cached_pde[virtual_address.pd_index].value.u.Hard.PageFrameNumber << 12) + 8 * virtual_address.pt_index || !cached_pte[virtual_address.pt_index].value.u.Hard.Valid) {
        cached_pte[virtual_address.pt_index].addr = (cached_pde[virtual_address.pd_index].value.u.Hard.PageFrameNumber << 12) + 8 * virtual_address.pt_index;
        rwx::read_phys_addr((PVOID)cached_pte[virtual_address.pt_index].addr, reinterpret_cast<PVOID>(&cached_pte[virtual_address.pt_index].value), 8, &size);
    }

    if (!cached_pte[virtual_address.pt_index].value.u.Hard.Valid)
        return 0;

    return (cached_pte[virtual_address.pt_index].value.u.Hard.PageFrameNumber << 12) + virtual_address.offset;
}

ULONG64 process::find_minimum(INT32 g, SIZE_T f)
{
    INT32 h = (INT32)f;
    ULONG64 result = 0;

    return (((g) < (h)) ? (g) : (h));
}
