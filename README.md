# Linux Kernel Drivers

Collection of Linux kernel modules and device drivers written in C,
built while working deep into the kernel/userspace boundary.

Each folder is a self-contained module with its own source, Makefile,
and README explaining what it does and how to run it.

## Contents

| Module                                     | Description                                                                     |
| ------------------------------------------ | ------------------------------------------------------------------------------- |
| [char-device-driver](./char-device-driver) | Character device driver: file operations, ioctl, userspace/kernel data exchange |

## Build & run (generic)

Each module builds independently:

```bash
cd <module-folder>
make
sudo insmod <module>.ko
dmesg | tail            # check kernel logs
sudo rmmod <module>
```

## Requirements

- Linux kernel headers (`linux-headers-$(uname -r)`)
- `make`, `gcc`
- Root privileges to load/unload modules

## Development environment

Modules are developed and tested with [virtme-ng](https://github.com/arighi/virtme-ng)
for fast build-test cycles on custom kernels, without touching the host system.

## Author

Olivier Tanoh - Embedded Software Engineer (Embedded Linux & Kernel)
