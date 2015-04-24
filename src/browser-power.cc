// BrowserPower.cpp : Implementation of BrowserPower

#include "stdafx.h"
#include "resource.h"
#include "browser-power_i.h"
#include "dllmain.h"
#include "xdlldata.h"
#include "browser-power.h"

/*-----------------------------------------------------------------------------
  Main entry point that is called when IE starts up
-----------------------------------------------------------------------------*/
STDMETHODIMP BrowserPower::SetSite(IUnknown *pUnkSite) {
  if (pUnkSite) {
    AtlTrace(_T("[BrowserPower] SetSite\n"));
    if (!_web_browser) {
      _web_browser = pUnkSite;
      DispEventAdvise(pUnkSite, &DIID_DWebBrowserEvents2);
    }
    AtlTrace(_T("[BrowserPower] SetSite complete\n"));
  } else {
    /*
    AtlTrace(_T("[BrowserPower] SetSite with NULL pUnkSite\n"));
    DispEventUnadvise(_web_browser, &DIID_DWebBrowserEvents2);
    _power.Stop();
    _web_browser.Release();
    AtlTrace(_T("[BrowserPower] SetSite with NULL pUnkSite complete\n"));
    */
  }
  return IObjectWithSiteImpl<BrowserPower>::SetSite(pUnkSite);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STDMETHODIMP_(void) BrowserPower::OnDocumentComplete(IDispatch *pDisp, 
            VARIANT * vUrl) {
  CString url;
  if (vUrl)
    url = *vUrl;

  CComPtr<IUnknown> unknown_browser = _web_browser;
  CComPtr<IUnknown> unknown_frame = pDisp;
  if (unknown_browser && unknown_frame && unknown_browser == unknown_frame) {
    AtlTrace(_T("[BrowserPower] OnDocumentComplete\n"));
    if(!url.CompareNoCase(_T("http://power.webpagetest.org/"))) {
      _power.Start(_web_browser);
    } else {
      _power.OnLoad();
    }
  }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STDMETHODIMP_(void) BrowserPower::OnNavigateError(IDispatch *pDisp, VARIANT *vUrl, 
            VARIANT *TargetFrameName, VARIANT *StatusCode, 
            VARIANT_BOOL *Cancel) {
  DWORD code = 0;
  if( StatusCode )
    code = StatusCode->lVal;
  if( !code )
    code = -1;
  CString url;
  if (vUrl)
    url = *vUrl;

  CComPtr<IUnknown> unknown_browser = _web_browser;
  CComPtr<IUnknown> unknown_frame = pDisp;
  if (unknown_browser && unknown_frame && unknown_browser == unknown_frame) {
    CString buff;
    buff.Format(_T("[BrowserPower] - NavigateError (%d): "), code);
    AtlTrace(buff + url);
    _power.OnLoad();
  }
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
STDMETHODIMP_(void) BrowserPower::OnQuit(VOID) {
  _power.OnLoad();
}

/*-----------------------------------------------------------------------------
  See if we need to block all popup windows
-----------------------------------------------------------------------------*/
STDMETHODIMP_(void) BrowserPower::OnNewWindow2(IDispatch ** pDisp, 
            VARIANT_BOOL *Cancel) {
  if (_power._active && Cancel) {
    *Cancel = VARIANT_TRUE;
  }
}

/*-----------------------------------------------------------------------------
  See if we need to block all popup windows
-----------------------------------------------------------------------------*/
STDMETHODIMP_(void) BrowserPower::OnNewWindow3(IDispatch **ppDisp, 
            VARIANT_BOOL *Cancel, DWORD dwFlags, BSTR bstrUrlContext, 
            BSTR bstrUrl) {
  if (_power._active && Cancel) 	{
    *Cancel = VARIANT_TRUE;
  }
}

