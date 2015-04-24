#pragma once

#include "power_interface.h"
#include "power_task.h"

class Power {
public:
  Power(void);
  ~Power(void);
  void Start();
  void Stop(void);
  bool OnMessage(UINT message, WPARAM wParam, LPARAM lParam);

  // browser events
  void  OnLoad();
  void  OnLoadError();

  bool _active;
  CComPtr<IWebBrowser2> _web_browser;

private:
  PowerInterface        _power_interface;
  HWND                  _message_window;
  PowerTask             _task;
  DWORD                 _scroll_count;

  void GetNextTask();
  void ProcessTask();

  // Timers
  void OnWatchdogTimer(void);
  void OnNavigateTimer(void);
  void OnWaitTimer(void);
  void OnScrollTimer(void);

  // commands
  void  NavigateTo(CString url);
  void  ScrollPage(int pixels);
};
