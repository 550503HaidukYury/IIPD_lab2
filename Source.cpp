#include <windows.h>
#include <iostream>
#include <vector>
#include <list>
#include <Ntddscsi.h>
#include <stdint.h>
#include <bitset>
#define wszDrive L"\\\\.\\C:"


BOOL GetDriveGeometry(LPWSTR wszPath, DISK_GEOMETRY *pdg)
{
	HANDLE hDevice = INVALID_HANDLE_VALUE;  // handle to the drive to be examined 
	BOOL bResult = FALSE;                 // results flag
	DWORD junk = 0;                     // discard results

	hDevice = CreateFileW(wszPath,          // drive to open
		0,                // no access to the drive
		FILE_SHARE_READ | // share mode
		FILE_SHARE_WRITE,
		NULL,             // default security attributes
		OPEN_EXISTING,    // disposition
		0,                // file attributes
		NULL);            // do not copy file attributes

	if (hDevice == INVALID_HANDLE_VALUE)    // cannot open the drive
	{
		return (FALSE);
	}

	bResult = DeviceIoControl(hDevice,                       // device to be queried
		IOCTL_DISK_GET_DRIVE_GEOMETRY, // operation to perform
		NULL, 0,                       // no input buffer
		pdg, sizeof(*pdg),            // output buffer
		&junk,                         // # bytes returned
		(LPOVERLAPPED)NULL);          // synchronous I/O
	CloseHandle(hDevice);
	return (bResult);
}

int main()
{
	setlocale(LC_ALL, "RUS");

//	HDEVINFO hDevInfo;
//	SP_DEVINFO_DATA DeviceInfoData;
//	DWORD i;
//	// Create a HDEVINFO with all present devices.
//	static GUID GUID_DEVINTERFACE_USB_DEVICE = { 0xA5DCBF10L, 0x6530, 0x11D2,
//	{ 0x90, 0x1F, 0x00, 0xC0, 0x4F, 0xB9, 0x51, 0xED } };
//	
//	//GUID_DEVINTERFACE_DISK
//
//	hDevInfo = SetupDiGetClassDevs(NULL,
//		"IDE", // Enumerator
//		0,
//		DIGCF_ALLCLASSES | DIGCF_PRESENT);
//	//hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_VOLUME,
//	//	0, // Enumerator
//	//	0,
//	//	DIGCF_ALLCLASSES | DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
//
//	if (hDevInfo == INVALID_HANDLE_VALUE)
//	{
//		// Insert error handling here.
//		return 1;
//	}
//	// Enumerate through all devices in Set.
//	DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
//	for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i,
//		&DeviceInfoData); i++)
//	{
//		DWORD DataT;
//		LPTSTR buffer = NULL;
//		DWORD buffersize = 0;
//		//
//		// Call function with null to begin with, 
//		// then use the returned buffer size (doubled)
//		// to Alloc the buffer. Keep calling until
//		// success or an unknown failure.
//		//
//		//  Double the returned buffersize to correct
//		//  for underlying legacy CM functions that 
//		//  return an incorrect buffersize value on 
//		//  DBCS/MBCS systems.
//		// 
//		wchar_t *hardwareID;
//		/*// First get requiredLength
//		SetupDiGetDeviceRegistryProperty(deviceInfoList, &deviceInfoData, SPDRP_HARDWAREID, NULL, NULL, 0, &requiredLength);
//
//		hardwareID = (wchar_t*)(new char[requiredLength]());
//
//		// Second call to populate hardwareID
//		SetupDiGetDeviceRegistryProperty(deviceInfoList, &deviceInfoData, SPDRP_HARDWAREID, NULL, (PBYTE)hardwareID, requiredLength, NULL);
//		*/
//		while (!SetupDiGetDeviceRegistryProperty(
//			hDevInfo,
//			&DeviceInfoData,
//			SPDRP_FRIENDLYNAME, //SPDRP_DEVICEDESC||SPDRP_DEVTYPE-for device info //SPDRP_FRIENDLYNAME - device_name
//			&DataT,
//			(PBYTE)buffer,
//			buffersize,
//			&buffersize))
//		{
//			if (GetLastError() ==
//				ERROR_INSUFFICIENT_BUFFER)
//			{
//				// Change the buffer size.
//				if (buffer) LocalFree(buffer);
//				// Double the size to avoid problems on 
//				// W2k MBCS systems per KB 888609. 
//				buffer = (LPTSTR)LocalAlloc(LPTR, buffersize * 2);
//			}
//			else
//			{
//				// Insert error handling here.
//				break;
//			}
//		}
//		printf("Result:[%s]\n", buffer);
////		if (buffer) LocalFree(buffer);
//
//		while (!SetupDiGetDeviceRegistryProperty(
//			hDevInfo,
//			&DeviceInfoData,
//			SPDRP_HARDWAREID, //firmvare version
//			&DataT,
//			(PBYTE)buffer,
//			buffersize,
//			&buffersize))
//		{
//			if (GetLastError() ==
//				ERROR_INSUFFICIENT_BUFFER)
//			{
//				// Change the buffer size.
//				if (buffer) LocalFree(buffer);
//				// Double the size to avoid problems on 
//				// W2k MBCS systems per KB 888609. 
//				buffer = (LPTSTR)LocalAlloc(LPTR, buffersize * 2);
//			}
//			else
//			{
//				// Insert error handling here.
//				break;
//			}
//		}
//		printf("Result:[%s]\n", buffer);
//		if (buffer) LocalFree(buffer);
//	}
//	if (GetLastError() != NO_ERROR &&
//		GetLastError() != ERROR_NO_MORE_ITEMS)
//	{
//		// Insert error handling here.
//		return 1;
//	}
//	//  Cleanup
//	SetupDiDestroyDeviceInfoList(hDevInfo);
//
	
	using namespace std;

	vector<wstring> Files;
		
		DISK_GEOMETRY pdg = { 0 }; // disk drive geometry structure
		BOOL bResult = FALSE;      // generic results flag
		bResult = GetDriveGeometry(wszDrive, &pdg);
		/*
		TCHAR volumeName[MAX_PATH + 1] = { 0 };
		TCHAR fileSystemName[MAX_PATH + 1] = { 0 };
		DWORD serialNumber = 0;
		DWORD maxComponentLen = 0;
		DWORD fileSystemFlags = 0;
		if (GetVolumeInformation(
			"C:\\",
			volumeName,
			ARRAYSIZE(volumeName),
			&serialNumber,
			&maxComponentLen,
			&fileSystemFlags,
			fileSystemName,
			ARRAYSIZE(fileSystemName)))
			cout <<"HardDrive serialnumber from volume info: " << serialNumber << endl;
		
		system("wmic path win32_physicalmedia get SerialNumber");
		*/
		

		//getATAsupport
		BOOL ataSupport = FALSE;      // generic results flag
		DWORD dwBytes;

		CONST UINT bufferSize = 512;
		CONST BYTE identifyDataCommandId = 0xEC;

		UCHAR identifyDataBuffer[bufferSize
			+ sizeof(ATA_PASS_THROUGH_EX)] = { 0 };

		ATA_PASS_THROUGH_EX & PTE = *(ATA_PASS_THROUGH_EX *)identifyDataBuffer;
		PTE.Length = sizeof(PTE);
		PTE.TimeOutValue = 10;
		PTE.DataTransferLength = 512;
		PTE.DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX);

		IDEREGS * ideRegs = (IDEREGS *)PTE.CurrentTaskFile;
		ideRegs->bCommandReg = identifyDataCommandId;
		ideRegs->bSectorCountReg = 1;

		PTE.AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;
		
		HANDLE hDevice(::CreateFileW(L"\\\\.\\PhysicalDrive0", GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL));
		if (hDevice == INVALID_HANDLE_VALUE)
		{
			std::wcout << L"CreateFileW( " << "here" << L" ) failed.  LastError: " << GetLastError() << std::endl;
			return -1;
		}
		bResult = DeviceIoControl(hDevice, IOCTL_ATA_PASS_THROUGH, &PTE,
			sizeof(identifyDataBuffer), &PTE,
			sizeof(identifyDataBuffer), &dwBytes, 0);
		if (bResult == FALSE) {
			std::cout << "Oops, something went wrong, error code: "
				<< GetLastError() << std::endl;
			return bResult;
		}
		WORD *data = (WORD *)(identifyDataBuffer + sizeof(ATA_PASS_THROUGH_EX));
		
		int16_t ataSupportBits = data[80];
		int16_t dmaSupportBits = data[63];
		int16_t pioSupportBits = data[64];
		int16_t ultraDmaSupportBits = data[62];
		
		//devicemodel
		cout << "DEVICE MODEL: \t";
		int16_t devicemodel[20];
		for (int i = 0; i < 20; i++)
			devicemodel[i] = data[27 + i];
		for (int i = 0; i < 20; i++)
		{
			cout << (char)(devicemodel[i] >> 8);
			cout << (char)(devicemodel[i]);

		}
		cout << endl;
		//cout << devicemodel << endl;

		//deviceFirmware
		cout << "Firmware: \t";
		int16_t Firmware[4];
		for (int i = 0; i < 4; i++)
			Firmware[i] = data[23 + i];
		for (int i = 0; i < 4; i++)
		{
			cout << (char)(Firmware[i] >> 8);
			cout << (char)(Firmware[i]);
		}
		cout << endl;
		//deviceSerial
		cout << "DEVICE SERIAL: \t";
		int16_t deviceSerial[10];
		for (int i = 0; i < 10; i++)
			deviceSerial[i] = data[10 + i];
		for (int i = 0; i < 10; i++)
		{
			cout << (char)(deviceSerial[i] >> 8);
			cout << (char)(deviceSerial[i]);
		}
		cout << endl;
		//cout << deviceSerial << endl;

		//deviceVolume
		typedef BOOL(WINAPI *P_GDFSE)(LPCTSTR, PULARGE_INTEGER,
			PULARGE_INTEGER, PULARGE_INTEGER);
		//GET MEMORYiNFO
		LPCSTR pszDrive = "C:";
		BOOL test, fResult;
		unsigned __int64 i64FreeBytesToCaller,
			i64TotalBytes,
			i64FreeBytes;
		P_GDFSE pGetDiskFreeSpaceEx = NULL;
		pGetDiskFreeSpaceEx = (P_GDFSE)GetProcAddress(
			GetModuleHandle("kernel32.dll"),
			"GetDiskFreeSpaceExA");
		test = pGetDiskFreeSpaceEx(pszDrive,
			(PULARGE_INTEGER)&i64FreeBytesToCaller,
			(PULARGE_INTEGER)&i64TotalBytes,
			(PULARGE_INTEGER)&i64FreeBytes);
		if (test)
		{
			printf("Free space on drive       = %I64u MB;",
				i64FreeBytes / (1024 * 1024));
			printf("\tOccupied space = %I64u MB;",
				(i64TotalBytes - i64FreeBytes) / (1024 * 1024));
			printf("\tTotal space               = %I64u MB\n",
				i64TotalBytes / (1024 * 1024));
		}

		//ata
		bitset<16> mask(ataSupportBits);
		//std::cout << mask << endl;
		string ataSupportStr = "Supported_ATA: ";
		if ((mask[2]))
		{
			ataSupportStr += "ATA/atapi-4,";
		}
		if ((mask[3]))
		{
			ataSupportStr += "ATA/atapi-5,";
		}
		if ((mask[4]))
		{
			ataSupportStr += "ATA/atapi-6,";
		}
		if ((mask[5]))
		{
			ataSupportStr += "ATA/atapi-7,";
		}
		if ((mask[6]))
		{
			ataSupportStr += "ATA8-ACS";
		}
		cout << ataSupportStr << endl;
		//dma
		bitset<16> maskDma(dmaSupportBits);
		//std::cout << maskDma << endl;
		string dmaSupportStr = "Supported_DMA: ";
		if (maskDma[0])
		{
			dmaSupportStr += "MultiDMA-0,";
		}
		if (maskDma[1])
		{
			dmaSupportStr += "MultiDMA-1,";
		}
		if (maskDma[2])
		{
			dmaSupportStr += "MultiDMA-2,";
		}
		if (maskDma[3])
		{
			dmaSupportStr += "MultiDMA-3,";
		}
		if (maskDma[4])
		{
			dmaSupportStr += "MultiDMA-4,";
		}
		if (maskDma[5])
		{
			dmaSupportStr += "MultiDMA-5,";
		}
		if (maskDma[6])
		{
			dmaSupportStr += "MultiDMA-6,";
		}
		if (maskDma[7])
		{
			dmaSupportStr += "MultiDMA-7";
		}
		cout << dmaSupportStr << endl;
		//pio
		bitset<16> maskPio(pioSupportBits);
		//std::cout << maskPio << endl;
		string pioSupportStr = "Supported_PIO: ";
		if (maskPio[0])
		{
			pioSupportStr += "PIO-3,";
		}
		if (maskPio[1])
		{
			pioSupportStr += "PIO-4";
		}
		cout << pioSupportStr << endl;
		/*//serialnumber is here
		for (int i = 10; i < 20; i++)
		{
			printf("%c", (char)data[i]);
		}*/
		system("pause");
		return 0;
	return 0;
}


BOOL getAtaCompliance(HANDLE hDevice) {
	// HANDLE hDevice = INVALID_HANDLE_VALUE;
	DWORD dwBytes;
	BOOL  bResult;

	CONST UINT bufferSize = 512;
	CONST BYTE identifyDataCommandId = 0xEC;

	UCHAR identifyDataBuffer[bufferSize
		+ sizeof(ATA_PASS_THROUGH_EX)] = { 0 };

	ATA_PASS_THROUGH_EX & PTE = *(ATA_PASS_THROUGH_EX *)identifyDataBuffer;
	PTE.Length = sizeof(PTE);
	PTE.TimeOutValue = 10;
	PTE.DataTransferLength = 512;
	PTE.DataBufferOffset = sizeof(ATA_PASS_THROUGH_EX);

	IDEREGS * ideRegs = (IDEREGS *)PTE.CurrentTaskFile;
	ideRegs->bCommandReg = identifyDataCommandId;
	ideRegs->bSectorCountReg = 1;

	PTE.AtaFlags = ATA_FLAGS_DATA_IN | ATA_FLAGS_DRDY_REQUIRED;

	bResult = DeviceIoControl(hDevice, IOCTL_ATA_PASS_THROUGH, &PTE,
		sizeof(identifyDataBuffer), &PTE,
		sizeof(identifyDataBuffer), &dwBytes, 0);

	if (bResult == FALSE) {
		std::cout << "Oops, something went wrong, error code: "
			<< GetLastError() << std::endl;
		return bResult;
	}

	WORD *data = (WORD *)(identifyDataBuffer + sizeof(ATA_PASS_THROUGH_EX));

	int16_t ataSupportBits = data[80];

	return bResult;
}