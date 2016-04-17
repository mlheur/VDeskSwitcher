#pragma once
EXTERN_C const IID IID_IVirtualDesktopManager;

// For some reason this exists in VisualStudio if using the Release build, but not on the Debug build.
#ifdef _DEBUG
MIDL_INTERFACE("a5cd92ff-29be-454c-8d04-d82879fb3f1b")
IVirtualDesktopManager : public IUnknown
{
public:
	virtual HRESULT STDMETHODCALLTYPE IsWindowOnCurrentVirtualDesktop(
		/* [in] */ __RPC__in HWND topLevelWindow,
		/* [out] */ __RPC__out BOOL *onCurrentDesktop) = 0;

	virtual HRESULT STDMETHODCALLTYPE GetWindowDesktopId(
		/* [in] */ __RPC__in HWND topLevelWindow,
		/* [out] */ __RPC__out GUID *desktopId) = 0;

	virtual HRESULT STDMETHODCALLTYPE MoveWindowToDesktop(
		/* [in] */ __RPC__in HWND topLevelWindow,
		/* [in] */ __RPC__in REFGUID desktopId) = 0;
};
#endif