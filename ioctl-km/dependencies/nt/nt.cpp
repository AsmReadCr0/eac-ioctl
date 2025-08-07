#include "nt.hpp"

uintptr_t resolve_address(
    uintptr_t Instruction,
    ULONG OffsetOffset,
    ULONG InstructionSize)
{
    LONG RipOffset = *(PLONG)(Instruction + OffsetOffset);
    auto ResolvedAddr = (
        Instruction +
        InstructionSize +
        RipOffset);

    return ResolvedAddr;
}

uintptr_t get_ntoskrnl()
{
    typedef unsigned char uint8_t;
    auto Idt_base = reinterpret_cast<uintptr_t>(KeGetPcr()->IdtBase);
    auto align_page = *reinterpret_cast<uintptr_t*>(Idt_base + 4) >> 0xc << 0xc;

    for (; align_page; align_page -= PAGE_SIZE)
    {
        for (int index = 0; index < PAGE_SIZE - 0x7; index++)
        {
            auto current_address = static_cast<intptr_t>(align_page) + index;

            if (*reinterpret_cast<uint8_t*>(current_address) == 0x48
                && *reinterpret_cast<uint8_t*>(current_address + 1) == 0x8D
                && *reinterpret_cast<uint8_t*>(current_address + 2) == 0x1D
                && *reinterpret_cast<uint8_t*>(current_address + 6) == 0xFF)
            {
                auto Ntosbase = resolve_address(current_address, 3, 7);
                if (!((UINT64)Ntosbase & 0xfff))
                {
                    return Ntosbase;
                }
            }
        }
    }
    return 0;
}

namespace nt
{
    template< class type_t >
    type_t find_export(const char* export_name)
    {
        auto ntoskrnl = get_ntoskrnl();

        auto dos_header = reinterpret_cast<dos_header_t*>(ntoskrnl);
        if (!dos_header->is_valid()) {
        }

        auto m_nt_header = reinterpret_cast<nt_headers_t*>(reinterpret_cast<uint64_t>(dos_header) + dos_header->m_lfanew);
        if (!m_nt_header->is_valid()) {
        }

        auto library{ reinterpret_cast<int8_t*>(dos_header) };
        auto export_directory =
            reinterpret_cast<export_directory_t*> (ntoskrnl + m_nt_header->m_export_table.m_virtual_address);
        if (!export_directory->m_address_of_functions
            || !export_directory->m_address_of_names
            || !export_directory->m_address_of_names_ordinals)
            return {};

        auto names{ reinterpret_cast<int32_t*>(library + export_directory->m_address_of_names) };
        auto functions{ reinterpret_cast<int32_t*>(library + export_directory->m_address_of_functions) };
        auto ordinals{ reinterpret_cast<int16_t*>(library + export_directory->m_address_of_names_ordinals) };

        for (int32_t i = 0; i < export_directory->m_number_of_names; i++) {
            auto current_name{ library + names[i] };
            auto current_function{ library + functions[ordinals[i]] };

            if (!strcmp(export_name, reinterpret_cast<char*>(current_name)))
                return reinterpret_cast<type_t>(current_function);
        }

        return reinterpret_cast<type_t>(0);
    }

    void mm_unmap_io_space(
        void* base_address,
        SIZE_T number_of_bytes
    ) {
        auto function_address = find_export<addr_t>(("MmUnmapIoSpace"));
        if (!function_address) {
            return;
        }

        using function_t = void(
            void* base_address,
            SIZE_T number_of_bytes
            );

        reinterpret_cast<function_t*>(function_address) (
            base_address,
            number_of_bytes);
    }

    NTSTATUS ps_lookup_process_by_process_id(
        HANDLE pid,
        PEPROCESS* process
    ) {
        auto function_address = find_export<addr_t>(("PsLookupProcessByProcessId"));
        if (!function_address) {
            return {};
        }

        using function_t = NTSTATUS(
            HANDLE pid,
            PEPROCESS* process
        );

        return reinterpret_cast<function_t*>(function_address)(pid,
            process);
    }

    LIST_ENTRY* ps_active_process_head()
    {
        static auto function_address = find_export<addr_t>(("KeCapturePersistentThreadState"));
        if (!function_address) {
            return { };
        }

        while (function_address[0x0] != 0x20
            || function_address[0x1] != 0x48
            || function_address[0x2] != 0x8d)
            function_address++;

        return *reinterpret_cast<LIST_ENTRY**>
            (&function_address[0x8] + *reinterpret_cast<std::int32_t*>(&function_address[0x4]));
    }

    std::uintptr_t ps_initial_system_process()
    {
        auto function_address = find_export<addr_t>(("PsInitialSystemProcess"));
        if (!function_address) {
            return {};
        }

        return *reinterpret_cast<uintptr_t*>(function_address);
    }

    std::uint32_t ps_get_process_id(std::uint64_t process)
    {
        auto function_address = find_export<addr_t>(("PsGetProcessId"));
        if (!function_address) {
            return {};
        }

        using function_t = HANDLE(
            PEPROCESS process
        );

        return reinterpret_cast<uint32_t>(
            reinterpret_cast<function_t*>(function_address)(
                reinterpret_cast<PEPROCESS>(process))
            );
    }

    void* ex_allocate_pool(
        POOL_TYPE pool_type,
        SIZE_T number_of_bytes
    ) {
        auto function_address = find_export<addr_t>(("ExAllocatePool"));
        if (!function_address) {
            return nullptr;
        }

        using function_t = void* (
            POOL_TYPE pool_type,
            SIZE_T number_of_bytes
            );

        return reinterpret_cast<function_t*>(function_address)(
            pool_type,
            number_of_bytes);
    }

    void rtl_init_unicode_string(UNICODE_STRING dest,
        PCWSTR src
    ) {
        auto function_address = find_export<addr_t>(("RtlInitUnicodeString"));
        if (!function_address) {
            return;
        }

        using function_t = void(
            UNICODE_STRING dest,
            PCWSTR src
            );

        return reinterpret_cast<function_t*>(function_address)(
            dest,
            src);
    }

    void io_delete_device(
        PDEVICE_OBJECT device_object
    ) {
        auto function_address = find_export<addr_t>(("IoDeleteDevice"));
        if (!function_address) {
            return;
        }

        using function_t = void(
            PDEVICE_OBJECT device_object
            );

        reinterpret_cast<function_t*>(function_address)(
            device_object);
    }

    NTSTATUS io_create_symbolic_link(
        PUNICODE_STRING symlinkname,
        PUNICODE_STRING devicename
    ) {
        auto function_address = find_export<addr_t>(("IoCreateSymbolicLink"));
        if (!function_address) {
            return {};
        }

        using function_t = NTSTATUS(
            PUNICODE_STRING symlinkname,
            PUNICODE_STRING devicename
        );

        return reinterpret_cast<function_t*>(function_address)(symlinkname,
            devicename);
    }

    NTSTATUS io_create_device(
        PDRIVER_OBJECT driver_object,
        ULONG           device_ext_size,
        PUNICODE_STRING name,
        DEVICE_TYPE     type,
        ULONG           characeristic,
        BOOLEAN         exclusive,
        PDEVICE_OBJECT* device_object
    ) {
        auto function_address = find_export<addr_t>(("IoCreateDevice"));
        if (!function_address) {
            return {};
        }

        using function_t = NTSTATUS(
            PDRIVER_OBJECT driver_object,
            ULONG           device_ext_size,
            PUNICODE_STRING name,
            DEVICE_TYPE     type,
            ULONG           characeristic,
            BOOLEAN         exclusive,
            PDEVICE_OBJECT* device_object
        );

        return reinterpret_cast<function_t*>(function_address)(driver_object,
            device_ext_size,
            name,
            type,
            characeristic,
            exclusive,
            device_object);
    }


    void ex_free_pool(
        void* base_address
    ) {
        auto function_address = find_export<addr_t>(("ExFreePool"));
        if (!function_address) {
            return;
        }

        using function_t = void(
            void* base_address
            );

        reinterpret_cast<function_t*>(function_address)(
            base_address);
    }


    void* ex_allocate_pool_with_tag(
        POOL_TYPE pool_type,
        SIZE_T number_of_bytes,
        ULONG tag
    ) {
        auto function_address = find_export<addr_t>(("ExAllocatePoolWithTag"));
        if (!function_address) {
            return nullptr;
        }

        using function_t = void* (
            POOL_TYPE pool_type,
            SIZE_T number_of_bytes,
            ULONG tag
            );

        return reinterpret_cast<function_t*>(function_address)(
            pool_type,
            number_of_bytes,
            tag);
    }


    void ex_free_pool_with_tag(
        void* base_address,
        ULONG tag
    ) {
        auto function_address = find_export<addr_t>(("ExFreePoolWithTag"));
        if (!function_address) {
            return;
        }

        using function_t = void(
            void* base_address,
            ULONG tag
            );

        reinterpret_cast<function_t*>(function_address) (
            base_address,
            tag);
    }

    void* mm_map_io_space(
        uintptr_t physical_address,
        SIZE_T number_of_bytes
    ) {
        PHYSICAL_ADDRESS phys_addr{ physical_address };

        auto function_address = find_export<addr_t>(("MmMapIoSpace"));
        if (!function_address) {
            return {};
        }

        using function_t = void* (
            PHYSICAL_ADDRESS physical_address,
            SIZE_T number_of_bytes,
            MEMORY_CACHING_TYPE cache_type
            );

        return reinterpret_cast<function_t*>(function_address) (
            phys_addr,
            number_of_bytes,
            MmNonCached);
    }

    void* map_io_space_ex(
        PHYSICAL_ADDRESS physical_address,
        SIZE_T number_of_bytes,
        ULONG protect
    ) {
        auto function_address = find_export<addr_t>(("MmMapIoSpaceEx"));
        if (!function_address) {
            return {};
        }

        using function_t = void* (
            PHYSICAL_ADDRESS physical_address,
            SIZE_T number_of_bytes,
            ULONG protect
            );

        return reinterpret_cast<function_t*>(function_address) (
            physical_address,
            number_of_bytes,
            protect);
    }

    void rtl_init_unicode_string(
        PUNICODE_STRING destination_string,
        PCWSTR source_string
    ) {
        auto function_address = find_export<addr_t>(("RtlInitUnicodeString"));
        if (!function_address) {
            return;
        }

        using function_t = void* (
            PUNICODE_STRING destination_string,
            PCWSTR source_string
            );

        reinterpret_cast<function_t*>(function_address) (
            destination_string,
            source_string);
    }


    uintptr_t get_physical_address(
        uintptr_t virtual_address
    ) {
        auto function_address = find_export<addr_t>(("MmGetPhysicalAddress"));
        if (!function_address) {
            return {};
        }

        using function_t = PHYSICAL_ADDRESS(
            void* virtual_address
        );

        return reinterpret_cast<function_t*>(function_address)(
            reinterpret_cast<void*>(virtual_address)
            ).QuadPart;
    }


    uintptr_t get_virtual_for_physical(
        uintptr_t physical_address
    ) {
        PHYSICAL_ADDRESS phys_addr{ };
        phys_addr.QuadPart = physical_address;

        auto function_address = find_export<addr_t>(("MmGetVirtualForPhysical"));
        if (!function_address) {
            return {};
        }

        using function_t = void* (
            PHYSICAL_ADDRESS physical_address
            );

        return reinterpret_cast<uintptr_t>(
            reinterpret_cast<function_t*>(function_address)(
                phys_addr
                ));
    }

    PPHYSICAL_MEMORY_RANGE mm_get_physical_memory_ranges()
    {
        auto function_address = find_export<addr_t>(("MmGetPhysicalMemoryRanges"));
        if (!function_address) {
            return nullptr;
        }

        using function_t = PPHYSICAL_MEMORY_RANGE(void);

        return reinterpret_cast<function_t*>(function_address)();
    }

    void* ps_get_process_section_base_address(
        PEPROCESS Process
    ) {
        auto function_address = find_export<addr_t>(("PsGetProcessSectionBaseAddress"));
        if (!function_address) {
            return nullptr;
        }

        using function_t = void* (
            PEPROCESS Process
            );

        return reinterpret_cast<function_t*>(function_address)(Process);
    }

    NTSTATUS mm_copy_memory(
        void* target_address,
        MM_COPY_ADDRESS source_address,
        SIZE_T number_of_bytes,
        ULONG flags,
        PSIZE_T number_of_bytes_transferred
    ) {
        auto function_address = find_export<addr_t>(("MmCopyMemory"));
        if (!function_address) {
            return {};
        }

        using function_t = NTSTATUS(
            void* target_address,
            MM_COPY_ADDRESS source_address,
            SIZE_T number_of_bytes,
            ULONG falgs,
            PSIZE_T number_of_bytes_transferred
        );

        return reinterpret_cast<function_t*>(function_address) (
            target_address,
            source_address,
            number_of_bytes,
            flags,
            number_of_bytes_transferred);
    }


    bool mm_is_address_valid(
        void* VirtualAddress
    ) {
        auto function_address = find_export<addr_t>(("MmIsAddressValid"));
        if (!function_address) {
            return false;
        }

        using function_t = bool(
            void* VirtualAddress
            );

        return reinterpret_cast<function_t*>(function_address)(VirtualAddress);
    }
}