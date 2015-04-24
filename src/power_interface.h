#pragma once

class PowerTask;

class PowerInterface
{
public:
  PowerInterface(void);
  ~PowerInterface(void);

  bool  Start();
  bool  GetTask(PowerTask& task);

private:
  bool HttpGet(CString url, CString& response);
  CString test_id;
  CString user_agent;
};

