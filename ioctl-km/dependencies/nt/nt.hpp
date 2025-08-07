#pragma once
#include "..\..\core\wdk.hpp"
#include "..\..\dependencies\std.hpp"
#include "cstdint"

namespace nt
{
    template< class type_t >

    type_t find_export(
        const char* export_name
    );

    NTSTATUS ps_lookup_process_by_process_id(
        HANDLE pid,
        PEPROCESS* process
    );

    LIST_ENTRY* ps_active_process_head();

    std::uintptr_t ps_initial_system_process();

    std::uint32_t ps_get_process_id(
        std::uint64_t process
    );

    void* ex_allocate_pool(
        POOL_TYPE pool_type,
        SIZE_T number_of_bytes
    );

    void io_delete_device(
        PDEVICE_OBJECT device_object
    );


    NTSTATUS io_create_symbolic_link(
        PUNICODE_STRING symlinkname,
        PUNICODE_STRING devicename
    );

    NTSTATUS io_create_device(
        PDRIVER_OBJECT driver_object,
        ULONG           device_ext_size,
        PUNICODE_STRING name,
        DEVICE_TYPE     type,
        ULONG           characeristic,
        BOOLEAN         exclusive,
        PDEVICE_OBJECT* device_object
    );

    void rtl_init_unicode_string(
        UNICODE_STRING dest,
        PCWSTR src
    );

    void ex_free_pool(
        void* base_address
    );

    void* ex_allocate_pool_with_tag(
        POOL_TYPE pool_type,
        SIZE_T number_of_bytes,
        ULONG tag
    );

    NTSTATUS zw_close(
        HANDLE Handle
    );

    void ex_free_pool_with_tag(
        void* base_address,
        ULONG tag
    );

    NTSTATUS mm_copy_memory(
        void* target_address,
        MM_COPY_ADDRESS source_address,
        SIZE_T number_of_bytes,
        ULONG flags,
        PSIZE_T number_of_bytes_transferred
    );


    void* mm_map_io_space(
        std::uintptr_t physical_address,
        SIZE_T number_of_bytes
    );

    void* map_io_space_ex(
        PHYSICAL_ADDRESS physical_address,
        SIZE_T number_of_bytes,
        ULONG protect
    );

    void mm_unmap_io_space(
        void* base_address,
        SIZE_T number_of_bytes
    );

    void mm_unmap_io_space(
        void* base_address,
        SIZE_T number_of_bytes
    );


    void rtl_init_unicode_string(
        PUNICODE_STRING destination_string,
        PCWSTR source_string
    );

    std::uintptr_t get_physical_address(
        std::uintptr_t virtual_address
    );


    std::uintptr_t get_virtual_for_physical(
        std::uintptr_t physical_address);

    PPHYSICAL_MEMORY_RANGE mm_get_physical_memory_ranges();

    void* ps_get_process_section_base_address(
        PEPROCESS Process
    );


    bool mm_is_address_valid(
        void* VirtualAddress
    );

    template< class... args_t >
    std::int8_t dbg_print(
        const char* format,
        args_t... va_args
    ) {
        auto function_address = nt::find_export<addr_t>(("DbgPrintEx"));
        if (!function_address) {
            return {};
        }

        using function_t = int32_t(
            uint32_t flag,
            uint32_t level,
            const char* format,
            args_t... va_args
        );

        return reinterpret_cast<function_t*>(function_address)(0, 0, format, va_args...) == STATUS_SUCCESS;
    }
}
