#pragma once
#include "..\wdk.hpp"
#include "cstdint"

extern "C" __declspec(dllimport) void* IoDriverObjectType;

extern "C" NTSTATUS __stdcall ObCreateObject(
    std::uint8_t object_type,
    void* object_type_address,
    OBJECT_ATTRIBUTES* object_attributes,
    std::uint8_t access_mode,
    void* parse_context,
    std::uint32_t object_size,
    std::uint32_t page_charge,
    std::uint32_t tag,
    void** object
);
namespace ioctl
{
    NTSTATUS create_driver(NTSTATUS(*entry_point)(DRIVER_OBJECT*, UNICODE_STRING*));
}
