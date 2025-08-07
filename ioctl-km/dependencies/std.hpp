#pragma once
#include <cstdint>

using addr_t = unsigned char*;

enum pe_magic_t {
    dos_header = 0x5a4d,
    nt_headers = 0x4550,
    opt_header = 0x020b
};

struct export_directory_t {
    std::int32_t m_characteristics;
    std::int32_t m_time_date_stamp;
    std::int16_t m_major_version;
    std::int16_t m_minor_version;
    std::int32_t m_name;
    std::int32_t m_base;
    std::int32_t m_number_of_functions;
    std::int32_t m_number_of_names;
    std::int32_t m_address_of_functions;
    std::int32_t m_address_of_names;
    std::int32_t m_address_of_names_ordinals;
};

struct data_directory_t {
    std::int32_t m_virtual_address;
    std::int32_t m_size;

    template< class type_t >

    type_t as_rva(
        addr_t rva
    ) {
        return reinterpret_cast<type_t>(rva + m_virtual_address);
    }
};


struct dos_header_t
{
    std::int16_t m_magic;
    std::int16_t m_cblp;
    std::int16_t m_cp;
    std::int16_t m_crlc;
    std::int16_t m_cparhdr;
    std::int16_t m_minalloc;
    std::int16_t m_maxalloc;
    std::int16_t m_ss;
    std::int16_t m_sp;
    std::int16_t m_csum;
    std::int16_t m_ip;
    std::int16_t m_cs;
    std::int16_t m_lfarlc;
    std::int16_t m_ovno;
    std::int16_t m_res0[0x4];
    std::int16_t m_oemid;
    std::int16_t m_oeminfo;
    std::int16_t m_res1[0xa];
    std::int32_t m_lfanew;

    constexpr bool is_valid() {
        return m_magic == pe_magic_t::dos_header;
    }
};

struct nt_headers_t {
    std::int32_t m_signature;
    std::int16_t m_machine;
    std::int16_t m_number_of_sections;
    std::int32_t m_time_date_stamp;
    std::int32_t m_pointer_to_symbol_table;
    std::int32_t m_number_of_symbols;
    std::int16_t m_size_of_optional_header;
    std::int16_t m_characteristics;

    std::int16_t m_magic;
    std::int8_t m_major_linker_version;
    std::int8_t m_minor_linker_version;
    std::int32_t m_size_of_code;
    std::int32_t m_size_of_initialized_data;
    std::int32_t m_size_of_uninitialized_data;
    std::int32_t m_address_of_entry_point;
    std::int32_t m_base_of_code;
    std::uint64_t m_image_base;
    std::int32_t m_section_alignment;
    std::int32_t m_file_alignment;
    std::int16_t m_major_operating_system_version;
    std::int16_t m_minor_operating_system_version;
    std::int16_t m_major_image_version;
    std::int16_t m_minor_image_version;
    std::int16_t m_major_subsystem_version;
    std::int16_t m_minor_subsystem_version;
    std::int32_t m_win32_version_value;
    std::int32_t m_size_of_image;
    std::int32_t m_size_of_headers;
    std::int32_t m_check_sum;
    std::int16_t m_subsystem;
    std::int16_t m_dll_characteristics;
    std::uint64_t m_size_of_stack_reserve;
    std::uint64_t m_size_of_stack_commit;
    std::uint64_t m_size_of_heap_reserve;
    std::uint64_t m_size_of_heap_commit;
    std::int32_t m_loader_flags;
    std::int32_t m_number_of_rva_and_sizes;

    data_directory_t m_export_table;
    data_directory_t m_import_table;
    data_directory_t m_resource_table;
    data_directory_t m_exception_table;
    data_directory_t m_certificate_table;
    data_directory_t m_base_relocation_table;
    data_directory_t m_debug;
    data_directory_t m_architecture;
    data_directory_t m_global_ptr;
    data_directory_t m_tls_table;
    data_directory_t m_load_config_table;
    data_directory_t m_bound_import;
    data_directory_t m_iat;
    data_directory_t m_delay_import_descriptor;
    data_directory_t m_clr_runtime_header;
    data_directory_t m_reserved;

    constexpr bool is_valid() {
        return m_signature == pe_magic_t::nt_headers
            && m_magic == pe_magic_t::opt_header;
    }
};

struct section_header_t {
    char m_name[0x8];
    union {
        std::int32_t m_physical_address;
        std::int32_t m_virtual_size;
    };
    std::int32_t m_virtual_address;
    std::int32_t m_size_of_raw_data;
    std::int32_t m_pointer_to_raw_data;
    std::int32_t m_pointer_to_relocations;
    std::int32_t m_pointer_to_line_numbers;
    std::int16_t m_number_of_relocations;
    std::int16_t m_number_of_line_numbers;
    std::int32_t m_characteristics;
};