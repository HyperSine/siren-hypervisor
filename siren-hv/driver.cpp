#include <ntifs.h>
#include "driver.hpp"
#include "driver_irp_handler.hpp"

#include <algorithm>
#include "siren/vmx/mshv_hypervisor.hpp"
#include "siren/vmx/mshv_virtual_cpu.hpp"

static siren::hypervisor* g_SirenHypervisor;

extern "C"
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS status;
    PDEVICE_OBJECT siren_dev_object = nullptr;
    BOOLEAN siren_dev_symboliclinked = FALSE;

    {
        auto expt_hypervisor = siren::allocate_unique<siren::vmx::mshv_hypervisor>(siren::npaged_pool);
        if (expt_hypervisor.has_error()) {
            status = expt_hypervisor.error().value;
            goto ON_FINAL;
        }

        auto expt_status = expt_hypervisor.value()->intialize();
        if (expt_status.has_error()) {
            status = expt_status.error().value;
            goto ON_FINAL;
        }

        {
            KAPC_STATE previous_kapc_state;
            KeStackAttachProcess(PsInitialSystemProcess, &previous_kapc_state);
            expt_hypervisor.value()->start();
            KeUnstackDetachProcess(&previous_kapc_state);
        }

        g_SirenHypervisor = expt_hypervisor.value().release();
    }

    status = IoCreateDevice(DriverObject, 0, (PUNICODE_STRING)&g_DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &siren_dev_object);
    if (!NT_SUCCESS(status)) {
        goto ON_FINAL;
    }

    status = IoCreateSymbolicLink((PUNICODE_STRING)&g_DeviceDosName, (PUNICODE_STRING)&g_DeviceName);
    if (!NT_SUCCESS(status)) {
        goto ON_FINAL;
    } else {
        siren_dev_symboliclinked = TRUE;
    }

    std::ranges::fill(DriverObject->MajorFunction, SirenHvIrpNotImplemented);

    DriverObject->MajorFunction[IRP_MJ_CREATE] = SirenHvIrpCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = SirenHvIrpClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = SirenHvIrpDeviceCtrl;
    DriverObject->DriverUnload = SirenHvDriverUnload;

    g_DriverObject = DriverObject;

ON_FINAL:
    if (!NT_SUCCESS(status) && siren_dev_symboliclinked) {
        IoDeleteSymbolicLink((PUNICODE_STRING)&g_DeviceDosName);
        siren_dev_symboliclinked = FALSE;
    }

    if (siren_dev_object) {
        IoDeleteDevice(siren_dev_object);
        siren_dev_object = nullptr;
    }

    return status;
}

VOID SirenHvDriverUnload(_In_ PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);

    IoDeleteSymbolicLink((PUNICODE_STRING)&g_DeviceDosName);

    if (g_DeviceObject) {
        IoDeleteDevice(g_DeviceObject);
        g_DeviceObject = nullptr;
    }

    if (g_SirenHypervisor) {
        g_SirenHypervisor->stop();
        siren::allocator_delete(siren::npaged_pool, g_SirenHypervisor);
        g_SirenHypervisor = nullptr;
    }
}
