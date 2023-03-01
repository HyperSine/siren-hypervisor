#pragma once
#include "tlfs.hpp"

namespace siren::microsoft_hv {

    namespace hypercalls {

        enum class call_code_e : uint16_t {
            HvSwitchVirtualAddressSpace = 0x0001,
            HvFlushVirtualAddressSpace = 0x0002,
            HvFlushVirtualAddressList = 0x0003,
            HvGetLogicalProcessorRunTime = 0x0004,
            // 0x0005..0x0007 are reserved
            HvCallNotifyLongSpinWait = 0x0008,
            HvCallParkedVirtualProcessors = 0x0009,
            HvCallSyntheticClusterIpi = 0x000B,
            HvCallModifyVtlProtectionMask = 0x000C,
            HvCallEnablePartitionVtl = 0x000D,
            HvCallDisablePartitionVtl = 0x000E,
            HvCallEnableVpVtl = 0x000F,
            HvCallDisableVpVtl = 0x0010,
            HvCallVtlCall = 0x0011,
            HvCallVtlReturn = 0x0012,
            HvCallFlushVirtualAddressSpaceEx = 0x0013,
            HvCallFlushVirtualAddressListEx = 0x0014,
            HvCallSendSyntheticClusterIpiEx = 0x0015,
            // 0x0016..0x003F are reserved
            HvCreatePartition = 0x0040,
            HvInitializePartition = 0x0041,
            HvFinalizePartition = 0x0042,
            HvDeletePartition = 0x0043,
            HvGetPartitionProperty = 0x0044,
            HvSetPartitionProperty = 0x0045,
            HvGetPartitionId = 0x0046,
            HvGetNextChildPartition = 0x0047,
            HvDepositMemory = 0x0048,
            HvWithdrawMemory = 0x0049,
            HvGetMemoryBalance = 0x004A,
            HvMapGpaPages = 0x004B,
            HvUnmapGpaPages = 0x004C,
            HvInstallIntercept = 0x004D,
            HvCreateVp = 0x004E,
            HvDeleteVp = 0x004F,
            HvGetVpRegisters = 0x0050,
            HvSetVpRegisters = 0x0051,
            HvTranslateVirtualAddress = 0x0052,
            HvReadGpa = 0x0053,
            HvWriteGpa = 0x0054,
            // 0x0055 is deprecated
            HvClearVirtualInterrupt = 0x0056,
            // 0x0057 is deprecated
            HvDeletePort = 0x0058,
            HvConnectPort = 0x0059,
            HvGetPortProperty = 0x005A,
            HvDisconnectPort = 0x005B,
            HvPostMessage = 0x005C,
            HvSignalEvent = 0x005D,
            HvSavePartitionState = 0x005E,
            HvRestorePartitionState = 0x005F,
            HvInitializeEventLogBufferGroup = 0x0060,
            HvFinalizeEventLogBufferGroup = 0x0061,
            HvCreateEventLogBuffer = 0x0062,
            HvDeleteEventLogBuffer = 0x0063,
            HvMapEventLogBuffer = 0x0064,
            HvUnmapEventLogBuffer = 0x0065,
            HvSetEventLogGroupSources = 0x0066,
            HvReleaseEventLogBuffer = 0x0067,
            HvFlushEventLogBuffer = 0x0068,
            HvPostDebugData = 0x0069,
            HvRetrieveDebugData = 0x006A,
            HvResetDebugSession = 0x006B,
            HvMapStatsPage = 0x006C,
            HvUnmapStatsPage = 0x006D,
            HvCallMapSparseGpaPages = 0x006E,
            HvCallSetSystemProperty = 0x006F,
            HvCallSetPortProperty = 0x0070,
            // 0x0071..0x0075 are reserved
            HvCallAddLogicalProcessor = 0x0076,
            HvCallRemoveLogicalProcessor = 0x0077,
            HvCallQueryNumaDistance = 0x0078,
            HvCallSetLogicalProcessorProperty = 0x0079,
            HvCallGetLogicalProcessorProperty = 0x007A,
            HvCallGetSystemProperty = 0x007B,
            HvCallMapDeviceInterrupt = 0x007C,
            HvCallUnmapDeviceInterrupt = 0x007D,
            HvCallRetargetDeviceInterrupt = 0x007E,
            // 0x007F is reserved
            HvCallMapDevicePages = 0x0080,
            HvCallUnmapDevicePages = 0x0081,
            HvCallAttachDevice = 0x0082,
            HvCallDetachDevice = 0x0083,
            HvCallNotifyStandbyTransition = 0x0084,
            HvCallPrepareForSleep = 0x0085,
            HvCallPrepareForHibernate = 0x0086,
            HvCallNotifyPartitionEvent = 0x0087,
            HvCallGetLogicalProcessorRegisters = 0x0088,
            HvCallSetLogicalProcessorRegisters = 0x0089,
            HvCallQueryAssotiatedLpsforMca = 0x008A,
            HvCallNotifyRingEmpty = 0x008B,
            HvCallInjectSyntheticMachineCheck = 0x008C,
            HvCallScrubPartition = 0x008D,
            HvCallCollectLivedump = 0x008E,
            HvCallDisableHypervisor = 0x008F,
            HvCallModifySparseGpaPages = 0x0090,
            HvCallRegisterInterceptResult = 0x0091,
            HvCallUnregisterInterceptResult = 0x0092,
            HvCallAssertVirtualInterrupt = 0x0094,
            HvCallCreatePort = 0x0095,
            HvCallConnectPort = 0x0096,
            HvCallGetSpaPageList = 0x0097,
            // 0x0098 is reserved
            HvCallStartVirtualProcessor = 0x009A,
            HvCallGetVpIndexFromApicId = 0x009A,
            // 0x009A..0x00AE are reserved
            HvCallFlushGuestPhysicalAddressSpace = 0x00AF,
            HvCallFlushGuestPhysicalAddressList = 0x00B0
        };

        enum class status_code_e : uint16_t {
            HV_STATUS_SUCCESS = 0x0000,
            HV_STATUS_INVALID_HYPERCALL_CODE = 0x0002,
            HV_STATUS_INVALID_HYPERCALL_INPUT = 0x0003,
            HV_STATUS_INVALID_ALIGNMENT = 0x0004,
            HV_STATUS_INVALID_PARAMETER = 0x0005,
            HV_STATUS_ACCESS_DENIED = 0x0006,
            HV_STATUS_INVALID_PARTITION_STATE = 0x0007,
            HV_STATUS_OPERATION_DENIED = 0x0008,
            HV_STATUS_UNKNOWN_PROPERTY = 0x0009,
            HV_STATUS_PROPERTY_VALUE_OUT_OF_RANGE = 0x000A,
            HV_STATUS_INSUFFICIENT_MEMORY = 0x000B,
            HV_STATUS_PARTITION_TOO_DEEP = 0x000C,
            HV_STATUS_INVALID_PARTITION_ID = 0x000D,
            HV_STATUS_INVALID_VP_INDEX = 0x000E,
            HV_STATUS_INVALID_PORT_ID = 0x0011,
            HV_STATUS_INVALID_CONNECTION_ID = 0x0012,
            HV_STATUS_INSUFFICIENT_BUFFERS = 0x0033,
            HV_STATUS_NOT_ACKNOWLEDGED = 0x0014,
            HV_STATUS_INVALID_VP_STATE = 0x0015,
            HV_STATUS_ACKNOWLEDGED = 0x0016,
            HV_STATUS_INVALID_SAVE_RESTORE_STATE = 0x0017,
            HV_STATUS_INVALID_SYNIC_STATE = 0x0018,
            HV_STATUS_OBJECT_IN_USE = 0x0019,
            HV_STATUS_INVALID_PROXIMITY_DOMAIN_INFO = 0x001A,
            HV_STATUS_NO_DATA = 0x001B,
            HV_STATUS_INACTIVE = 0x001C,
            HV_STATUS_NO_RESOURCES = 0x001D,
            HV_STATUS_FEATURE_UNAVAILABLE = 0x001E,
            HV_STATUS_PARTIAL_PACKET = 0x001F,
            HV_STATUS_PROCESSOR_FEATURE_NOT_SUPPORTED = 0x0020,
            HV_STATUS_PROCESSOR_CACHE_LINE_FLUSH_SIZE_INCOMPATIBLE = 0x0030,
            HV_STATUS_INSUFFICIENT_BUFFER = 0x0033,
            HV_STATUS_INCOMPATIBLE_PROCESSOR = 0x0037,
            HV_STATUS_INSUFFICIENT_DEVICE_DOMAINS = 0x0038,
            HV_STATUS_CPUID_FEATURE_VALIDATION_ERROR = 0x003C,
            HV_STATUS_CPUID_XSAVE_FEATURE_VALIDATION_ERROR = 0x003D,
            HV_STATUS_PROCESSOR_STARTUP_TIMEOUT = 0x003E,
            HV_STATUS_SMX_ENABLED = 0x003F,
            HV_STATUS_INVALID_LP_INDEX = 0x0041,
            HV_STATUS_INVALID_REGISTER_VALUE = 0x0050,
            HV_STATUS_NX_NOT_DETECTED = 0x0055,
            HV_STATUS_INVALID_DEVICE_ID = 0x0057,
            HV_STATUS_INVALID_DEVICE_STATE = 0x0058,
            HV_STATUS_PENDING_PAGE_REQUESTS = 0x0059,
            HV_STATUS_PAGE_REQUEST_INVALID = 0x0060,
            HV_STATUS_OPERATION_FAILED = 0x0071,
            HV_STATUS_NOT_ALLOWED_WITH_NESTED_VIRT_ACTIVE = 0x0072
        };

        struct input_value_t {
            union {
                uint64_t storage;
                struct {
                    call_code_e call_code;
                    uint16_t fast : 1;
                    uint16_t variable_header_size : 10;
                    uint16_t reserved0 : 4;
                    uint16_t is_nested : 1;
                    uint16_t rep_count : 12;
                    uint16_t reserved1 : 4;
                    uint16_t rep_start_index : 12;
                    uint16_t reserved2 : 4;
                } semantics;
            };
        };

        static_assert(sizeof(input_value_t) == 8);
        static_assert(sizeof(input_value_t::storage) == sizeof(input_value_t::semantics));

        struct result_value_t {
            union {
                uint64_t storage;
                struct {
                    status_code_e result;
                    uint16_t reserved0;
                    uint32_t reps_completed : 12;
                    uint32_t reserved1 : 20;
                } semantics;
            };
        };

        static_assert(sizeof(result_value_t) == 8);
        static_assert(sizeof(result_value_t::storage) == sizeof(result_value_t::semantics));

        [[nodiscard]]
        result_value_t flush_virtual_address_space(address_space_id_t address_space, flush_flags_t flags, uint64_t processor_mask) noexcept;

        [[nodiscard]]
        result_value_t flush_guest_physical_address_space(spa_t address_space) noexcept;

        //[[nodiscard]]
        //result_value_t flush_guest_physical_address_list(spa_t address_space, /*todo*/) noexcept;

    }

}
