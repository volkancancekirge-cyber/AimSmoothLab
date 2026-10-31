#define UNICODE
#define _UNICODE
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <cmath>
#include <string>
#include <fstream>
#include <algorithm>
#include <chrono>

struct Config { int key=VK_XBUTTON1; int smooth=35; bool toggle=false; } cfg;
HWND windowHandle=nullptr, keyButton=nullptr, slider=nullptr, toggleBox=nullptr, colorBox=nullptr;
int targetColor=0;
bool listening=false, active=false, held=false;
POINT aim{420,300};
float targetX=420.f,targetY=220.f, phase=0.f;
std::wstring keyName(int key){ if(key==VK_XBUTTON1)return L"Mouse 4"; if(key==VK_XBUTTON2)return L"Mouse 5"; if(key==VK_LBUTTON)return L"Left Mouse"; if(key==VK_RBUTTON)return L"Right Mouse"; if(key==VK_MBUTTON)return L"Middle Mouse"; UINT scan=MapVirtualKeyW(key,MAPVK_VK_TO_VSC)<<16; wchar_t name[80]{}; if(GetKeyNameTextW((LONG)scan,name,80))return name; return L"Key "+std::to_wstring(key); }
void save(){std::ofstream f("AimSmoothLab.ini");f<<cfg.key<<' '<<cfg.smooth<<' '<<cfg.toggle;}
void load(){std::ifstream f("AimSmoothLab.ini");int k,s,t;if(f>>k>>s>>t && k>0 && k<256 && s>=1 && s<=100){cfg.key=k;cfg.smooth=s;cfg.toggle=t!=0;}}
void refresh(){SetWindowTextW(keyButton,(L"Activation: "+keyName(cfg.key)+L" (click to change)").c_str());SendMessageW(slider,TBM_SETPOS,TRUE,cfg.smooth);SendMessageW(toggleBox,BM_SETCHECK,cfg.toggle?BST_CHECKED:BST_UNCHECKED,0);}
void setKey(int key){if(key==VK_ESCAPE){listening=false;refresh();return;}cfg.key=key;listening=false;active=false;held=false;refresh();save();}
void keyInput(int key,bool down){if(listening){if(down)setKey(key);return;}if(key!=cfg.key)return;if(cfg.toggle){if(down&&!held)active=!active;}else active=down;held=down;}
LRESULT CALLBACK proc(HWND h,UINT msg,WPARAM w,LPARAM l){
 switch(msg){
 case WM_CREATE:{load();INITCOMMONCONTROLSEX ic{sizeof(ic),ICC_BAR_CLASSES};InitCommonControlsEx(&ic);
 keyButton=CreateWindowW(L"BUTTON",L"",WS_CHILD|WS_VISIBLE|BS_PUSHBUTTON,20,16,325,32,h,(HMENU)1,nullptr,nullptr);
 CreateWindowW(L"STATIC",L"Smooth (1 slow - 100 fast)",WS_CHILD|WS_VISIBLE,20,58,320,22,h,nullptr,nullptr,nullptr);
 slider=CreateWindowW(TRACKBAR_CLASSW,L"",WS_CHILD|WS_VISIBLE|TBS_AUTOTICKS,20,82,325,38,h,(HMENU)2,nullptr,nullptr);SendMessageW(slider,TBM_SETRANGE,TRUE,MAKELPARAM(1,100));
 toggleBox=CreateWindowW(L"BUTTON",L"Toggle mode (instead of hold)",WS_CHILD|WS_VISIBLE|BS_AUTOCHECKBOX,20,126,330,26,h,(HMENU)3,nullptr,nullptr);
 CreateWindowW(L"STATIC",L"Click the arena to move the crosshair. Hold the selected key to smoothly follow the bot's head. No game input is sent.",WS_CHILD|WS_VISIBLE|SS_LEFT,20,160,650,38,h,nullptr,nullptr,nullptr);
 colorBox=CreateWindowW(L"COMBOBOX",L"",WS_CHILD|WS_VISIBLE|CBS_DROPDOWNLIST,370,82,290,130,h,(HMENU)4,nullptr,nullptr);
 SendMessageW(colorBox,CB_ADDSTRING,0,(LPARAM)L"Purple target");SendMessageW(colorBox,CB_ADDSTRING,0,(LPARAM)L"Yellow target");SendMessageW(colorBox,CB_ADDSTRING,0,(LPARAM)L"Red target");SendMessageW(colorBox,CB_SETCURSEL,targetColor,0);
 refresh();SetTimer(h,1,16,nullptr);return 0;}
 case WM_COMMAND:if(LOWORD(w)==1){listening=true;SetWindowTextW(keyButton,L"Press any key or mouse button (Esc cancels)...");SetFocus(h);}if(LOWORD(w)==4 && HIWORD(w)==CBN_SELCHANGE){targetColor=(int)SendMessageW(colorBox,CB_GETCURSEL,0,0);InvalidateRect(h,nullptr,FALSE);}if(LOWORD(w)==3){cfg.toggle=SendMessageW(toggleBox,BM_GETCHECK,0,0)==BST_CHECKED;save();}return 0;
 case WM_HSCROLL:if((HWND)l==slider){cfg.smooth=(int)SendMessageW(slider,TBM_GETPOS,0,0);save();}return 0;
 case WM_KEYDOWN:case WM_SYSKEYDOWN:keyInput((int)w,true);return 0;
 case WM_KEYUP:case WM_SYSKEYUP:keyInput((int)w,false);return 0;
 case WM_XBUTTONDOWN:keyInput(HIWORD(w)==XBUTTON1?VK_XBUTTON1:VK_XBUTTON2,true);return TRUE;
 case WM_XBUTTONUP:keyInput(HIWORD(w)==XBUTTON1?VK_XBUTTON1:VK_XBUTTON2,false);return TRUE;
 case WM_MBUTTONDOWN:keyInput(VK_MBUTTON,true);return 0;case WM_MBUTTONUP:keyInput(VK_MBUTTON,false);return 0;
 case WM_LBUTTONDOWN:if(listening){setKey(VK_LBUTTON);}else if(GET_Y_LPARAM(l)>210){aim={GET_X_LPARAM(l),GET_Y_LPARAM(l)};}return 0;
 case WM_RBUTTONDOWN:keyInput(VK_RBUTTON,true);return 0;case WM_RBUTTONUP:keyInput(VK_RBUTTON,false);return 0;
 case WM_TIMER:{phase+=.018f;targetX=420+190*std::sin(phase);targetY=355+70*std::sin(phase*.61f);if(active){double alpha=0.004+0.26*cfg.smooth/100.0;aim.x+=(LONG)std::round((targetX-aim.x)*alpha);aim.y+=(LONG)std::round((targetY-aim.y)*alpha);}InvalidateRect(h,nullptr,FALSE);return 0;}
 case WM_PAINT:{PAINTSTRUCT ps;HDC dc=BeginPaint(h,&ps);RECT r;GetClientRect(h,&r);RECT arena{10,210,r.right-10,r.bottom-12};HBRUSH bg=CreateSolidBrush(RGB(24,31,44));FillRect(dc,&arena,bg);DeleteObject(bg);SetBkMode(dc,TRANSPARENT);SetTextColor(dc,RGB(220,228,239));TextOutW(dc,20,220,L"TRAINING ARENA - simulation only",32);
 COLORREF chosen=targetColor==0?RGB(172,93,245):(targetColor==1?RGB(255,218,65):RGB(245,81,91));HBRUSH body=CreateSolidBrush(chosen);HBRUSH head=CreateSolidBrush(RGB(240,186,130));HGDIOBJ old=SelectObject(dc,body);Ellipse(dc,(int)targetX-21,(int)targetY+15,(int)targetX+21,(int)targetY+100);SelectObject(dc,head);Ellipse(dc,(int)targetX-15,(int)targetY-15,(int)targetX+15,(int)targetY+15);SelectObject(dc,old);DeleteObject(body);DeleteObject(head);
 HPEN pen=CreatePen(PS_SOLID,2,active?RGB(120,240,160):RGB(240,230,130));old=SelectObject(dc,pen);MoveToEx(dc,aim.x-13,aim.y,nullptr);LineTo(dc,aim.x+13,aim.y);MoveToEx(dc,aim.x,aim.y-13,nullptr);LineTo(dc,aim.x,aim.y+13);SelectObject(dc,old);DeleteObject(pen);std::wstring status=active?L"ACTIVE":L"IDLE";TextOutW(dc,20,r.bottom-39,status.c_str(),(int)status.size());double dx=targetX-aim.x,dy=targetY-aim.y;std::wstring metrics=L"Head distance: "+std::to_wstring((int)std::sqrt(dx*dx+dy*dy))+L" px | Smooth: "+std::to_wstring(cfg.smooth)+L" | "+status;TextOutW(dc,20,r.bottom-62,metrics.c_str(),(int)metrics.size());EndPaint(h,&ps);return 0;}
 case WM_DESTROY:save();PostQuitMessage(0);return 0;
 }return DefWindowProcW(h,msg,w,l);
}
int WINAPI wWinMain(HINSTANCE inst,HINSTANCE,LPWSTR,int show){WNDCLASSW wc{};wc.lpfnWndProc=proc;wc.hInstance=inst;wc.lpszClassName=L"AimSmoothLabWindow";wc.hCursor=LoadCursor(nullptr,IDC_ARROW);wc.hbrBackground=(HBRUSH)(COLOR_BTNFACE+1);RegisterClassW(&wc);windowHandle=CreateWindowW(wc.lpszClassName,L"AimSmoothLab - Offline Aim Smoothing Simulator",WS_OVERLAPPEDWINDOW|WS_VISIBLE,100,100,720,680,nullptr,nullptr,inst,nullptr);ShowWindow(windowHandle,show);MSG msg;while(GetMessageW(&msg,nullptr,0,0)){TranslateMessage(&msg);DispatchMessageW(&msg);}return 0;}
