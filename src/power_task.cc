#include "StdAfx.h"
#include "power_task.h"
#include "json/json.h"

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
PowerTask::PowerTask(void) {
  Reset();
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
PowerTask::~PowerTask(void) {
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
void PowerTask::Reset() {
  url.Empty();
  wait = 0;
  scroll = false;
  did_load = false;
  did_load_ok = false;
  did_wait = false;
  did_scroll = false;
  valid = false;
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
bool PowerTask::ParseTask(CString task) {
  Reset();
  Json::Value root;
  Json::Reader reader;
  std::string document = CT2A(task);
  if (reader.parse(document, root)) {
    int code = 0;
    Json::Value value = root.get("result", 0 );
    if (!value.empty() && value.isConvertibleTo(Json::intValue))
      code = value.asInt();
    if (code == 200) {
      const Json::Value task = root["task"];
      if (!task.empty() && task.isObject()) {
        Json::Value value = task.get("url", "");
        if (!value.empty() && value.isConvertibleTo(Json::stringValue))
          url = value.asCString();
        value = task.get("wait", 0);
        if (!value.empty() && value.isConvertibleTo(Json::uintValue))
          wait = value.asUInt();
        value = task.get("scroll", false);
        if (!value.empty() && value.isConvertibleTo(Json::booleanValue))
          scroll = value.asBool();
        if (url.GetLength() > 0)
          valid = true;
      }
    }
  }

  return valid;
}
