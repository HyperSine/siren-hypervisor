#include <ntifs.h>
#include "driver.hpp"
#include "driver_irp_handler.hpp"
#include "siren/siren_memory.hpp"
#include "siren/hypervisors/vmx/mshv_hypervisor.hpp"
#include "siren/x86/paging.hpp"

siren::hypervisors::vmx::mshv_hypervisor* g_IntelHypervisor;

extern "C"
NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
    UNREFERENCED_PARAMETER(RegistryPath);

    NTSTATUS nt_status = STATUS_SUCCESS;

    // siren::managed_object_ptr<siren::hypervisors::vmx::mshv_hypervisor> intel_hypervisor = nullptr;

    PDEVICE_OBJECT siren_dev_object = nullptr;
    BOOLEAN siren_dev_symboliclinked = FALSE;

    //intel_hypervisor = siren::make_managed_object<false, siren::hypervisors::vmx::mshv_hypervisor>();
    //if (!intel_hypervisor) {
    //    nt_status = STATUS_INSUFFICIENT_RESOURCES;
    //    goto ON_FINAL;
    //}

    //if (auto status = intel_hypervisor->init(); status.fails()) {
    //    nt_status = status.to_native_error<NTSTATUS>();
    //    goto ON_FINAL;
    //}

    //{
    //    KAPC_STATE previous_kapc_state;
    //    KeStackAttachProcess(PsInitialSystemProcess, &previous_kapc_state);
    //    intel_hypervisor->start();
    //    KeUnstackDetachProcess(&previous_kapc_state);
    //}

    nt_status = IoCreateDevice(DriverObject, 0, (PUNICODE_STRING)&g_DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, &siren_dev_object);
    if (!NT_SUCCESS(nt_status)) {
        goto ON_FINAL;
    }

    nt_status = IoCreateSymbolicLink((PUNICODE_STRING)&g_DeviceDosName, (PUNICODE_STRING)&g_DeviceName);
    if (!NT_SUCCESS(nt_status)) {
        goto ON_FINAL;
    } else {
        siren_dev_symboliclinked = TRUE;
    }

    for (auto& major_fn : DriverObject->MajorFunction) {
        major_fn = SirenHvIrpNotImplemented;
    }
    DriverObject->MajorFunction[IRP_MJ_CREATE] = SirenHvIrpCreate;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = SirenHvIrpClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = SirenHvIrpDeviceCtrl;
    DriverObject->DriverUnload = SirenHvDriverUnload;

    // g_IntelHypervisor = intel_hypervisor.transfer();
    // intel_hypervisor = nullptr;

    (void)siren::x86::get_max_physical_address();

    std::swap(g_DeviceObject, siren_dev_object);

    g_DriverObject = DriverObject;

ON_FINAL:
    if (!NT_SUCCESS(nt_status) && siren_dev_symboliclinked) {
        IoDeleteSymbolicLink((PUNICODE_STRING)&g_DeviceDosName);
        siren_dev_symboliclinked = FALSE;
    }

    if (siren_dev_object) {
        IoDeleteDevice(siren_dev_object);
        siren_dev_object = nullptr;
    }

    return nt_status;
}

VOID SirenHvDriverUnload(_In_ PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);

    IoDeleteSymbolicLink((PUNICODE_STRING)&g_DeviceDosName);

    if (g_DeviceObject) {
        IoDeleteDevice(g_DeviceObject);
        g_DeviceObject = nullptr;
    }

    //if (g_IntelHypervisor) {
    //    siren::generic_delete<siren::hypervisors::abstract_hypervisor>{}(g_IntelHypervisor);
    //    g_IntelHypervisor = nullptr;
    //}
}
