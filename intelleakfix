#include <ntddk.h>
PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING rDevices, rDosDevices;



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
	DbgPrintEx(0, 0, "Unloading FixLeak!\n");
	IoDeleteSymbolicLink(&rDosDevices);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath) 
{
	intelfix(pDriverObject);

	DbgPrintEx(0, 0, "FixLeak Loaded!\n");


	RtlInitUnicodeString(&rDevices, L"\\Device\\FixLeak");
	RtlInitUnicodeString(&rDosDevices, L"\\DosDevices\\FixLeak");

	IoCreateSymbolicLink(&rDosDevices, &rDevices);

	
	pDriverObject->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
