#include "StdAfx.h"
#include "power_interface.h"
#include "power_task.h"
#include "json/json.h"

const TCHAR * TASK_REQUEST = _T("http://power.webpagetest.org/getTask.php?test=");
const TCHAR * TEST_CREATE = _T("http://power.webpagetest.org/startTest.php");

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
PowerInterface::PowerInterface(void) {
  char ua[10000];
  DWORD len = _countof(ua);
  *ua = 0;
  ObtainUserAgentString(0, ua, &len);
  user_agent = CA2T(ua);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
PowerInterface::~PowerInterface(void) {
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
bool PowerInterface::Start() {
  CString response;
  test_id.Empty();
  if (HttpGet(TEST_CREATE, response)) {
    AtlTrace(_T("[browser-power] Test Create response: %s"), response);
    Json::Value root;
    Json::Reader reader;
    std::string document = CT2A(response);
    if (reader.parse(document, root)) {
      int code = 0;
      Json::Value value = root.get("result", 0 );
      if (!value.empty() && value.isConvertibleTo(Json::intValue))
        code = value.asInt();
      if (code == 200) {
        Json::Value value = root.get("id", "" );
        if (!value.empty() && value.isConvertibleTo(Json::stringValue))
          test_id = value.asCString();
        test_id = test_id.Trim();
      }
    }
  }
  return test_id.GetLength() > 0;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
bool PowerInterface::GetTask(PowerTask& task) {
  task.Reset();
  CString response;
  if (HttpGet(TASK_REQUEST + test_id, response)) {
    AtlTrace(_T("[browser-power] Task String: %s"), response);
    task.ParseTask(response);
  }

  return task.valid;
}

/*-----------------------------------------------------------------------------
  Perform a http GET operation and return the body as a string
-----------------------------------------------------------------------------*/
bool PowerInterface::HttpGet(CString url, CString& response) {
  bool result = false;
  response.Empty();
  HINTERNET internet = InternetOpen(user_agent, 
                                    INTERNET_OPEN_TYPE_PRECONFIG,
                                    NULL, NULL, 0);
  if (internet) {
    DWORD timeout = 30000;
    InternetSetOption(internet, INTERNET_OPTION_CONNECT_TIMEOUT,
                      &timeout, sizeof(timeout));
    InternetSetOption(internet, INTERNET_OPTION_RECEIVE_TIMEOUT,
                      &timeout, sizeof(timeout));
    InternetSetOption(internet, INTERNET_OPTION_SEND_TIMEOUT,
                      &timeout, sizeof(timeout));
    HINTERNET http_request = InternetOpenUrl(internet, url, NULL, 0, 
                                INTERNET_FLAG_NO_CACHE_WRITE | 
                                INTERNET_FLAG_NO_UI | 
                                INTERNET_FLAG_PRAGMA_NOCACHE | 
                                INTERNET_FLAG_RELOAD, NULL);
    if (http_request) {
      char buff[4097];
      DWORD bytes_read;
      HANDLE file = INVALID_HANDLE_VALUE;
      while (InternetReadFile(http_request, buff, sizeof(buff) - 1, 
              &bytes_read) && bytes_read) {
        // NULL-terminate it and add it to our response string
        buff[bytes_read] = 0;
        response += CA2T(buff, CP_UTF8);
      }
      InternetCloseHandle(http_request);
    }
    InternetCloseHandle(internet);
  }

  if (response.GetLength())
    result = true;

  return result;
}

