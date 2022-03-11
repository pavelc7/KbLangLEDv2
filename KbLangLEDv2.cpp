// KbLangLEDv2.cpp
// There is KbLangLED.dpr on Delphi 
// see https://habr.com/ru/post/651317/comments/#comment_24073011
// I rewrote on C++, added some modifications
// "KbLangLEDv2.cpp" - is the source code, so you can recompile utility with any changes.
// https://sourceforge.net/projects/kblangledv2

#define WIN32_LEAN_AND_MEAN
//#include <tchar.h>
#include <windows.h>

enum TKbLEDTag 
{
	kbledNum=0, kbledCaps, kbledScroll, _kbledLast
};
TKbLEDTag LangKbLED = kbledScroll;
BYTE VKeyCodes[_kbledLast] = {VK_NUMLOCK, VK_CAPITAL, VK_SCROLL};
BYTE ScanCodes[_kbledLast] = {0x45,       0x3A,       0x46};

void SimulateLkKey(bool KeyDown,TKbLEDTag LEDTag)  
{
    keybd_event(VKeyCodes[LEDTag], ScanCodes[LEDTag], KEYEVENTF_EXTENDEDKEY +
      (! KeyDown) * KEYEVENTF_KEYUP, 0);
}

void ToggleKeybrdLED(TKbLEDTag LEDTag)  
{
  SimulateLkKey(true,LEDTag);  // Simulate a key press
  SimulateLkKey(false,LEDTag); // Simulate a key release
}


int APIENTRY WinMain(HINSTANCE /*hInstance*/,
                     HINSTANCE /*hPrevInstance*/,
                     LPSTR    /*lpCmdLine*/,
                     int       /*nCmdShow*/)
{		
	// Check already running
	HANDLE Mutex = CreateMutexA(NULL, TRUE, (LPCSTR)"KbLangLED_Running");
	HANDLE hExit = CreateEventA(NULL, TRUE, FALSE, (LPCSTR)"KbLangLED_Exit_please");
	if (Mutex == NULL || WaitForSingleObject(Mutex, 0) != WAIT_OBJECT_0){
		SetEvent(hExit); //second launch runs exitting
		return 0;
	}

	MSG msg;
	// Turn off the feedback cursor
	if (PostThreadMessage(GetCurrentThreadId(), 0, 0, 0))
		GetMessage(&msg, NULL, 0, 0);
	//bool EnUS;
	//bool OldEnUS = true;
	while(1)
	{  
		DWORD kbl = (DWORD)GetKeyboardLayout(GetWindowThreadProcessId(GetForegroundWindow(),NULL));
		bool EnUS = (kbl & 0x0000FFFF) == 0x0409;

		SHORT ks = GetKeyState(VKeyCodes[LangKbLED]);
		//bool toggleKey =  (EnUS && !(ks & 0x01)) || (!EnUS && (ks & 0x01)); 
		bool altPressed = (GetKeyState(VK_MENU) & 0x0080)==0x0080;
		bool toggleKey =  EnUS ^ !(ks & 0x01); 
		if ( toggleKey && !altPressed )	//if (EnUS != OldEnUS)
		{
			ToggleKeybrdLED(LangKbLED);
			//OldEnUS = EnUS;
		}
		Sleep(100);
		if(WaitForSingleObject(hExit, 0) == WAIT_OBJECT_0)
		{
			ResetEvent(hExit);
			return 0;
		}
	}	
}
