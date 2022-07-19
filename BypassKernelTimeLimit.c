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


#define extremenanoseconds		100000000

ULONGLONG	test()
{
	KeSetPriorityThread(PsGetCurrentThread(), HIGH_PRIORITY - 1);
	KIRQL oldIrql;
	KeRaiseIrql(HIGH_LEVEL, &oldIrql);
	ULONGLONG counter;
	ULONGLONG freq;
	LONGLONG  Ticks;
	counter = (ULONGLONG)KeQueryPerformanceCounter((PLARGE_INTEGER)&freq).QuadPart;
	counter = counter * extremenanoseconds / freq;
	Ticks = (LONGLONG)(counter & 0xFFFFFFFF);
	Sleepx(1);
	KeLowerIrql(oldIrql);
	return Ticks;
}


NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Unloading test!\n");
	IoDeleteSymbolicLink(&rDosDevices);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath)
{
	test();
	DbgPrintEx(0, 0, "test Loaded!\n");


	RtlInitUnicodeString(&rDevices, L"\\Device\\test");
	RtlInitUnicodeString(&rDosDevices, L"\\DosDevices\\test");

	IoCreateSymbolicLink(&rDosDevices, &rDevices);


	pDriverObject->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
