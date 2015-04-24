#include "StdAfx.h"
#include "power.h"

extern HINSTANCE dll_hinstance;
Power* global_power = NULL;

#define UWM_TASK WM_APP + 1
const DWORD NAVIGATE_TIMEOUT = 60000;
const DWORD SCROLL_DELAY = 1000;
const DWORD SCROLL_COUNT = 60;
const DWORD SCROLL_AMOUNT = 100;
const DWORD WATCHDOG_INTERVAL = 60000;


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
Power::Power(void):
  _active(false)
  ,_message_window(NULL) {
  AtlTrace(_T("Power::Power"));
}


/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
Power::~Power(void) {
  global_power = NULL;
  AtlTrace(_T("Power::~Power"));
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
static LRESULT CALLBACK BrowserPowerWindowProc(HWND hwnd, UINT uMsg, 
                                                WPARAM wParam, LPARAM lParam) {
  LRESULT ret = 0;
  bool handled = false;
  if (global_power)
    handled = global_power->OnMessage(uMsg, wParam, lParam);
  if (!handled)
    ret = DefWindowProc(hwnd, uMsg, wParam, lParam);
  return ret;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
bool Power::OnMessage(UINT message, WPARAM wParam, LPARAM lParam) {
  bool ret = true;

  switch (message){
    case WM_TIMER:
        switch (wParam) {
          case 1: OnWatchdogTimer(); break;
          case 2: OnNavigateTimer(); break;
          case 3: OnWaitTimer(); break;
          case 4: OnScrollTimer(); break;
        }
        break;
    default:
        ret = false;
        break;
  }

  return ret;
}

void Power::OnWatchdogTimer(void) {
  AtlTrace(_T("[browser-power] - OnWatchdogTimer"));
  GetNextTask();
}

void Power::OnNavigateTimer(void) {
  AtlTrace(_T("[browser-power] - OnNavigateTimer"));
  KillTimer(_message_window, 2);
  ProcessTask();
}

void Power::OnWaitTimer(void) {
  AtlTrace(_T("[browser-power] - OnWaitTimer"));
  _task.did_wait = true;
  KillTimer(_message_window, 3);
  ProcessTask();
}

void Power::OnScrollTimer(void) {
  AtlTrace(_T("[browser-power] - OnScrollTimer"));
  if (_scroll_count < SCROLL_COUNT) {
    _scroll_count++;
    ScrollPage(SCROLL_AMOUNT);
  } else {
    _task.did_scroll = true;
    KillTimer(_message_window, 4);
    ProcessTask();
  }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void Power::Start() {
  AtlTrace(_T("[browser-power] - Start"));
  if (!_active) {
    _active = true;
    global_power = this;
    WNDCLASS wndClass;
    memset(&wndClass, 0, sizeof(wndClass));
    wndClass.lpszClassName = _T("browser-power");
    wndClass.lpfnWndProc = BrowserPowerWindowProc;
    wndClass.hInstance = dll_hinstance;
    if (RegisterClass(&wndClass)) {
      _message_window = CreateWindow(wndClass.lpszClassName,
          wndClass.lpszClassName, WS_POPUP, 0, 0, 0, 0, NULL, NULL,
          dll_hinstance, NULL);
      if (_message_window) {
        if (_power_interface.Start()) {
          SetTimer(_message_window, 1, WATCHDOG_INTERVAL, NULL);
          GetNextTask();
        }
      }
    }
  }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void Power::GetNextTask() {
  if (!_task.valid) {
    if (_power_interface.GetTask(_task))
      ProcessTask();
  }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void Power::ProcessTask() {
  if (_task.valid) {
    if (!_task.did_load) {
      NavigateTo(_task.url);
    } else if (_task.did_load_ok) {
      if (!_task.did_wait && _task.wait > 0) {
        AtlTrace(_T("[browser-power] - Waiting"));
        SetTimer(_message_window, 3, _task.wait * 1000, NULL);
      } else if (!_task.did_scroll && _task.scroll) {
        AtlTrace(_T("[browser-power] - Scrolling"));
        _scroll_count = 0;
        SetTimer(_message_window, 4, SCROLL_DELAY, NULL);
      } else {
        AtlTrace(_T("[browser-power] - Task done"));
        _task.valid = false;
      }
    } else {
      AtlTrace(_T("[browser-power] - Task done"));
      _task.valid = false;
    }
  }

  if (!_task.valid) {
    GetNextTask();
  }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void Power::Stop(void) {
  _active = false;
  if (_message_window) {
    // kill any timers we may have active
    for (int i = 1; i < 10; i++)
      KillTimer(_message_window, i);
    DestroyWindow(_message_window);
  }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void Power::OnLoad() {
  KillTimer(_message_window, 2);
  if (_active && !_task.did_load_ok) {
    _task.did_load_ok = true;
    AtlTrace(_T("[browser-power] - Power::OnLoad()"));
    ProcessTask();
  }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void Power::OnLoadError() {
  KillTimer(_message_window, 2);
  if (_active && !_task.did_load_ok) {
    AtlTrace(_T("[browser-power] - Power::OnLoadError()"));
    _task.valid = false;
    ProcessTask();
  }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void  Power::NavigateTo(CString url) {
  OutputDebugString(_T("[browser-power] NavigateTo ") + url);
  SetTimer(_message_window, 2, NAVIGATE_TIMEOUT, NULL);
  _task.did_load = true;
  if (_web_browser) {
    CComBSTR bstr_url = url;
    _web_browser->Navigate(bstr_url, 0, 0, 0, 0);
  }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void  Power::ScrollPage(int pixels) {
  if (_web_browser) {
    CComPtr<IDispatch> dispatch;
    if (SUCCEEDED(_web_browser->get_Document(&dispatch))) {
      CComQIPtr<IHTMLDocument2> document = dispatch;
      if (document) {
        CComPtr<IHTMLWindow2> window;
        if (SUCCEEDED(document->get_parentWindow(&window))) {
          window->scrollBy(0, pixels);
        }
      }
    }
  }
}