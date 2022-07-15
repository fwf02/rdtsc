#include <ntddk.h>
#include <stdint.h>

PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING rDevices, rDosDevices;

// Thanks to

// @vRare @wongfei

NTSTATUS Sleepx(ULONGLONG milliseconds)
{
	LARGE_INTEGER delay;
	ULONG* split;

	milliseconds *= 1000000;

	milliseconds /= 100;

	milliseconds = -milliseconds;

	split = (ULONG*)&milliseconds;

	delay.LowPart = *split;

	split++;

	delay.HighPart = *split;


	KeDelayExecutionThread(KernelMode, 0, &delay);

	return STATUS_SUCCESS;
}


NTSTATUS BypassKernelTimeLimit(PDRIVER_OBJECT pDriverObject)
{
	LARGE_INTEGER Freq, Timeout, t0, t1;
	LONGLONG Elapsed;
	t0 = KeQueryPerformanceCounter(&Freq);
	t1 = KeQueryPerformanceCounter(&Freq);
	Elapsed = ((t1.QuadPart - t0.QuadPart) * 100000000) / Freq.QuadPart; // extreme precision
	Sleepx(1);
	return 0;
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Unloading test!\n");
	IoDeleteSymbolicLink(&rDosDevices);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath)
{
	BypassKernelTimeLimit(pDriverObject);
	DbgPrintEx(0, 0, "test Loaded!\n");


	RtlInitUnicodeString(&rDevices, L"\\Device\\test");
	RtlInitUnicodeString(&rDosDevices, L"\\DosDevices\\test");

	IoCreateSymbolicLink(&rDosDevices, &rDevices);


	pDriverObject->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
