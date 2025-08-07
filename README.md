# ioctl-driver

- Lightweight Windows kernel-mode and user-mode communication framework using IOCTL.
- Designed for manual mapping.
- Currently Undetected EAC/BE/VGK

## Structure

- `ioctl-km`: Kernel module.
- `ioctl-um`: User-mode module.

## Features

- No `IoCreateDriver`.
- Read and write physical memory
- Retrieve the CR3 register value of target processes
- Obtain EPROCESS & base addresses

## Usage

- Load the kernel driver via manual mapping.

## Contact
- If you need high-quality kernel or UEFI cheat, hit me up on [telegram - readcr0](https://t.me/readcr0)
- Come only money ready!
