#pragma once
#include <wdm.h>

NTSTATUS SirenHvIrpCreate(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp);

NTSTATUS SirenHvIrpClose(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp);

NTSTATUS SirenHvIrpDeviceCtrl(_In_ PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp);

NTSTATUS SirenHvIrpNotImplemented(PDEVICE_OBJECT DeviceObject, _Inout_ PIRP Irp);
