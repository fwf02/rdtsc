#include <ntddk.h>
#include <intrin.h>
PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING rDevices, rDosDevices;






VOID readpmc(PDRIVER_OBJECT pDriverObject)
{
	KeStallExecutionProcessor(1);
	ULONG64 StartTime, EndTime;
	StartTime = __readpmc(0); // test __readpmc(1); as well
	EndTime = StartTime + KeGetPcr()->StallScaleFactor * 1;
	return EndTime;
}




NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Unloading readpmc!\n");
	IoDeleteSymbolicLink(&rDosDevices);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath) 
{
	Timer(pDriverObject);

	DbgPrintEx(0, 0, "readpmc Loaded!\n");


	RtlInitUnicodeString(&rDevices, L"\\Device\\readpmc");
	RtlInitUnicodeString(&rDosDevices, L"\\DosDevices\\readpmc");

	IoCreateSymbolicLink(&rDosDevices, &rDevices);

	
	pDriverObject->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
