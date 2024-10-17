#include <windows.h>
#include <stdio.h>

#define ID_URL_BAR 1
#define WIN_MAIN_URL_LENGTH 512

// Subclass procedure for the URL bar to intercept the Enter key
LRESULT CALLBACK URLBarProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KEYDOWN && wParam == VK_RETURN) {
        char url[WIN_MAIN_URL_LENGTH];
        GetWindowText(hwnd, url, sizeof(url));
        printf("Loading URL: %s\n", url);  // Replace with actual loading logic
        return 0;  // Prevent further processing (avoids beep)
    }
    return CallWindowProc((WNDPROC)GetWindowLongPtr(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
}

// Set the font and colors for the URL bar
HBRUSH hBrush, hBgBrush;
HFONT hFont;

#define WIN_MAIN_BASEBAR_HEIGHT 45
#define WIN_MAIN_URLBAR_HEIGHT 25
#define WIN_MAIN_URLBAR_INIT_WIDTH 600
#define WIN_MAIN_INIT_WIDTH 800
#define WIN_MAIN_URLBAR_PADDING_X 10
#define WIN_MAIN_URLBAR_PADDING_Y 25

#define WIN_MAIN_GRAY RGB(240, 240, 240)
#define WIN_MAIN_WHITE RGB(255, 255, 255)

#define WIN_MAIN_URL_FONT_CHEIGHT 16

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND urlBar;

    switch (uMsg) {
        case WM_CREATE:
            // Create the URL bar with styling
            urlBar = CreateWindowEx(
                0, "EDIT", "",
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_EX_CLIENTEDGE | ES_LEFT | ES_AUTOHSCROLL,
                0, WIN_MAIN_URLBAR_PADDING_X, WIN_MAIN_URLBAR_INIT_WIDTH, WIN_MAIN_URLBAR_HEIGHT, hwnd, (HMENU)ID_URL_BAR, NULL, NULL
            );

            // Subclass the URL bar to intercept key events
            SetWindowLongPtr(urlBar, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtr(
                urlBar, GWLP_WNDPROC, (LONG_PTR)URLBarProc
            ));

            // Create a font for the URL bar
            hFont = CreateFont(
                WIN_MAIN_URL_FONT_CHEIGHT, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PALETTE, "Arial"
            );
            SendMessage(urlBar, WM_SETFONT, (WPARAM)hFont, TRUE);

            // Create brushes for background colors
            hBrush = CreateSolidBrush(WIN_MAIN_GRAY); // Off-white background for rectangle
            hBgBrush = CreateSolidBrush(WIN_MAIN_WHITE); // White background for the URL bar
            break;

        case WM_CTLCOLOREDIT:
            if ((HWND)lParam == urlBar) {
                HDC hdc = (HDC)wParam;
                SetBkColor(hdc, WIN_MAIN_WHITE); // Set background color to white
                return (INT_PTR)hBgBrush; // Set brush for the background
            }
            break;

        case WM_ERASEBKGND: {
            HDC hdc = (HDC)wParam;
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            FillRect(hdc, &clientRect, hBrush); // Fill with off-white color
            return 1; // Prevent further erasing
        }

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Draw the off-white rectangle behind the URL bar
            RECT rect;
            rect.left = 0; // Padding from left
            rect.top = 0;  // Padding from top
            rect.right = WIN_MAIN_INIT_WIDTH; // Set to window width
            rect.bottom = WIN_MAIN_BASEBAR_HEIGHT; // URL bar height
            FillRect(hdc, &rect, hBrush); // Fill with off-white color

            EndPaint(hwnd, &ps);
            break;
        }

        case WM_SIZE: {
            // Redraw the background on resize
            InvalidateRect(hwnd, NULL, TRUE);
            // Center the URL bar on resize
            if (urlBar) {
                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                int centerX = (clientRect.right - WIN_MAIN_URLBAR_INIT_WIDTH) / 2; // Centering the URL bar
                SetWindowPos(urlBar, NULL, centerX, WIN_MAIN_URLBAR_PADDING_X, WIN_MAIN_URLBAR_INIT_WIDTH, WIN_MAIN_URLBAR_PADDING_Y, SWP_NOZORDER);
            }
            break;
        }

        case WM_DESTROY:
            DeleteObject(hFont); // Clean up the font
            DeleteObject(hBrush); // Clean up the brush
            DeleteObject(hBgBrush); // Clean up the background brush
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "MyWindowClass";

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClass(&wc);

    HWND hwnd = CreateWindowEx(
        0, CLASS_NAME, "My Browser Window", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, WIN_MAIN_INIT_WIDTH, WIN_MAIN_URLBAR_INIT_WIDTH,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);

    // Center the URL bar initially
    HWND urlBar = FindWindowEx(hwnd, NULL, "EDIT", NULL);
    if (urlBar) {
        RECT clientRect;
        GetClientRect(hwnd, &clientRect);
        int centerX = (clientRect.right - WIN_MAIN_URLBAR_INIT_WIDTH) / 2; // Centering the URL bar
        SetWindowPos(urlBar, NULL, centerX, WIN_MAIN_URLBAR_PADDING_X, WIN_MAIN_URLBAR_INIT_WIDTH, WIN_MAIN_URLBAR_PADDING_Y, SWP_NOZORDER);
    }

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
