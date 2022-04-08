#include <ntddk.h>


PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING rDevices, rDosDevices;

#ifdef __cplusplus
extern "C" {
#endif
	int _fltused = 0; // it should be a single underscore since the double one is the mangled name
#ifdef __cplusplus
}
#endif

double GetPentiumTimeAsDouble(unsigned __int64 frequency) // RDTSC Direct
{
	// returns < 0 on failure; otherwise, returns current cpu time, in seconds.
	// warning: watch out for wraparound!

	if (frequency == 0)
		return -1.0;

	// get high-precision time:
	__try
	{
		unsigned __int64 high_perf_time;
		unsigned __int64* dest = &high_perf_time;
		__asm
		{
			_emit 0xf        // these two bytes form the 'rdtsc' asm instruction,
			_emit 0x31       //  available on Pentium I and later.
			mov esi, dest
			mov[esi], eax    // lower 32 bits of tsc
			mov[esi + 4], edx    // upper 32 bits of tsc
		}
		__int64 time_s = (__int64)(high_perf_time / frequency);  // unsigned->sign conversion should be safe here
		__int64 time_fract = (__int64)(high_perf_time % frequency);  // unsigned->sign conversion should be safe here
		// note: here, we wrap the timer more frequently (once per week) 
		// than it otherwise would (VERY RARELY - once every 585 years on
		// a 1 GHz), to alleviate floating-point precision errors that start 
		// to occur when you get to very high counter values.  
		double ret = (time_s % (60 * 60 * 24 * 7)) + (double)time_fract / (double)((__int64)frequency);
		return ret;
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		return -1.0;
	}

	return -1.0;
}

VOID intelfix(PDRIVER_OBJECT pDriverObject)
{
	__int64 FixLeak = KeQueryActiveProcessors();
	int i;
	for (i = 0; i < 32; i++) {
		if (FixLeak & ((__int64)1 << i)) {
			KeSetSystemAffinityThread((__int64)-2 << i);
		}
	}
}


NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Unloading FinalFix!\n");
	IoDeleteSymbolicLink(&rDosDevices);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath) 
{
	intelfix(pDriverObject);
	GetPentiumTimeAsDouble(1);
	DbgPrintEx(0, 0, "FinalFix Loaded!\n");


	RtlInitUnicodeString(&rDevices, L"\\Device\\FinalFix");
	RtlInitUnicodeString(&rDosDevices, L"\\DosDevices\\FinalFix");

	IoCreateSymbolicLink(&rDosDevices, &rDevices);

	
	pDriverObject->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
