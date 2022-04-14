
#include <ntddk.h>
PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING rDevices, rDosDevices;


NTSTATUS ThreadProc(PDRIVER_OBJECT pDriverObject)
{
	LARGE_INTEGER Freq, Timeout, t0, t1;
	LONGLONG UpdateRateMicros = 0, Elapsed, UpdateRateMs = 0;
	UpdateRateMicros = (LONGLONG)UpdateRateMs * 1000;
	t0 = KeQueryPerformanceCounter(&Freq);
	t1 = KeQueryPerformanceCounter(&Freq);
	Elapsed = ((t1.QuadPart - t0.QuadPart) * 1000000) / Freq.QuadPart; // microseconds
	Timeout.QuadPart = -((UpdateRateMicros - Elapsed) * 10); // microseconds -> units of 100 nanoseconds
	KeDelayExecutionThread(KernelMode, FALSE, &Timeout);
}



NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Unloading T1!\n");
	IoDeleteSymbolicLink(&rDosDevices);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath)
{
	ThreadProc(pDriverObject);
	DbgPrintEx(0, 0, "T1 Loaded!\n");


	RtlInitUnicodeString(&rDevices, L"\\Device\\T1");
	RtlInitUnicodeString(&rDosDevices, L"\\DosDevices\\T1");

	IoCreateSymbolicLink(&rDosDevices, &rDevices);


	pDriverObject->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
