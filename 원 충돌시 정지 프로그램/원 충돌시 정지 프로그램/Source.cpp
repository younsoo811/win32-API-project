#include <windows.h>
#include <TCHAR.H>
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM lParam);
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
	LPSTR lpszCmdLine, int nCmdShow)
{
	HWND 	 hwnd;
	MSG 	 msg;
	WNDCLASS WndClass;
	WndClass.style = CS_HREDRAW | CS_VREDRAW;
	WndClass.lpfnWndProc = WndProc;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.hInstance = hInstance;
	WndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	WndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	WndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = _T("Window Class Name");
	RegisterClass(&WndClass);
	hwnd = CreateWindow(_T("Window Class Name"),
		_T("Window Title Name"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		hInstance,
		NULL
		);
	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}

#include <math.h>
#define  BSIZE 50	//반지름
double LengthPts(int x1, int y1, int x2, int y2)
{
	return(sqrt((float)((x2 - x1)*(x2 - x1) + (y2 - y1)*(y2 - y1))));
}

BOOL OutCircle(int x, int y, int mx, int my)
{
	if (LengthPts(x, y, mx, my) > BSIZE) return TRUE;	//큰 경우로 변경 (원 겹치게 마우스 눌렀을때 조건도 추가하면 좋을듯 
	else return FALSE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg,
	WPARAM wParam, LPARAM lParam)
{
	HDC			hdc;
	PAINTSTRUCT ps;
	HBRUSH hBrush;
	static int	x, y;
	static BOOL Selection;
	int			nx, ny;
	static int dx, dy;

	switch (iMsg)
	{
	case WM_CREATE:
		x = 150; y = 150;
		Selection = FALSE;
		break;
	case WM_PAINT:
		hdc = BeginPaint(hwnd, &ps);
		if (Selection){
			(HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(255, 0, 0)));	//클릭하면 원 색상변경
			Ellipse(hdc, dx - BSIZE, dy - BSIZE, dx + BSIZE, dy + BSIZE);
		}
		(HBRUSH)SelectObject(hdc, CreateSolidBrush(RGB(255, 255, 255)));
		Ellipse(hdc, x - BSIZE, y - BSIZE, x + BSIZE, y + BSIZE);
		EndPaint(hwnd, &ps);
		break;
	case WM_LBUTTONDOWN:
		dx = LOWORD(lParam);
		dy = HIWORD(lParam);

		if (OutCircle(x, y, dx, dy)) {
			Selection = TRUE;
			SetTimer(hwnd, 1, 100, NULL);
		}

		InvalidateRgn(hwnd, NULL, TRUE);
		break;
	case WM_LBUTTONUP:
		Selection = FALSE;
		InvalidateRgn(hwnd, NULL, TRUE);
		break;
	case WM_TIMER:
		if (Selection){
			double distance = 10;				//원 이동 거리
			double c = LengthPts(x, y, dx, dy);	//대각선의 거리
			double a = dx - x;					//밑변 길이
			double b = dy - y;					//높이

			double nx = distance * (a / c);		//cos (실제 움직인 거리 구하기)
			double ny = distance * (b / c);		//sin

			x += nx;
			y += ny;

			if (c < BSIZE * 2 + distance){
				//대각선의 거리가 103과 같은 경우 10 이동시 원이 겹친상태로 멈추게 되는데, 겹친만큼 뒤에서 멈추게 한다
				distance = -(BSIZE * 2 + distance - c);			//겹친 부분의 길이만큼 뒤로 이동
				c = LengthPts(x, y, dx, dy);	//대각선의 거리
				a = dx - x;						//밑변 길이
				b = dy - y;						//높이

				nx = distance * (a / c);		//cos (실제 움직인 거리 구하기)
				ny = distance * (b / c);		//sin

				x += nx;
				y += ny;

				KillTimer(hwnd, 1);				//원 정지
			}
		}
		InvalidateRgn(hwnd, NULL, TRUE);
		break;
	case WM_DESTROY:
		KillTimer(hwnd, 1);
		PostQuitMessage(0);
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));
}