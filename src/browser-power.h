// BrowserPower.h : Declaration of the BrowserPower

#pragma once
#include "resource.h"       // main symbols
#include "browser-power_i.h"
#include "power.h"

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif

// BrowserPower
class ATL_NO_VTABLE BrowserPower :
  public CComObjectRootEx<CComSingleThreadModel>,
  public CComCoClass<BrowserPower, &CLSID_BrowserPower>,
  public IObjectWithSiteImpl<BrowserPower>,
  public IDispEventImpl<1, BrowserPower, &DIID_DWebBrowserEvents2, &LIBID_SHDocVw, 1, 1>,
  public IDispatchImpl<IBrowserPower, &IID_IBrowserPower, &LIBID_browserpowerLib, /*wMajor =*/ 1, /*wMinor =*/ 0>
{
public:
  BrowserPower(){
  }

DECLARE_REGISTRY_RESOURCEID(IDR_BROWSERPOWER)

DECLARE_NOT_AGGREGATABLE(BrowserPower)

BEGIN_COM_MAP(BrowserPower)
  COM_INTERFACE_ENTRY(IBrowserPower)
  COM_INTERFACE_ENTRY(IDispatch)
  COM_INTERFACE_ENTRY(IObjectWithSite)
END_COM_MAP()

  DECLARE_PROTECT_FINAL_CONSTRUCT()
  BEGIN_SINK_MAP(BrowserPower)
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_DOCUMENTCOMPLETE, OnDocumentComplete )
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NAVIGATEERROR, OnNavigateError )
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW2, OnNewWindow2 )
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_NEWWINDOW3, OnNewWindow3 )
    SINK_ENTRY_EX(1, DIID_DWebBrowserEvents2, DISPID_ONQUIT, OnQuit )
  END_SINK_MAP()

  HRESULT FinalConstruct()
  {
    return S_OK;
  }

  void FinalRelease()
  {
  }

  STDMETHOD_(void,OnDocumentComplete)( IDispatch *pDisp, VARIANT * vUrl );
  STDMETHOD_(void,OnNavigateError)( IDispatch *pDisp, VARIANT *vUrl, 
              VARIANT *TargetFrameName, VARIANT *StatusCode, 
              VARIANT_BOOL *Cancel);
  STDMETHOD_(void,OnNewWindow2)( IDispatch ** pDisp, VARIANT_BOOL *cancel );
  STDMETHOD_(void,OnNewWindow3)( IDispatch **ppDisp, VARIANT_BOOL *Cancel, 
              DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl);
  STDMETHOD_(void,OnQuit)( VOID );

public:
  // IObjectWithSite
  STDMETHOD(SetSite)(IUnknown *pUnkSite);

private:
  CComQIPtr<IWebBrowser2> _web_browser;

// leave this public empty to support the DECLARE_PROTECT_FINAL_CONSTRUCT macro
public:
};

OBJECT_ENTRY_AUTO(__uuidof(BrowserPower), BrowserPower)
