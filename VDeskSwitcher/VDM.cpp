#include "stdafx.h"
#include "afxtempl.h"
#include "VDM.h"

// http://www.cyberforum.ru/blogs/105416/blog3671.html
HRESULT EnumVirtualDesktops(IVirtualDesktopManagerInternal *pDesktopManager, CArray<GUID> *pDesks)
{
	IObjectArray *pObjectArray = NULL;
	HRESULT hr = pDesktopManager->GetDesktops(&pObjectArray);

	if (SUCCEEDED(hr))
	{
		UINT count;
		hr = pObjectArray->GetCount(&count);
		if (SUCCEEDED(hr))
		{
			pDesks->SetSize(count);
			for (UINT i = 0; i < count; i++)
			{
				IVirtualDesktop *pDesktop = NULL;
				if (FAILED(pObjectArray->GetAt(i, __uuidof(IVirtualDesktop), (void**)&pDesktop)))
					continue;
				GUID id = { 0 };
				if (SUCCEEDED(pDesktop->GetID(&id)))
				{
					pDesks->SetAt(i, id);
				}
				else {
					pDesks->SetAt(i, {0});
				}
				pDesktop->Release();
			}
		}
		pObjectArray->Release();
	}
	return hr;
}