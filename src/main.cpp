//clipboard to imagefile
#include<windows.h>
#include<iostream>
#include<vector>
#include<fstream>

#define _release
LRESULT CALLBACK WndProc(HWND hwnd , UINT msg , WPARAM wp , LPARAM lp) {
	HDC hdc;
	PAINTSTRUCT ps;
	HGLOBAL hg;
	PTSTR strText , strClip;
	RECT rect;

	switch (msg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		hdc = BeginPaint(hwnd , &ps);

		if (OpenClipboard(hwnd)) {
			HGLOBAL hDib = GetClipboardData(CF_DIB);
			if (hDib) {
				BYTE* pDib = (BYTE*)GlobalLock(hDib);
				if (pDib) {
					BITMAPINFOHEADER* bih = (BITMAPINFOHEADER*)pDib;
					std::cout << "biBitCount: " << bih->biBitCount << std::endl;
					std::cout << "biCompression: " << bih->biCompression << std::endl;
					std::cout << "biClrUsed: " << bih->biClrUsed << std::endl;
					std::cout << "biHeight: " << bih->biHeight << std::endl;
					// パレット有無
					if (bih->biBitCount <= 8) {
						std::cout << "Palette colors: " << (bih->biClrUsed ? bih->biClrUsed : 1 << bih->biBitCount) << std::endl;
					}

					// BMPファイルとして保存
					int headerSize = sizeof(BITMAPFILEHEADER) + bih->biSize + (bih->biClrUsed ? bih->biClrUsed * 4 : (bih->biBitCount <= 8 ? (1 << bih->biBitCount) * 4 : 0));
					DWORD dibSize = GlobalSize(hDib);
					BITMAPFILEHEADER bfh = {};
					bfh.bfType = 0x4D42; // 'BM'
					bfh.bfSize = headerSize + dibSize - bih->biSize;
					bfh.bfOffBits = headerSize;

					std::ofstream os("ClipToBMP/ClipToBMP.bmp ", std::ios::binary);
					os.write((char*)&bfh, sizeof(bfh));
					os.write((char*)pDib, dibSize);
					os.close();

					GlobalUnlock(hDib);
				}
			}
			CloseClipboard();
		}

		#ifdef _release
		PostQuitMessage(0);
		#endif

		EndPaint(hwnd , &ps);
		return 0;			
	}
	return DefWindowProc(hwnd , msg , wp , lp);
}

int WINAPI WinMain(HINSTANCE hInstance , HINSTANCE hPrevInstance ,
			PSTR lpCmdLine , int nCmdShow ) {
	HWND hwnd;
	HACCEL haccel;
	MSG msg;
	WNDCLASS winc;

	winc.style		= CS_HREDRAW | CS_VREDRAW;
	winc.lpfnWndProc	= WndProc;
	winc.cbClsExtra	= 0;
	winc.cbWndExtra	= DLGWINDOWEXTRA;
	winc.hInstance		= hInstance;
	winc.hIcon		= LoadIcon(NULL , IDI_APPLICATION);
	winc.hCursor		= LoadCursor(NULL , IDC_ARROW);
	winc.hbrBackground	= (HBRUSH)GetStockObject(WHITE_BRUSH);
	winc.lpszMenuName	= NULL;
	winc.lpszClassName	= TEXT("ClipToBMP");

	if (!RegisterClass(&winc)) return -1;

	hwnd = CreateWindow(
			TEXT("ClipToBMP") , TEXT("clipboard to imagefile") ,
			WS_OVERLAPPEDWINDOW | WS_VISIBLE ,
			CW_USEDEFAULT , CW_USEDEFAULT ,
			CW_USEDEFAULT , CW_USEDEFAULT ,
			NULL , NULL , hInstance , NULL
	);

	if (hwnd == NULL) return -1;

	while(GetMessage(&msg , NULL , 0 , 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return msg.wParam;
}