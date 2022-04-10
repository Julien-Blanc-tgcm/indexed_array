<!--
Copyright 2022 Julien Blanc
Distributed under the Boost Software License, Version 1.0.
https://www.boost.org/LICENSE_1_0.txt
-->

# Performance considerations

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
	subl	$3, %edi
	leaq	_ZL8vals_idx(%rip), %rax
	movslq	%edi, %rdi
	movl	(%rax,%rdi,4), %edi
	jmp	_Z1fi@PLT
	.cfi_endproc

.LFB2264:
	.cfi_startproc
	subl	$3, %edi
	leaq	_ZL8vals_arr(%rip), %rax
	movslq	%edi, %rdi
	movl	(%rax,%rdi,4), %edi
	jmp	_Z1fi@PLT
	.cfi_endproc
```

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
	movslq	%edi, %rdi
	leaq	_ZL8vals_idx(%rip), %rax
	movl	(%rax,%rdi,4), %edi
	jmp	_Z1fi@PLT
	.cfi_endproc

.LFB2264:
	.cfi_startproc
	movslq	%edi, %rdi
	leaq	_ZL8vals_int(%rip), %rax
	movl	(%rax,%rdi,4), %edi
	jmp	_Z1fi@PLT
	.cfi_endproc
```

## Complex indexing schemes
