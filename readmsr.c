#include <ntddk.h>
#include <intrin.h>
PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING rDevices, rDosDevices;






VOID readmsr(PDRIVER_OBJECT pDriverObject)
{
	KeSetSystemAffinityThread(0); // 0 for HyperThreading / 1 for off HyperThreading
	KeStallExecutionProcessor(1);
	ULONG64 StartTime, EndTime;
	StartTime = __readmsr(0x12); // test __readmsr(0x13); as well
	EndTime = StartTime + KeGetPcr()->StallScaleFactor * 1;
	return EndTime;
}




NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Unloading readmsr!\n");
	IoDeleteSymbolicLink(&rDosDevices);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath) 
{
	Timer(pDriverObject);

	DbgPrintEx(0, 0, "readmsr Loaded!\n");


	RtlInitUnicodeString(&rDevices, L"\\Device\\readmsr");
	RtlInitUnicodeString(&rDosDevices, L"\\DosDevices\\readmsr");

	IoCreateSymbolicLink(&rDosDevices, &rDevices);

	
	pDriverObject->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
