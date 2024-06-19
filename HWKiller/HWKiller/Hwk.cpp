#include "Hwk.hpp"

#define MH_SUCCESS(MHStatus) (MHStatus == MH_OK)

t_NtQuerySystemInformation orgNtQSI;

// change it to whatever you like
char uuidReplace[16] = { 0x31 ,0x33 ,0x33 ,0x37 ,0x31 ,0x33 ,0x33 ,0x37 ,0x31 ,0x33 ,0x33 ,0x37 ,0x31 ,0x33 ,0x33 ,0x37 };

VOID SpoofUUID(PBYTE UUID) {
	memcpy(UUID, uuidReplace, sizeof(uuidReplace));
}

NTSTATUS detourNTQSI (

	SYSTEM_INFORMATION_CLASS SystemInformationClass,
	PVOID                    SystemInformation,
	ULONG                    SystemInformationLength,
	PULONG                   ReturnLength

) {
	NTSTATUS orgStatus;
	if (SystemInformationClass == 0x4c) {

		PSYSTEM_FIRMWARE_TABLE_INFORMATION firmwareTableInfo = (PSYSTEM_FIRMWARE_TABLE_INFORMATION)SystemInformation;

		if (firmwareTableInfo->Action == SystemFirmwareTable_Get) {

			if (firmwareTableInfo->ProviderSignature == 'RSMB') {

				orgStatus = orgNtQSI(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
				PSMBIOSData buf = (PSMBIOSData)(firmwareTableInfo->TableBuffer);

				PSMBIOSData Smbios;
				PDMIHeader header;

				Smbios = (PSMBIOSData)buf;
				PBYTE tableData = Smbios->SMBIOSTableData;

				for (int i = 0; i < Smbios->Length; i++) {
					header = (PDMIHeader)tableData;

					if (header->type == 1) {
						PBYTE foundUUID = tableData + 0x8;
						SpoofUUID(foundUUID);
					}

					tableData += header->length;
					while ((*(WORD*)tableData) != 0) tableData++;
					tableData += 2;
				}
			}
		}
	}
	else {
		orgStatus = orgNtQSI(SystemInformationClass, SystemInformation, SystemInformationLength, ReturnLength);
	}

	return orgStatus;
}
BOOL InitMinHook() {
	return MH_SUCCESS(MH_Initialize());
}

BOOL InitNTQSIHook() {

	if (!MH_SUCCESS(MH_CreateHookApi(L"NTDLL", "NtQuerySystemInformation", detourNTQSI, (PVOID*)&orgNtQSI))) {
		return FALSE;
	}

	if (!MH_SUCCESS(MH_EnableHook(MH_ALL_HOOKS))) {
		return FALSE;
	}

	return TRUE;
}