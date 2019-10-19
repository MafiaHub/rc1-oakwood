#pragma once
#define _WIN32_DCOM
#include <comdef.h>
#include <Wbemidl.h>

namespace hwid {
    std::wstring cachedMoboID;
    std::string cachedGUID;

    std::wstring getMoboID() {
        HRESULT res;

        if (!cachedMoboID.empty()) {
            return cachedMoboID;
        }

        IWbemLocator* loc = NULL;
        res = CoCreateInstance(
            CLSID_WbemLocator,
            0,
            CLSCTX_INPROC_SERVER,
            IID_IWbemLocator, (LPVOID*)& loc);

        if (FAILED(res)) {
            MessageBox(NULL, "Failed to proceed with security flow.1 Exiting!", "Mafia Oakwood", MB_OK);
            CoUninitialize();
            exit(0);
        }

        IWbemServices* svc = NULL;
        res = loc->ConnectServer(
            _bstr_t(L"ROOT\\CIMV2"),
            NULL,
            NULL,
            0,
            NULL,
            0,
            0,
            &svc
        );

        if (FAILED(res)) {
            MessageBox(NULL, "Failed to proceed with security flow.2 Exiting!", "Mafia Oakwood", MB_OK);
            loc->Release();
            CoUninitialize();
            exit(0);
        }

        res = CoSetProxyBlanket(
            svc,
            RPC_C_AUTHN_WINNT,
            RPC_C_AUTHZ_NONE,
            NULL,
            RPC_C_AUTHN_LEVEL_CALL,
            RPC_C_IMP_LEVEL_IMPERSONATE,
            NULL,
            EOAC_NONE
        );

        if (FAILED(res)) {
            MessageBox(NULL, "Failed to proceed with security flow.3 Exiting!", "Mafia Oakwood", MB_OK);
            svc->Release();
            loc->Release();
            CoUninitialize();
            exit(0);
        }

        IEnumWbemClassObject* enumerator = NULL;
        res = svc->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT * FROM Win32_ComputerSystemProduct"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &enumerator
        );

        if (FAILED(res)) {
            MessageBox(NULL, "Failed to proceed with security flow.4 Exiting!", "Mafia Oakwood", MB_OK);
            svc->Release();
            loc->Release();
            CoUninitialize();
            exit(0);
        }

        IWbemClassObject* pclsObj = NULL;
        ULONG uReturn = 0;

        std::wstring moboID, moboSN;

        while (enumerator)
        {
            HRESULT hr = enumerator->Next(WBEM_INFINITE, 1,
                &pclsObj, &uReturn);

            if (0 == uReturn)
            {
                break;
            }

            VARIANT vtProp;

            // Get the value of the Name property
            hr = pclsObj->Get(L"UUID", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                moboID = vtProp.bstrVal;
            }
            VariantClear(&vtProp);
            pclsObj->Release();
        }

        res = svc->ExecQuery(
            bstr_t("WQL"),
            bstr_t("SELECT * FROM Win32_BIOS"),
            WBEM_FLAG_FORWARD_ONLY | WBEM_FLAG_RETURN_IMMEDIATELY,
            NULL,
            &enumerator
        );

        if (FAILED(res)) {
            MessageBox(NULL, "Failed to proceed with security flow.4 Exiting!", "Mafia Oakwood", MB_OK);
            svc->Release();
            loc->Release();
            CoUninitialize();
            exit(0);
        }

        while (enumerator)
        {
            HRESULT hr = enumerator->Next(WBEM_INFINITE, 1,
                &pclsObj, &uReturn);

            if (0 == uReturn)
            {
                break;
            }

            VARIANT vtProp;

            // Get the value of the Name property
            hr = pclsObj->Get(L"SerialNumber", 0, &vtProp, 0, 0);
            if (SUCCEEDED(hr)) {
                moboSN = vtProp.bstrVal;
            }
            VariantClear(&vtProp);
            pclsObj->Release();
        }

        svc->Release();
        loc->Release();

        cachedMoboID = moboSN+moboID;

        return cachedMoboID;
    }

    std::wstring getRawID() {
        auto moboID = getMoboID();
        auto magic = std::to_wstring((u64)342084380340);

        WCHAR volumeName[MAX_PATH + 1] = { 0 };
        WCHAR fileSystemName[MAX_PATH + 1] = { 0 };
        DWORD serialNumber = 0;
        DWORD maxComponentLen = 0;
        DWORD fileSystemFlags = 0;

        GetVolumeInformationW(
            L"C:\\",
            volumeName,
            sizeof(volumeName),
            &serialNumber,
            &maxComponentLen,
            &fileSystemFlags,
            fileSystemName,
            sizeof(fileSystemName));

        return (magic + std::to_wstring(serialNumber));
    }

    u64 getID() {
        auto rawID = getRawID();

        auto id = zpl_crc64(rawID.c_str(), zpl_strlen((char *)rawID.c_str()));

        return id;
    }
}
