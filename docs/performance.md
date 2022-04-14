<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Performance considerations

The library has been designed and implemented to make sure that any performance penalty is minimal.

## Iterating through elements

Whatever the indexing scheme is, data is stored as a contiguous array. So, usage of `begin`, `end`,
`for` loop cost the same as for a traditional, one-dimension array.

## Range-indexing

Range indexing results in a single offset being added for each access. With 
optimizations enabled, the same code is generated for the following snippets :

```cpp
extern int f(int);

static indexed_array<int, interval<3, 7>> vals_idx{0, 1, 2, 3, 4};

int test_idx(int idx)
{
	return f(vals_idx[idx]);
}

static std::array<int, 5> vals_arr{0, 1, 2, 3, 4};

int test_arr(int idx)
{
	return f(vals_arr[idx - 3]);
}
```

Both will result in the same assembly:

```asm
.LFB2259:
        .cfi_startproc
        subl    $3, %edi
        leaq    _ZL8vals_idx(%rip), %rax
        movslq  %edi, %rdi
        movl    (%rax,%rdi,4), %edi
        jmp     _Z1fi@PLT
        .cfi_endproc

.LFB2264:
        .cfi_startproc
        subl    $3, %edi
        leaq    _ZL8vals_arr(%rip), %rax
        movslq  %edi, %rdi
        movl    (%rax,%rdi,4), %edi
        jmp     _Z1fi@PLT
        .cfi_endproc
```

If the starting offset is zero, then there is no penalty at all compared to a plain array.

## Enum indexing

Enum indexing, with a 0-starting range, incurs no run-time penalty at all:

```cpp
extern int f(int);

enum class color
{
	red,
	green,
	blue,
	black
};

const std::array<int, 4> vals_int{0xFF0000, 0x00FF00, 0x0000FF, 0};

const indexed_array<int, interval<color::red, color::black>> vals_idx{0xFF0000, 0x00FF00, 0x0000FF, 0};

int test_idx(color idx)
{
	return f(vals_idx[idx]);
}

int test_arr(int idx)
{
	return f(vals_int[idx]);
}
```

Results in the following assembly:

```asm
.LFB2259:
        .cfi_startproc
        movslq  %edi, %rdi
        leaq    _ZL8vals_idx(%rip), %rax
        movl    (%rax,%rdi,4), %edi
        jmp    _Z1fi@PLT
        .cfi_endproc

.LFB2264:
        .cfi_startproc
        movslq  %edi, %rdi
        leaq    _ZL8vals_int(%rip), %rax
        movl    (%rax,%rdi,4), %edi
        jmp    _Z1fi@PLT
        .cfi_endproc
```

If starting at a non-zero value, this is equivalent to range indexing.

## Complex indexing schemes

Compile time indexes are optimized by the compiler. So, the following code :

```cpp
extern int f(int);

enum class color
{
	red = 1,
	green = 2,
	blue = 4,
	black = 8
};
using idxarray = indexed_array<int, std::integer_sequence<color, color::red, color::green, color::blue, color::black>>;
const idxarray vals_idx{0xFF0000, 0x00FF00, 0x0000FF, 0};

int test_idx(idxarray const& arr)
{
	return f(arr[color::blue]) + f(vals_idx[color::red]);
}
```

Results in the following assembly (the same assembly is generated if using a plain array)

```asm
_Z8test_idxRKN3jbc13indexed_array6detail13indexed_arrayIiNS1_15default_indexerISt16integer_sequenceI5colorJLS5_1ELS5_2ELS5_4ELS5_8EEEvEEEE:
.LFB3568:
        .cfi_startproc
        pushq   %rbx
        .cfi_def_cfa_offset 16
        .cfi_offset 3, -16
        movl    8(%rdi), %edi
        call    _Z1fi@PLT
        movl    $16711680, %edi
        movl    %eax, %ebx
        call    _Z1fi@PLT
        addl    %ebx, %eax
        popq    %rbx
        .cfi_def_cfa_offset 8
        ret
        .cfi_endproc
```

Run-time indexes, however, are resolved in linear time

```cpp
int test_idx2(color c)
{
	return vals_idx[c];
}
```

```asm
_Z9test_idx25color:
.LFB3582:
        .cfi_startproc
        movl    $16711680, %eax
        cmpl    $1, %edi
        je      .L23
        cmpl    $2, %edi
        je      .L24
        movl    $255, %eax
        cmpl    $4, %edi
        je      .L25
        cmpl    $8, %edi
        je      .L26
        movl    -4+_ZL8vals_idx(%rip), %eax
        ret
.L24:
        movl    $65280, %eax
        ret
        .p2align 4,,10
        .p2align 3
.L23:
        ret
        .p2align 4,,10
        .p2align 3
.L25:
        ret
.L26:
        xorl    %eax, %eax
        ret
        .cfi_endproc
```

Which is a bit worse than what would give a switch/case approach, but should be acceptable in much cases:

```asm
_Z9test_idx35color:
.LFB3584:
        .cfi_startproc
        cmpl    $4, %edi
        je      .L32
        jg      .L30
        movl    $16711680, %eax
        cmpl    $1, %edi
        je      .L28
        cmpl    $2, %edi
        jne     .L31
        movl    $65280, %eax
.L28:
        ret
        .p2align 4,,10
        .p2align 3
.L30:
        xorl    %eax, %eax
        cmpl    $8, %edi
        je      .L28
.L31:
        movslq  %edi, %rdi
        leaq    _ZL8vals_int(%rip), %rax
        movl    (%rax,%rdi,4), %eax
        ret
        .p2align 4,,10
        .p2align 3
.L32:
        movl    $255, %eax
        ret
        .cfi_endproc
```

Remember that, if you need optimal performance, you can always provide your own custom implementation of the indexer. In our 
example, we can write:

```cpp
struct custom_indexer
{
	using index = color;
	static inline constexpr size_t size = 4;
	template<bool c = true>
	static constexpr std::size_t at(index v)
	{
		int r = static_cast<int>(v);
		if constexpr(c)
		{
			if (r != 1 && r != 2 && r != 4 && r != 8)
				throw std::out_of_range("Invalid index");
		}
		// this computation gives the correct result
		return (r & 0x1) + (r & 0x2) + ((r & 0x4) >> 2) + ((r & 0x4) >> 1) + ((r & 0x8) >> 1) - 1;
	}
};
```

Which will results in (no range check is done, because it is not requested by operator[]):

```asm
t_idx25color:
.LFB3575:
        .cfi_startproc
        movl    %edi, %eax
        movl    %edi, %edx
        movl    %edi, %ecx
        sarl    $2, %eax
        sarl    %ecx
        andl    $3, %edx
        andl    $1, %eax
        addl    %edx, %eax
        movl    %ecx, %edx
        andl    $4, %ecx
        andl    $2, %edx
        addl    %edx, %eax
        leaq    _ZL8vals_idx(%rip), %rdx
        leal    -1(%rax,%rcx), %eax
        cltq
        movl    (%rdx,%rax,4), %eax
        ret
        .cfi_endproc
```

### Detection of contiguous sequences

The code is optimized for the case
where a sequence is contiguous (it can be represented as an interval), so given the following:

```cpp
indexed_array<int, interval<3, 7>> arr1;
indexed_array<int, std::integer_sequence<int, 3, 4, 5, 6, 7>> arr2;
```

Accesses to `arr1` and `arr2` will incur the same single offset calculation. This makes it
possible to use the library efficiently with `describe`-d enums, which are always resolved as an
`integer_sequence`.
