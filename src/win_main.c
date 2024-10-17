#include <windows.h>
#include <stdio.h>

#define ID_URL_BAR 1

// Subclass procedure for the URL bar to intercept the Enter key
LRESULT CALLBACK URLBarProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    if (uMsg == WM_KEYDOWN && wParam == VK_RETURN) {
        char url[256];
        GetWindowText(hwnd, url, sizeof(url));
        printf("Loading URL %s\n", url);  // Replace with actual loading logic
        return 0;  // Prevent further processing (avoids beep)
    }
    return CallWindowProc((WNDPROC)GetWindowLongPtr(hwnd, GWLP_USERDATA), hwnd, uMsg, wParam, lParam);
}

// Set the font and colors for the URL bar
HBRUSH hBrush, hBgBrush;
HFONT hFont;

#define WIN_MAIN_BASEBAR_HEIGHT 45

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static HWND urlBar;

    switch (uMsg) {
        case WM_CREATE:
            // Create the URL bar with styling
            urlBar = CreateWindowEx(
                0, "EDIT", "",
                WS_CHILD | WS_VISIBLE | WS_BORDER | WS_EX_CLIENTEDGE | ES_LEFT | ES_AUTOHSCROLL,
                0, 10, 600, 25, hwnd, (HMENU)ID_URL_BAR, NULL, NULL
            );

            // Subclass the URL bar to intercept key events
            SetWindowLongPtr(urlBar, GWLP_USERDATA, (LONG_PTR)SetWindowLongPtr(
                urlBar, GWLP_WNDPROC, (LONG_PTR)URLBarProc
            ));

            // Create a font for the URL bar
            hFont = CreateFont(
                16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
                DEFAULT_QUALITY, DEFAULT_PALETTE, "Arial"
            );
            SendMessage(urlBar, WM_SETFONT, (WPARAM)hFont, TRUE);

            // Create brushes for background colors
            hBrush = CreateSolidBrush(RGB(240, 240, 240)); // Off-white background for rectangle
            hBgBrush = CreateSolidBrush(RGB(255, 255, 255)); // White background for the URL bar
            break;

        case WM_CTLCOLOREDIT:
            if ((HWND)lParam == urlBar) {
                HDC hdc = (HDC)wParam;
                SetBkColor(hdc, RGB(255, 255, 255)); // Set background color to white
                return (INT_PTR)hBgBrush; // Set brush for the background
            }
            break;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);

            // Fill the entire client area with off-white color
            RECT clientRect;
            GetClientRect(hwnd, &clientRect);
            FillRect(hdc, &clientRect, hBrush); // Fill the entire client area

            // Draw the off-white rectangle behind the URL bar
            RECT rect;
            rect.left = 0;              // Fill the entire width
            rect.top = 0;               // Start from the top
            rect.right = clientRect.right; // Fill to the right edge
            rect.bottom = WIN_MAIN_BASEBAR_HEIGHT; // Height for the base bar
            FillRect(hdc, &rect, hBrush); // Fill with off-white color

            EndPaint(hwnd, &ps);
            break;
        }

        case WM_SIZE: {
            // Reposition the URL bar when the window is resized
            if (urlBar) {
                RECT clientRect;
                GetClientRect(hwnd, &clientRect);
                int width = clientRect.right - clientRect.left - 20; // 10 padding on each side
                SetWindowPos(urlBar, NULL, (clientRect.right - width) / 2, 10, width, 25, SWP_NOZORDER);
            }
            InvalidateRect(hwnd, NULL, TRUE); // Force a repaint to update the background
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
        CW_USEDEFAULT, CW_USEDEFAULT, 800, 600,
        NULL, NULL, hInstance, NULL
    );

    if (hwnd == NULL) return 0;

    ShowWindow(hwnd, nCmdShow);

    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
