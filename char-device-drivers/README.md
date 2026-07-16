# Character Device Driver

A minimal Linux character device driver demonstrating the core of
kernel/userspace communication.

## What it does

- Registers a character device with a major/minor number
- Implements file operations: `open`, `read`, `write`, `release`
- Exchanges data with userspace via `copy_to_user` / `copy_from_user`
- Handles custom commands through `ioctl`
- Creates the `/dev` node automatically via udev

## Build

```bash
make
```

## Load

```bash
sudo insmod chardev.ko
dmesg | tail
```

The device node appears automatically under `/dev/<name>`.

## Test

```bash
echo "hello kernel" > /dev/<name>
cat /dev/<name>
```

## Unload

```bash
sudo rmmod chardev
```

## Key concepts

- `file_operations` structure
- userspace/kernel boundary (`copy_to_user` / `copy_from_user`)
- `ioctl` command handling
- automatic device node creation with `class_create` / `device_create`
