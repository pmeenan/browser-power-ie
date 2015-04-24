// dllmain.h : Declaration of module class.

class CbrowserpowerModule : public ATL::CAtlDllModuleT< CbrowserpowerModule >
{
public :
	DECLARE_LIBID(LIBID_browserpowerLib)
	DECLARE_REGISTRY_APPID_RESOURCEID(IDR_BROWSERPOWER, "{DD3683C2-A39F-44BF-B7AF-E2F5ACDD8527}")
};

extern class CbrowserpowerModule _AtlModule;
