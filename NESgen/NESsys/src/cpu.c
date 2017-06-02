#include <stdio.h>
#include <stdlib.h>
#include "cpu.h"
#include "game_base.h"
#include "memory.h"
#include "ppu.h"
#include "NESsys.h"

/*
 * Initializes the CPU.
 */
void cpu_init()
{
	cpuSpeedMultiplier = APPLICATION_DEFAULT_GAME_SPEED;
	cpuCyclesCurrentSecond = 0;
	get_time(&lastSyncTime);
	cpuCyclesLastSecond = 0;
	cpuPaused = FALSE;
	cpuRestarting = FALSE;
	memset(&registers, 0, sizeof(registers));
	memset(&zeroPage, 0, sizeof(zeroPage));
	memset(&stack, 0, sizeof(stack));
	memset(&ram, 0, sizeof(ram));
	memset(&sram, 0, sizeof(sram));
	memset(&interrupts, 0, sizeof(interrupts));
	interrupts.current = INTERRUPT_RESET;
	registers.SP = 0xFF; // top of stack, moves to bottom.
	cpu_set_flags(0); // makes sure our unused flag is always set.

	// If our PRG-ROM data was never backed up, do so now, as it can be changed at runtime.
	// If it was, then we restore our backup now.
	if(prgRomDataBackup == NULL)
	{
		prgRomDataBackup = malloc(prgRomDataSize);
		memcpy(prgRomDataBackup, prgRomData, prgRomDataSize);
	}
	else
	{
		memcpy(prgRomData, prgRomDataBackup, prgRomDataSize);
	}

	// If our unmapped memory was never initialized, initialize it.
	if(unmappedCPUMemory == NULL)
		unmappedCPUMemory = malloc(0x10000);
	memset(unmappedCPUMemory, 0, 0x10000);

	debug_log("Central Processing Unit (CPU) initialized...\n");
}
/*
 * Gets the processor status flag at the given index.
 */
BOOL cpu_get_flag(BYTE index)
{
	return (registers.P & (1 << index)) != 0;
}
/*
 * Sets the flag on the processor status flags at the given index if value is true.
 */
void cpu_set_flag(BYTE index, BOOL value)
{
	if(value)
		registers.P |= (1 << index);
	else
		registers.P &= ~(1 << index);
}
/*
 * Sets processor status flags (keeping unused flag set)
 */
void cpu_set_flags(BYTE newFlags)
{
	registers.P = newFlags;
	cpu_set_flag(CPU_FLAG_UNUSED, TRUE); // always supposed to be set.
}
/*
 * Pushes the given data to the stack and decrements the stack pointer.
 */
void cpu_stack_push(BYTE data)
{
	stack[registers.SP--] = data;
}
/*
 * Increments stack pointer and pops data off the stack.
 */
BYTE cpu_stack_pop()
{
	return stack[++registers.SP];
}
/*
 * Executed after every instruction, meant to perform functions hardware normally would (handle clock cycles, PPU rendering calls, interrupts)
 * Returns TRUE if we should stop executing the current interrupt handler.
 */
BOOL cpu_sync(UINT cycles)
{
	// Call our sync event handler
	if(onCpuSync != NULL)
		onCpuSync();

	// For each CPU cycle, we the PPU will execute 3.
	for(UINT i = 0; i < cycles * 3; i++)
		ppu_update();

	// Handle NMI/IRQ if there is a request and we're not in one currently (NMI overrides IRQ)
	if(interrupts.current == INTERRUPT_RESET)
	{
		if(interrupts.requestedNMI)
		{
			// Clear our interrupt requests
			interrupts.requestedIRQ = FALSE;
			interrupts.requestedNMI = FALSE;

			// TODO: Interrupt Latency: 7 cycles

			// Handle our NMI interrupt
			interrupts.current = INTERRUPT_NMI;
			cpu_stack_push(registers.P);
			cpu_set_flag(CPU_FLAG_INTERRUPT_DISABLE, TRUE);
			game_execute(ID_FUNCTION_NMI);
			cpu_set_flags(cpu_stack_pop());
			interrupts.current = INTERRUPT_RESET;

			// TODO: Interrupt Latency: 7 cycles
		}
		else if(interrupts.requestedIRQ)
		{
			// Clear our interrupt requests
			interrupts.requestedIRQ = FALSE;
			interrupts.requestedNMI = FALSE;

			// TODO: Interrupt Latency: 7 cycles

			// Handle our IRQ request
			interrupts.current = INTERRUPT_IRQ;
			cpu_stack_push(registers.P);
			cpu_set_flag(CPU_FLAG_BREAK, TRUE);
			cpu_set_flag(CPU_FLAG_INTERRUPT_DISABLE, TRUE);
			game_execute(ID_FUNCTION_IRQ);
			cpu_set_flags(cpu_stack_pop());
			interrupts.current = INTERRUPT_RESET;

			// TODO: Interrupt Latency: 7 cycles
		}
	}
	else if(interrupts.requestedIRQ || interrupts.requestedNMI)
	{
		// But if we are in an interrupt and we requested one, tell our calling function we want to end execution.
		return TRUE;
	}

	// Add to our cpu cycle count we're tracking for this second
	cpuCyclesCurrentSecond += cycles;

	// Throttling: Loop until we're allowed to execute more cycles.
	TIMEDATA currentTime;
	ULONGLONG timeDifference;
	ULONGLONG desiredCycleCount;
	do
	{
		// Get the time difference from the time stamp when we hit the start of this second.
		get_time(&currentTime);
		timeDifference = get_time_difference(&currentTime, &lastSyncTime);

		// Calculate how many instructions we should have executed by this point.
		double secondFraction = timeDifference / (double)1000;
		desiredCycleCount = (UINT)(secondFraction * cpuSpeedMultiplier * CPU_CYCLES_PER_SECOND);
	}while(cpuPaused || cpuCyclesCurrentSecond > desiredCycleCount);

	// If we've moved a second past our last time stamp, reset our time variables.
	if(timeDifference >= 1000)
	{
		cpuCyclesLastSecond = cpuCyclesCurrentSecond;
		cpuCyclesCurrentSecond = 0;
		framesPerSecond = frameCount;
		frameCount = 0;
		lastSyncTime = currentTime;
	}
	return cpuRestarting;
}
