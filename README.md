# ioctl-driver

- Lightweight Windows kernel-mode and user-mode communication framework using IOCTL.
- Designed for manual mapping.
- Currently Undetected EAC/BE/VGK

## Structure

- `ioctl-km/`: Kernel module.
- `ioctl-um/`: User-mode module.

## Features

- No `IoCreateDriver`.
- Minimal IOCTL dispatch mechanism with direct handling.
- Simple user-mode interface for direct communication.
- Read and write physical and virtual memory safely from user mode.
- Retrieve the CR3 register value of target processes.
- Obtain EPROCESS base addresses for process-specific operations.

## Usage

- Load the kernel driver via manual mapping.
- 
## Contact
- If you need high-quality kernel or UEFI cheat, hit me up on [telegram - readcr0](https://t.me/readcr0)
- Come only money ready!
