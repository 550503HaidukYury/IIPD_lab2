#define _SCL_SECURE_NO_WARNINGS 1
#include <windows.h>
#include <iostream>
#include <vector>
#include <list>
#include <Ntddscsi.h>
#include <stdint.h>
#include <bitset>
#include <string>
#include "winioctl.h"
#include "usbscan.h" 

#ifdef _UNICODE
typedef wchar_t TCHAR;
#else
typedef char TCHAR;
#endif

#define Name L"\\\\.\\"
#define ATABit 80
#define DMABit 63
#define PIOBit 64
CONST UINT bufferSize = 512;
CONST BYTE identifyDataCommandId = 0xEC;

using namespace std;

void getDeviceModel(WORD *data){
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
}

void getDeviceFirmware(WORD *data){
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
}

void getDeviceSerial(WORD *data){
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
}


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

WORD* GetDeviceIOControl(LPWSTR wszDrive_converted){
	HANDLE hDevice(::CreateFileW(wszDrive_converted, GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL));
	if (hDevice == INVALID_HANDLE_VALUE)
	{
		std::wcout << L"CreateFileW( " << "here" << L" ) failed.  LastError: " << GetLastError() << std::endl;
		system("pause");
		return FALSE;
	}
	BOOL bResult = FALSE;
	DWORD dwBytes;
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

		bResult = DeviceIoControl(hDevice, IOCTL_SCSI_PASS_THROUGH, &PTE,
			sizeof(identifyDataBuffer), &PTE,
			sizeof(identifyDataBuffer), &dwBytes, 0);
		if (bResult == FALSE) {
			std::cout << "Unknown disk device:"
				<< GetLastError() << std::endl;

		}
		std::cout << "Something went wrong:"
			<< GetLastError() << std::endl;

		system("pause");
		
	}
	WORD* data = (WORD *)(identifyDataBuffer + sizeof(ATA_PASS_THROUGH_EX));
	
	//showParameters

	//devicemodel
	getDeviceModel(data);
	//deviceFirmware
	getDeviceFirmware(data);
	//deviceSerial
	getDeviceSerial(data);

	return data;
}

std::string wstrtostr(const std::wstring &wstr)
{
	// Convert a Unicode string to an ASCII string
	std::string strTo;
	char *szTo = new char[wstr.length() + 1];
	szTo[wstr.size()] = '\0';
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), -1, szTo, (int)wstr.length(), NULL, NULL);
	strTo = szTo;
	delete[] szTo;
	return strTo;
}

std::wstring strtowstr(const std::string &str)
{
	// Convert an ASCII string to a Unicode String
	std::wstring wstrTo;
	wchar_t *wszTo = new wchar_t[str.length() + 1];
	wszTo[str.size()] = L'\0';
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wszTo, (int)str.length());
	wstrTo = wszTo;
	delete[] wszTo;
	return wstrTo;
}

LPWSTR ConvertToLPWSTR(const std::wstring& s)
{
	LPWSTR ws = new wchar_t[s.size() + 1];
	copy(s.begin(), s.end(), ws);
	ws[s.size()] = 0;
	return ws;
}


void getFreeSpaceValues(LPCSTR pszDrive){
	typedef BOOL(WINAPI *P_GDFSE)(LPCTSTR, PULARGE_INTEGER,PULARGE_INTEGER, PULARGE_INTEGER);
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
}

void getAtaSupport(int16_t ataSupportBits){
	bitset<16> mask(ataSupportBits);
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
}

void getDmaSupport(int16_t dmaSupportBits){
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
}

void getPIOSupport(int16_t pioSupportBits){
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
}

int main()
{
	setlocale(LC_ALL, "RUS");
		TCHAR szDrive[] = ("\\A:");
		DWORD uDriveMask = GetLogicalDrives();
		printf("The bitmask of the logical drives in hex: %0X\n", uDriveMask);
		printf("The bitmask of the logical drives in decimal: %d\n", uDriveMask);
		if (uDriveMask == 0)
			printf("\nGetLogicalDrives() failed with failure code: %d\n", GetLastError());
		else
		{
			printf("\nThis machine has the following logical drives:\n");
			while (uDriveMask)
			{// use the bitwise AND, 1–available, 0-not available
				if (uDriveMask & 1){
					printf("%s\n", szDrive);
					
					wstring fullName = Name;;
					fullName += strtowstr(szDrive);
					LPWSTR wszDrive_converted = ConvertToLPWSTR(fullName);
					
					WORD *data = GetDeviceIOControl(wszDrive_converted);

					int16_t ataSupportBits = data[ATABit];
					int16_t dmaSupportBits = data[DMABit];
					int16_t pioSupportBits = data[PIOBit];
					
					//GET MEMORYiNFO
					getFreeSpaceValues(szDrive);
					//ata
					getAtaSupport(ataSupportBits);
					//dma
					getDmaSupport(dmaSupportBits);
					//pio
					getPIOSupport(pioSupportBits);

					system("pause");
				}
		// increment... 
		++szDrive[1];
		// shift the bitmask binary right
		uDriveMask >>= 1;
			}
			printf("\n ");
		}
}