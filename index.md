---
title: VMEM
layout: pmdk
---
The source for VMEM is in this
[GitHub repository](https://github.com/pmem/vmem/).

The following libraries are part of VMEM:

#### libvmem

The **libvmem** library turns a pool of persistent memory into a
volatile memory pool, similar to the system heap but kept separate
and with its own malloc-style API.

See the [libvmem page](libvmem) for documentation and examples.

>NOTE:
Since persistent memory support
has been integrated into [libmemkind](https://github.com/memkind/memkind),
that library is the **recommended** choice for any new volatile usages,
since it combines support for multiple types of volatile memory into
a single, convenient API.

#### libvmmalloc

The **libvmmalloc** library **transparently** converts all the dynamic
memory allocations into persistent memory allocations.  This allows the use
of persistent memory as volatile memory without modifying the target
application.

See the [libvmmalloc page](libvmmalloc) for documentation and examples.
