#include <ntddk.h>


PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING rDevices, rDosDevices;





VOID Timer(PDRIVER_OBJECT pDriverObject)
{
	KeSetSystemAffinityThread(0); // 0 for HyperTherading On / 1 for HyperTherading off
	KeStallExecutionProcessor(1);
	ULONG64 StartTime, EndTime;
	StartTime = __rdtsc();
	EndTime = StartTime + KeGetPcr()->StallScaleFactor * 1;
	return EndTime;
}




NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Unloading Timer!\n");
	IoDeleteSymbolicLink(&rDosDevices);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath) 
{
	Timer(pDriverObject);

	DbgPrintEx(0, 0, "Timer Loaded!\n");


	RtlInitUnicodeString(&rDevices, L"\\Device\\Timer");
	RtlInitUnicodeString(&rDosDevices, L"\\DosDevices\\Timer");

	IoCreateSymbolicLink(&rDosDevices, &rDevices);

	
	pDriverObject->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}