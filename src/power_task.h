#pragma once

class PowerTask {
public:
  PowerTask(void);
  ~PowerTask(void);
  void  Reset();
  bool  ParseTask(CString task);

  CString url;
  DWORD   wait;
  bool    scroll;
  bool    did_load;
  bool    did_load_ok;
  bool    did_wait;
  bool    did_scroll;
  bool    valid;
};

