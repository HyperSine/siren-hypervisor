#pragma once
#include <wdm.h>

DECLARE_GLOBAL_CONST_UNICODE_STRING(g_DeviceName, L"\\Device\\siren-hv");
DECLARE_GLOBAL_CONST_UNICODE_STRING(g_DeviceDosName, L"\\DosDevices\\siren-hv");

inline PDRIVER_OBJECT g_DriverObject;
inline PDEVICE_OBJECT g_DeviceObject;

extern "C"
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath);

VOID SirenHvDriverUnload(_In_ PDRIVER_OBJECT DriverObject);
