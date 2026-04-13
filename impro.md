# Impro

## Syscalls

Basically ferdig, mangler bare et par implementasjoner. Følger linux ganske tett egt

## User Space

*(Planned but not started)*

Rølp AI har funnet ut:

### 2.1 Restructure paging for user/kernel separation

Current paging identity-maps 0–8MB. Need:

- **Kernel virtual** at 0xC0000000 (high half)
- **User virtual** at 0x00000000–0xC0000000

Physical layout:

```
0x000000–0x0FFFFF  (1MB)   : Bootstrap, then user space
0x100000–0x3FFFFF  (3MB)   : Kernel
0x400000–0x7FFFFF  (4MB)   : Page tables, kernel stacks
0x800000–0x9FFFFF  (2MB)   : Initial user process
0xA00000–           : User dynamic allocations
```

### 2.2 User-mode memory allocator

`user_mmap(start, length)` and `user_munmap()` using physical page allocator + page table updates.

### 2.3 User-mode segment registers

GDT entries 3 (code) and 4 (data) already defined with base=0. Update base dynamically per-process, or use 4GB limit
with paging protection.

### 2.4 Return-to-user stub

Assembly routine `return_to_user(eip, cs, eflags, esp, ss)` that pushes user context and executes `iret`.

### 2.5 Syscall return path

Modify `syscall_stub` to load user segment registers (selector 0x23) and use `iret` when returning to user space.

### 2.6 Flat executable format

```
Offset 0  : Magic "SBSP" (4 bytes)
Offset 4  : Entry point RVA (4 bytes)
Offset 8  : Code size (4 bytes)
Offset 12 : Data size (4 bytes)
Offset 16 : BSS size (4 bytes)
Offset 20–63: Reserved / padding
Offset 64+ : Code bytes, then Data bytes
```

### 2.7 Userspace libc

- `syscall0/1/2/3/4/5(n, ...)` — `int 0x80` inline asm
- `_exit`, `getpid`, `read`, `write`
- `printf` via existing `vfprintf`
- `malloc`/`free` bump allocator

---

## Phase 3: FAT32 Filesystem + RAM Disk

*(Planned but not started)*

### 3.1 Load FAT32 into RAM

Load `fat32.img` (passed by Limine or embedded in ISO) into a pre-allocated RAM region (~32MB) via `alloc_page()`.

### 3.2 VFS layer

```
vnode_t { type, inode, ops* }
file_t  { fd, flags, vnode*, position }
fs_ops  { open, read, write, close, readdir }
```

### 3.3 FAT32 driver

Parse BPB, implement cluster-to-sector conversion, FAT table traversal, directory entry reading (8.3 short filenames).

### 3.4 Syscalls: open/read/close

- `SYS_open(path, flags)` → find dir entry, allocate fd
- `SYS_read(fd, buf, n)` → copy from filesystem to user buffer
- `SYS_close(fd)` → release fd

### 3.5 Path resolution

Split `/foo/bar.txt` on `/`, traverse FAT32 directory entries.

---

## Phase 4: Shell Commands

*(Planned but not started)*

| Command        | Description                       |
|----------------|-----------------------------------|
| `ls [path]`    | List directory entries            |
| `cat <file>`   | Print file contents               |
| `cd <dir>`     | Change working directory          |
| `mkdir <path>` | Create directory                  |
| `run <bin>`    | Load and execute userspace binary |
| `reboot`       | Triple fault to reboot            |

---

## Key Files

| File                               | Action                                       |
|------------------------------------|----------------------------------------------|
| `src/arch/i386/cpu/interrupts.asm` | Modify syscall_stub for iret-to-usermode     |
| `src/kernel/memory.c`              | Restructure paging + user allocator          |
| `src/arch/i386/linker.ld`          | Add userspace program linker script          |
| `src/kernel/main.c`                | Load FAT32 image into RAM, init userspace    |
| `src/kernel/fs/vfs.c`              | **New** VFS layer                            |
| `src/kernel/fs/fat32.c`            | **New** FAT32 driver                         |
| `src/kernel/fs/ramdisk.c`          | **New** RAM disk manager                     |
| `include/kernel/fs.h`              | **New** VFS + FAT32 headers                  |
| `src/shell/commands/ls.c`          | **New**                                      |
| `src/shell/commands/cat.c`         | **New**                                      |
| `src/shell/commands/cd.c`          | **New**                                      |
| `src/shell/commands/run.c`         | **New**                                      |
| `CMakeLists.txt`                   | Add userspace build target, FAT32 FS sources |
