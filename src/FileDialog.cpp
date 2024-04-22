#include <windows.h>
#include <shobjidl_core.h>

#include "FileDialog.h"

std::wstring OpenFileDialog(bool bSelDir)
{
    HRESULT hRes = OleInitialize(NULL);
    //if (hRes != S_OK) {}

    IFileOpenDialog* pFileOpenDialog = NULL;
    IShellItem* pShellItem = NULL;
    LPWSTR pSelectPath = NULL;
    std::wstring wsSelectPath;

    hRes = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pFileOpenDialog));
    if (SUCCEEDED(hRes))
    {
        FILEOPENDIALOGOPTIONS opts = FOS_FORCEFILESYSTEM;

        if (bSelDir)
        {
            opts |= FOS_PICKFOLDERS;
        }

        hRes = pFileOpenDialog->SetOptions(opts);
        hRes = pFileOpenDialog->Show(NULL);

        if (SUCCEEDED(hRes))
        {
            hRes = pFileOpenDialog->GetResult(&pShellItem);
            pShellItem->GetDisplayName(SIGDN_FILESYSPATH, &pSelectPath);
            if (pSelectPath)
            {
                wsSelectPath = pSelectPath;
            }
        }
        pFileOpenDialog->Release();
    }

    CoTaskMemFree(pSelectPath);
    OleUninitialize();
    return wsSelectPath;
}
