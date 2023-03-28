#include <ntddk.h>
#include <cstdint>
#include <malloc.h>




typedef struct _PROCESSOR_POWER_INFORMATION {
	ULONG Number;
	ULONG MaxMhz;
	ULONG CurrentMhz;
	ULONG MhzLimit;
	ULONG MaxIdleState;
	ULONG CurrentIdleState;
} PROCESSOR_POWER_INFORMATION, * PPROCESSOR_POWER_INFORMATION;


// thanks to nbourdau https://github.com/mmlabs-mindmaze/mmlib/blob/0ae40719580cfaca353dde4d43b1779330404957/src/clock-win32.c

#define PICOSEC_IN_SEC  1000000000000LL
#define MHZ_IN_HZ       1000000

static int64_t cpu_cycle_freq;
static int64_t cpu_cycle_tick_psec; // Time of a cpu cycle in picosecond

static
void determine_cpu_cycle_rate(void)
{

	int num_proc;
	PROCESSOR_POWER_INFORMATION* proc_pwr_info;

	// Get number of logical processor to allocate proc_pwr_info array size
	// to the right one
	num_proc = KeQueryActiveProcessors();

	// Get CPU information.
	// When requesting 'ProcessorInformation', the output receives one
	// PROCESSOR_POWER_INFORMATION structure for each processor that is
	// installed on the system
	proc_pwr_info = (PROCESSOR_POWER_INFORMATION*)_alloca(num_proc * sizeof(*proc_pwr_info));
	ZwPowerInformation(ProcessorInformation, NULL, 0,
		proc_pwr_info, num_proc * sizeof(*proc_pwr_info));

	// Compute CPU max clock rate and tick length
	// On x86 processors supporting invariant TSC, the TSC clock rate is
	// this one
	cpu_cycle_freq = proc_pwr_info[0].MaxMhz * MHZ_IN_HZ;
	cpu_cycle_tick_psec = PICOSEC_IN_SEC / cpu_cycle_freq;
}

VOID DriverUnload(PDRIVER_OBJECT DriverObject)
{
	// https://msdn.microsoft.com/en-us/library/windows/hardware/ff564886(v=vs.85).aspx
	DbgPrint("[*] DriverUnload\r\n");
}

NTSTATUS DriverEntry(PDRIVER_OBJECT DriverObject, PUNICODE_STRING RegistryPath)
{
   determine_cpu_cycle_rate();
	DriverObject->DriverUnload = DriverUnload;

	return STATUS_SUCCESS;
}
