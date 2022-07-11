#include <ntddk.h>
#include <stdint.h>


// Thanks to
 // https://community.osr.com/discussion/10078/how-to-query-time-elapsed
// https://cpp.hotexamples.com/
// https://stackoverflow.com/questions/1583196/building-visual-c-app-that-doesnt-use-crt-functions-still-references-some


#ifdef __cplusplus
extern "C" {
#endif
	int _fltused = 0; // it should be a single underscore since the double one is the mangled name
#ifdef __cplusplus
}
#endif

PDEVICE_OBJECT pDeviceObject;
UNICODE_STRING rDevices, rDosDevices;

unsigned long getSystemTime()
{
	ULARGE_INTEGER InterruptTime, time;
	KeQuerySystemTime(&time);
	InterruptTime.QuadPart = KeQueryInterruptTime();
	return (unsigned long)((time.QuadPart / 10000000) - 11643609600);
}

#if !defined(_KS_)

//
// Performs a x*y/z operation on 64 bit quantities by splitting the operation. The equation
// is simplified with respect to adding in the remainder for the upper 32 bits.
//
// (xh * 10000000 / Frequency) * 2^32 + ((((xh * 10000000) % Frequency) * 2^32 + (xl * 10000000)) / Frequency)
//
#define NANOSECONDS 10000000
#define KSCONVERT_PERFORMANCE_TIME(Frequency, PerformanceTime) ((((ULONGLONG)(ULONG)(PerformanceTime).HighPart * NANOSECONDS / (Frequency)) << 32) + ((((((ULONGLONG)(ULONG)(PerformanceTime).HighPart * NANOSECONDS) % (Frequency)) << 32) + ((ULONGLONG)(PerformanceTime).LowPart * NANOSECONDS)) / (Frequency)))



#endif

void v() {
	ULONGLONG timeIncrement;
	timeIncrement = KeQueryTimeIncrement(); // 100 ns units
	void start(void);
	void stop(void);
	ULONG get(void);
	ULONG stopAndGet(void);
	// ULONGLONG startTime;
	// ULONGLONG endTime;
	LARGE_INTEGER endTime;
}

typedef * PTimeDelta;

inline void
start(void)
{
	LARGE_INTEGER startTime, rate;
	startTime = KeQueryPerformanceCounter(&rate);
	// KeQueryTickCount((PLARGE_INTEGER) &startTime);
}

inline void
stop(void)
{
	LARGE_INTEGER endTime, rate;
	endTime = KeQueryPerformanceCounter(&rate);

	//KeQueryTickCount((PLARGE_INTEGER) &endTime);
}

inline ULONG
get(void)
{
	KIRQL OldIrql2;
	KeRaiseIrql(HIGH_LEVEL, &OldIrql2);
	LARGE_INTEGER startTime = { 0 };
	LARGE_INTEGER rate = { 0 };
	LARGE_INTEGER endTime = { 0 };
	LARGE_INTEGER delta;

	delta.QuadPart = endTime.QuadPart - startTime.QuadPart;

	delta.QuadPart = KSCONVERT_PERFORMANCE_TIME(rate.QuadPart, delta);

	//delta *= timeIncrement; // time in 100 ns unit

	delta.QuadPart /= 10000;
	KeLowerIrql(OldIrql2);
	return (ULONG)delta.LowPart;
}


inline ULONG
stopAndGet(void)
{
	stop();
	return get();
}

NTSTATUS UnloadDriver(PDRIVER_OBJECT pDriverObject)
{
	DbgPrintEx(0, 0, "Unloading TimerFix!\n");
	IoDeleteSymbolicLink(&rDosDevices);
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegPath)
{
	getSystemTime();
	v();
	stopAndGet();
	DbgPrintEx(0, 0, "TimerFix Loaded!\n");


	RtlInitUnicodeString(&rDevices, L"\\Device\\TimerFix");
	RtlInitUnicodeString(&rDosDevices, L"\\DosDevices\\TimerFix");

	IoCreateSymbolicLink(&rDosDevices, &rDevices);


	pDriverObject->DriverUnload = UnloadDriver;

	return STATUS_SUCCESS;
}
