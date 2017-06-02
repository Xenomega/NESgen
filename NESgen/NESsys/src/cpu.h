
#ifndef CPU_H_
#define CPU_H_
#include "NESsys.h"
#include "memory.h"

// ---------------------------------
// CPU Definitions
// ---------------------------------
#define CPU_CYCLES_PER_SECOND			1789772
#define CPU_FLAG_CARRY					0
#define CPU_FLAG_ZERO					1
#define CPU_FLAG_INTERRUPT_DISABLE		2
#define CPU_FLAG_DECIMAL				3
#define CPU_FLAG_BREAK					4
#define CPU_FLAG_UNUSED					5
#define CPU_FLAG_OVERFLOW				6
#define CPU_FLAG_SIGN					7

struct CPURegisters
{
	BYTE A; // Accumulator
	BYTE X; // X Index Register
	BYTE Y; // Y Index Register
	BYTE SP; // Stack Pointer
	BYTE P; // Processor Status Flags
	BYTE PC; // Program Counter
};
enum INTERRUPT { INTERRUPT_RESET, INTERRUPT_IRQ, INTERRUPT_NMI };
struct InterruptState
{
	BOOL requestedNMI;
	BOOL requestedIRQ;
	enum INTERRUPT current;
};


struct CPURegisters registers;
struct InterruptState interrupts;

BOOL cpuPaused;
BOOL cpuRestarting;
DOUBLE cpuSpeedMultiplier;
UINT cpuCyclesCurrentSecond;
UINT cpuCyclesLastSecond;
TIMEDATA lastSyncTime;

// ---------------------------------
// CPU Memory Regions
// ---------------------------------
BYTE zeroPage[MEMORY_PAGE_SIZE]; // 0x000-0x100
BYTE stack[MEMORY_PAGE_SIZE]; // 0x100-0x200
BYTE ram[MEMORY_PAGE_SIZE*6]; // 0x200-0x800
BYTE sram[MEMORY_PAGE_SIZE*0x20]; // 0x6000-0x8000
BYTE* prgRomDataBackup; // backs up the PRG-ROM data since it can be changed at runtime.

// ---------------------------------
// Events
// ---------------------------------
typedef void (*GenericEvent)();
GenericEvent onCpuSync;

// ---------------------------------
// Functions
// ---------------------------------
void cpu_init();
BOOL cpu_get_flag(BYTE index);
void cpu_set_flag(BYTE index, BOOL value);
void cpu_set_flags(BYTE newFlags);
void cpu_stack_push(BYTE data);
BYTE cpu_stack_pop();
BOOL cpu_sync(UINT cycles);

#endif /* CPU_H_ */
