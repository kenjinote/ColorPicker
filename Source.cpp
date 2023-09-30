#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include <windows.h>
#include <windowsx.h>
#include "resource.h"

TCHAR szClassName[] = TEXT("Window");

WNDPROC StaticWndProc;
HBRUSH hBrush;
COLORREF rgb;
HCURSOR hCursor;

LRESULT CALLBACK StaticProc1(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static BOOL bDrag;
	static COLORREF Cust[16];
	switch (msg)
	{
	case WM_LBUTTONDOWN:
		SetFocus(hWnd);
		bDrag = TRUE;
		SetCapture(hWnd);
		break;
	case WM_MOUSEMOVE:
		if (bDrag)
		{
			SetCursor(hCursor);
			POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			ClientToScreen(hWnd, &point);
			HWND hDesktop = GetDesktopWindow();
			HDC hdc = GetDC(hDesktop);
			rgb = GetPixel(hdc, point.x, point.y);
			ReleaseDC(hDesktop, hdc);
			DeleteObject(hBrush);
			hBrush = CreateSolidBrush(rgb);
			InvalidateRect(hWnd, 0, TRUE);
		}
		break;
	case WM_LBUTTONUP:
		if (bDrag)
		{
			ReleaseCapture();
			bDrag = FALSE;
			RECT rc;
			GetClientRect(hWnd, &rc);
			POINT point = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
			if (PtInRect(&rc, point)) {
				CHOOSECOLOR cc;
				ZeroMemory(&cc, sizeof(CHOOSECOLOR));
				cc.lStructSize = sizeof(CHOOSECOLOR);
				cc.hwndOwner = hWnd;
				cc.rgbResult = rgb;
				cc.lpCustColors = Cust;
				cc.Flags = CC_RGBINIT | CC_FULLOPEN;
				if (ChooseColor(&cc) == TRUE) {
					rgb = cc.rgbResult;
					DeleteObject(hBrush);
					hBrush = CreateSolidBrush(rgb);
					InvalidateRect(hWnd, 0, FALSE);
				}
			}
			InvalidateRect(hWnd, 0, FALSE);
		}
		break;
	case WM_KILLFOCUS:
	case WM_KEYDOWN:
		ReleaseCapture();
		bDrag = FALSE;
		break;
	default:
		break;
	}
	return CallWindowProc(StaticWndProc, hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hStatic;
	switch (msg)
	{
	case WM_CREATE:
		hCursor = LoadCursor(GetModuleHandle(0), MAKEINTRESOURCE(IDC_CURSOR2));
		rgb = RGB(128, 128, 128);
		hBrush = CreateSolidBrush(rgb);
		hStatic = CreateWindow(TEXT("STATIC"), 0, WS_VISIBLE | WS_CHILD | WS_BORDER | SS_NOTIFY, 10, 10, 128, 128, hWnd, (HMENU)1000, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		StaticWndProc = (WNDPROC)SetWindowLongPtr(hStatic, GWLP_WNDPROC, (LONG_PTR)StaticProc1);
		break;
	case WM_CTLCOLORSTATIC:
		return (LRESULT)hBrush;
	case WM_DESTROY:
		DeleteObject(hCursor);
		DeleteObject(hBrush);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("Window"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
