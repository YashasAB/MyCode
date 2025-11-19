#ifndef _IDT_H
#define _IDT_H

extern void scheduler_wrapper(void);
extern void rtc_wrapper(void);
extern void keyboard_wrapper(void);
extern void system_call_wrapper(void);

void init_idt(); 

void divide_error_exception(void);
void debug_exception(void);
void nmi_interrupt(void);
void breakpoint_exception(void);
void overflow_exception(void);
void bound_range_exceeded_exception(void);
void invalid_opcode_exception(void);
void device_not_available_exception(void);
void double_fault_exception(void);
void coprocessor_segment_overrun(void);
void invalid_tss_exception(void);
void segment_not_present(void);
void stack_fault_exception(void);
void general_protection_exception(void);
void page_fault_exception(void);
// void RESERVED_BY_INTEL(void);
void x87_floating_point_error(void);
void alignment_check_exception(void);
void machine_check_exception(void);
void simd_floating_point_exception(void);

#endif
