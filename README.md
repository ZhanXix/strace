# mystrace

一个简易的 Linux `strace` 实现，基于 `ptrace` 系统调用，用于跟踪目标进程的系统调用并打印调用名称与返回值。仅支持 x86-64 架构。

## 编译

```bash
make
```

## 使用

跟踪一个命令：

```bash
./strace ls -la
```

附加到已运行的进程（需要 root 权限）：

```bash
./strace -p <pid>
```

## 输出示例

```
execve         = 0
brk            = 0x55a3b2c
access         = 0xfffffffffffffffe
mmap           = 0x7f8a3b000000
...
exited in 120 syscalls with status=0
```

## 实现原理

- 父进程作为 tracer，通过 `PTRACE_SYSCALL` 在每次系统调用入口/出口暂停子进程
- 使用 `PTRACE_GETREGS` 读取寄存器，`orig_rax` 获取系统调用号，`rax` 获取返回值
- 通过硬编码的 x86-64 系统调用表（324 项）将调用号映射为可读名称

---

> 这是我大约六年前写的项目，应该是当时用来学习 Linux `ptrace` 机制和系统调用跟踪原理的。现在已经完全忘了，本 README 由 AI 辅助整理生成。
