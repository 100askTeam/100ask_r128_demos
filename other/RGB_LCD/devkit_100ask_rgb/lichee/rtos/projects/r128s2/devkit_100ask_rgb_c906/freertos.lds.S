OUTPUT_ARCH("riscv")
OUTPUT_FORMAT("elf64-littleriscv","elf64-littleriscv","elf64-littleriscv")

#if (defined(CONFIG_ROM))
#include "rom_symbol.ld"
#endif

/* Linker script to configure memory regions. */
MEMORY
{
    RAM (rwx)   : ORIGIN = CONFIG_ARCH_START_ADDRESS, LENGTH = CONFIG_ARCH_MEM_LENGTH
    FLASH (rx)  : ORIGIN = 0x10000000, LENGTH = 16M
#if (defined(CONFIG_SRAM))
    SRAM (rwx) : ORIGIN = CONFIG_SRAM_START_ADDRESS, LENGTH = CONFIG_SRAM_LENGTH
#else
    SRAM (rwx) : ORIGIN = 0x04000000, LENGTH = 1M
#endif
#if (defined(CONFIG_PSRAM))
    PSRAM (rwx) : ORIGIN = CONFIG_PSRAM_START_ADDRESS, LENGTH = CONFIG_PSRAM_LENGTH
#else
    PSRAM (rwx) : ORIGIN = 0x08000000, LENGTH = 8M
#endif
#if (defined(CONFIG_HPSRAM))
    HPSRAM (rwx) : ORIGIN = CONFIG_HPSRAM_START_ADDRESS, LENGTH = CONFIG_HPSRAM_LENGTH
#else
    HPSRAM (rwx) : ORIGIN = 0x0c000000, LENGTH = 8M
#endif
}

__RAM_BASE = ORIGIN(RAM);
__PSRAM_BASE = ORIGIN(PSRAM);
__PSRAM_LENGTH = LENGTH(PSRAM);
__HPSRAM_BASE = ORIGIN(HPSRAM);
__HPSRAM_LENGTH = LENGTH(HPSRAM);
__MSP_STACK_LENGTH = 0x100;

/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions FLASH and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *   _estack
 */
ENTRY(_start)

SECTIONS
{
#if (defined(CONFIG_XIP))
    .xip :
    {
        . = ALIGN(16);
        __xip_start__ = .;

#include "../xip.lds.S"

        *(.xip_text* .xip_rodata*)
        . = ALIGN(16);
        __xip_end__ = .;
    } > FLASH
#endif /* CONFIG_XIP */

#if (defined(CONFIG_PSRAM))
    .psram_text :
    {
        . = ALIGN(16);
        __psram_start__ = .;
        __psram_text_start__ = .;

        /* MUST not put IRQ handler/callback in .psram section */
        . = ALIGN(16);
        __psram_text_end__ = .;
    } > PSRAM

    .psram_data :
    {
        . = ALIGN(16);
        __psram_data_start__ = .;

        /* MUST not put IRQ handler/callback used data in .psram section */
        . = ALIGN(16);
        __psram_data_end__ = .;
    } > PSRAM

    .psram_bss :
    {
        . = ALIGN(16);
        __psram_bss_start__ = .;

        /* MUST not put IRQ handler/callback used data in .psram section */
        . = ALIGN(16);
        __psram_bss_end__ = .;
        __psram_end__ = .;
    } > PSRAM
#ifdef CONFIG_HEAP_MULTIPLE_DYN
    . = ALIGN(CONFIG_HEAP_MIN_ALIGNMENT_SIZE);
    __psram_heap_start = .;
#endif
#endif /* CONFIG_PSRAM */

#if (defined(CONFIG_HPSRAM))
    .hpsram_text :
    {
        . = ALIGN(16);
        __hpsram_start__ = .;
        __hpsram_text_start__ = .;

        /* MUST not put IRQ handler/callback in .hpsram section */
        . = ALIGN(16);
        __hpsram_text_end__ = .;
    } > HPSRAM

    .hpsram_data :
    {
        . = ALIGN(16);
        __hpsram_data_start__ = .;

        /* MUST not put IRQ handler/callback used data in .hpsram section */
        . = ALIGN(16);
        __hpsram_data_end__ = .;
    } > HPSRAM

    .hpsram_bss :
    {
        . = ALIGN(16);
        __hpsram_bss_start__ = .;

        /* MUST not put IRQ handler/callback used data in .hpsram section */
        . = ALIGN(16);
        __hpsram_bss_end__ = .;
        __hpsram_end__ = .;
    } > HPSRAM
#ifdef CONFIG_HEAP_MULTIPLE_DYN
    . = ALIGN(CONFIG_HEAP_MIN_ALIGNMENT_SIZE);
    __hpsram_heap_start = .;
#endif
#endif /* CONFIG_HPSRAM */

    .text :
    {
        . = ALIGN(4);
        __text_start__ = .;
        KEEP(*(.start))
        *(*.text)
        *(.text*)
        *(.nonxip_text*)
        *(.sram_text*)
        KEEP(*(.init))
        KEEP(*(.fini))

        /* .ctors */
        *crtbegin.o(.ctors)
        *crtbegin?.o(.ctors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
        *(SORT(.ctors.*))
        *(.ctors)

        /* .dtors */
        *crtbegin.o(.dtors)
        *crtbegin?.o(.dtors)
        *(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
        *(SORT(.dtors.*))
        *(.dtors)

        *(.rodata*)
        *(.nonxip_rodata*)
        *(.sram_rodata*)

#if (defined(CONFIG_COMPONENTS_CPLUSPLUS_UNWIND))
        __eh_frame_start = .;
        KEEP(*(.eh_frame*))
        __eh_frame_end = .;
        . += 16;
        __gcc_except_table_start = .;
        KEEP(*(.gcc_except_table*))
        __gcc_except_table_end = .;
#endif

        . = ALIGN(4);
        __text_end__ = .;
    } > RAM

    . = ALIGN(4);
    __etext = .;
    _sidata = .;

    .data :
    {
        . = ALIGN(4);
        __data_start__ = .;
        _sdata = .;

        *(vtable)
        *(.data*)
        *(.nonxip_data*)
        *(.sram_data*)

        . = ALIGN(4);
        /* preinit data */
        PROVIDE_HIDDEN (__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN (__preinit_array_end = .);

        . = ALIGN(4);
        /* init data */
        PROVIDE_HIDDEN (__init_array_start = .);
        PROVIDE(__ctors_start__ = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE(__ctors_end__ = .);
        PROVIDE_HIDDEN (__init_array_end = .);

        . = ALIGN(4);
        /* finit data */
        PROVIDE_HIDDEN (__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN (__fini_array_end = .);

        KEEP(*(.jcr*))

        . = ALIGN(16);
        __init_process_stack_start__ = .;
        . += 16384;
        __init_process_stack_end__ = .;
        . = ALIGN(CONFIG_HEAP_MIN_ALIGNMENT_SIZE);
        __freertos_irq_stack_bottom = .;
        . += 16384;
        __freertos_irq_stack_top = .;
        PROVIDE( __global_pointer$ = . + 0x400 );

        . = ALIGN(4);
#if (defined(CONFIG_ROM))
        *(*.got*)
#endif
        __data_end__ = .;
        _edata = .;
    } > RAM

#if defined(CONFIG_BLEHOST) || defined(CONFIG_USE_SETTINGS_WITHOUT_ZEPHYR)
#if defined(CONFIG_BLEHOST_Z_ITERABLE_SECTION)

/* ROMABLE_REGION_DEFINE_IN_SECTION=0 the common-rom.ld will define as section */
/* ROMABLE_REGION_DEFINE_IN_SECTION=1 the common-rom.ld will define *.o need define in section */
#define ROMABLE_REGION_DEFINE_IN_SECTION 0
/* choose the MEMORY */
#define ZEPHYR_BLE_RAMABLE_REGION     RAM
#define ZEPHYR_BLE_ROMABLE_REGION     RAM

#include "../../../components/common/thirdparty/bluetooth/zephyr/include/ble/linker/common-ram.ld"
#include "../../../components/common/thirdparty/bluetooth/zephyr/include/ble/linker/common-rom.ld"

#endif /* Z_ITERABLE_SECTION */
#endif /* CONFIG_BLEHOST */

   .FSymTab : {
        _syscall_table_begin = .;
	KEEP(*(FSymTab))
	_syscall_table_end = .;
   } > RAM

   .VSymTab : {
        __vsymtab_start = .;
	KEEP(*(VSymTab))
	__vsymtab_end = .;
   } > RAM

   .ttcall : {
        _tt_begin = .;
	KEEP(*(ttcall))
	_tt_end = .;
   } > RAM

    .heap (COPY):
    {
        __end__ = .;
        __heap_start__ = .;
        _heap_start = .;
        end = __end__;
        *(.heap*)
        __HeapLimit = .;
    } > RAM

    /* .stack_dummy section doesn't contains any symbols. It is only
     * used for linker to calculate size of stack sections, and assign
     * values to stack symbols later */
    .stack_dummy (COPY):
    {
        *(.stack*)
    } > RAM

    .bss (NOLOAD):
    {
        . = ALIGN(4);
        __bss_start__ = .;
        _sbss = .;

#if  CONFIG_COMPONENT_BT || CONFIG_AW_COMPONENT_BLUEDROID_CTRL
        __btc_bss_start__ = .;
        *libxrbtc.a: ( .bss .bss.* .nonxip_bss* .sram_bss* COMMON )
        *libxrbtc.o ( .bss .bss.* .nonxip_bss* .sram_bss* COMMON )
        __btc_bss_end__ = .;
#endif
        *(.bss*)
        *(COMMON)
        *(.nonxip_bss*)
        *(.sram_bss*)

        . = ALIGN(4);
        __bss_end__ = .;
        _ebss = .;
    } > RAM

    . = ALIGN(8);
#ifdef CONFIG_HEAP_MULTIPLE_DYN
    __default_heap_start = .;
#else
#ifdef CONFIG_DEFAULT_DRAM_HEAP
    __dram_heap_start = .;
#elif defined(CONFIG_DEFAULT_SRAM_HEAP)
    __sram_heap_start = .;
#elif defined(CONFIG_DEFAULT_LPSRAM_HEAP)
    __lpsram_heap_start = .;
#elif defined(CONFIG_DEFAULT_HPSRAM_HEAP)
    __hpsram_heap_start = .;
#endif
#endif

    /* Set stack top to end of RAM, and stack limit move down by
     * size of stack_dummy section */
    __StackTop = ORIGIN(RAM) + LENGTH(RAM);
    _estack = __StackTop;
    __heap_end__ = _estack - __MSP_STACK_LENGTH;
    _heap_end = __heap_end__;
    __StackLimit = __StackTop - SIZEOF(.stack_dummy);
    PROVIDE(__stack = __StackTop);

    __FLASH_BASE = ORIGIN(FLASH);
    PROVIDE(__XIP_Base = __FLASH_BASE);
#if (defined(CONFIG_XIP))
    PROVIDE(__XIP_End = __xip_end__);
#else
    PROVIDE(__XIP_End = __XIP_Base);
#endif

#if (defined(CONFIG_PSRAM))
    __PSRAM_BASE = ORIGIN(PSRAM);
    PROVIDE(__PSRAM_Base = __PSRAM_BASE);
    PROVIDE(__PSRAM_End = __psram_end__);
    __PsramTop = ORIGIN(PSRAM) + LENGTH(PSRAM);
    PROVIDE(__PSRAM_Top = __PsramTop);
#endif

#if (defined(CONFIG_HPSRAM))
    __HPSRAM_BASE = ORIGIN(HPSRAM);
    PROVIDE(__HPSRAM_Base = __HPSRAM_BASE);
    PROVIDE(__HPSRAM_End = __hpsram_end__);
    __HpsramTop = ORIGIN(HPSRAM) + LENGTH(HPSRAM);
    PROVIDE(__HPSRAM_Top = __HpsramTop);
#endif

    /* Check if data + heap + stack exceeds RAM limit */
    ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")
}
