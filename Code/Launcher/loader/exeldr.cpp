/*
* This file is part of the CitizenFX project - http://citizen.re/
*
* See LICENSE in the root of the source tree for information
* regarding licensing.
*/

#include <loader/exeldr.h>
#include <algorithm>

ExecutableLoader::ExecutableLoader(const uint8_t* origBinary) {
    m_origBinary = origBinary;
    m_loadLimit = UINT_MAX;

    SetLibraryLoader([](const char* name) {
        return LoadLibraryA(name);
    });

    SetFunctionResolver([](HMODULE module, const char* name) {
        return (LPVOID)GetProcAddress(module, name);
    });
}

void ExecutableLoader::LoadImports(IMAGE_NT_HEADERS* ntHeader) {
    IMAGE_DATA_DIRECTORY* importDirectory = &ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
    auto descriptor = GetTargetRVA<IMAGE_IMPORT_DESCRIPTOR>(importDirectory->VirtualAddress);

    while (descriptor->Name)
    {
        const char* name = GetTargetRVA<char>(descriptor->Name);

        HMODULE module = ResolveLibrary(name);

        if (!module)
        {
            //  FatalError("Could not load dependent module %s. Error code was %i.\n", name, GetLastError());
        }

        // "don't load"
        if (reinterpret_cast<uint32_t>(module) == 0xFFFFFFFF)
        {
            descriptor++;
            continue;
        }

        auto nameTableEntry = GetTargetRVA<uintptr_t>(descriptor->OriginalFirstThunk);
        auto addressTableEntry = GetTargetRVA<uintptr_t>(descriptor->FirstThunk);

        // GameShield (Payne) uses FirstThunk for original name addresses
        if (!descriptor->OriginalFirstThunk)
        {
            nameTableEntry = GetTargetRVA<uintptr_t>(descriptor->FirstThunk);
        }

        while (*nameTableEntry)
        {
            FARPROC function;
            const char* functionName;

            // is this an ordinal-only import?
            if (IMAGE_SNAP_BY_ORDINAL(*nameTableEntry))
            {
                function = GetProcAddress(module, MAKEINTRESOURCEA(IMAGE_ORDINAL(*nameTableEntry)));
                //functionName = va("#%d", IMAGE_ORDINAL(*nameTableEntry));
            }
            else
            {
                auto import = GetTargetRVA<IMAGE_IMPORT_BY_NAME>(*nameTableEntry);

                function = (FARPROC)m_functionResolver(module, import->Name);
                functionName = import->Name;
            }

            if (!function)
            {
                char pathName[MAX_PATH];
                GetModuleFileNameA(module, pathName, sizeof(pathName));

                //FatalError("Could not load function %s in dependent module %s (%s).\n", functionName, name, pathName);
            }

            *addressTableEntry = (uintptr_t)function;

            nameTableEntry++;
            addressTableEntry++;
        }

        descriptor++;
    }
}

void ExecutableLoader::LoadSection(IMAGE_SECTION_HEADER* section)
{
    void* targetAddress = GetTargetRVA<uint8_t>(section->VirtualAddress);
    const void* sourceAddress = m_origBinary + section->PointerToRawData;

    if ((uintptr_t)targetAddress >= m_loadLimit)
    {
        printf("attempting to access address after limit\n");
        return;
    }

    if (section->SizeOfRawData > 0)
    {
        uint32_t sizeOfData = min(section->SizeOfRawData, section->Misc.VirtualSize);

        memcpy(targetAddress, sourceAddress, sizeOfData);

        DWORD oldProtect;
        VirtualProtect(targetAddress, sizeOfData, PAGE_EXECUTE_READWRITE, &oldProtect);
    }
}

void ExecutableLoader::LoadSections(IMAGE_NT_HEADERS* ntHeader)
{
    IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(ntHeader);

    for (int i = 0; i < ntHeader->FileHeader.NumberOfSections; i++)
    {
        LoadSection(section);

        section++;
    }
}

void ExecutableLoader::LoadIntoModule(HMODULE module)
{
    m_module = module;

    IMAGE_DOS_HEADER* header = (IMAGE_DOS_HEADER*)m_origBinary;

    if (header->e_magic != IMAGE_DOS_SIGNATURE)
    {
        return;
    }

    IMAGE_DOS_HEADER* sourceHeader = (IMAGE_DOS_HEADER*)module;
    IMAGE_NT_HEADERS* sourceNtHeader = GetTargetRVA<IMAGE_NT_HEADERS>(sourceHeader->e_lfanew);

    IMAGE_NT_HEADERS* ntHeader = (IMAGE_NT_HEADERS*)(m_origBinary + header->e_lfanew);

    m_entryPoint = GetTargetRVA<void>(ntHeader->OptionalHeader.AddressOfEntryPoint);

    LoadSections(ntHeader);
    LoadImports(ntHeader);

    // copy over TLS index (source in this case indicates the TLS data to copy from, which is the launcher app itself)
    if (ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].Size)
    {
        const IMAGE_TLS_DIRECTORY* targetTls = GetTargetRVA<IMAGE_TLS_DIRECTORY>(
            sourceNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
        const IMAGE_TLS_DIRECTORY* sourceTls = GetTargetRVA<IMAGE_TLS_DIRECTORY>(
            ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);

        *(DWORD*)(sourceTls->AddressOfIndex) = 0;

        // note: 32-bit!
        LPVOID tlsBase = *(LPVOID*)__readfsdword(0x2C);

        DWORD oldProtect;
        VirtualProtect(reinterpret_cast<LPVOID>(targetTls->StartAddressOfRawData),
                       sourceTls->EndAddressOfRawData - sourceTls->StartAddressOfRawData, PAGE_READWRITE, &oldProtect);

        memcpy(tlsBase, reinterpret_cast<void*>(sourceTls->StartAddressOfRawData),
               sourceTls->EndAddressOfRawData - sourceTls->StartAddressOfRawData);
        memcpy((void*)targetTls->StartAddressOfRawData, reinterpret_cast<void*>(sourceTls->StartAddressOfRawData),
               sourceTls->EndAddressOfRawData - sourceTls->StartAddressOfRawData);
        /*#else
        const IMAGE_TLS_DIRECTORY* targetTls = GetTargetRVA<IMAGE_TLS_DIRECTORY>(ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_TLS].VirtualAddress);
        m_tlsInitializer(targetTls);*/
    }

#if 1
    // copy over the offset to the new imports directory
    DWORD oldProtect;
    VirtualProtect(sourceNtHeader, 0x1000, PAGE_EXECUTE_READWRITE, &oldProtect);

    sourceNtHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT] = ntHeader->OptionalHeader.DataDirectory[
        IMAGE_DIRECTORY_ENTRY_IMPORT];

    memcpy(sourceNtHeader, ntHeader,
           sizeof(IMAGE_NT_HEADERS) + (ntHeader->FileHeader.NumberOfSections * (sizeof(IMAGE_SECTION_HEADER))));
#endif
}

HMODULE ExecutableLoader::ResolveLibrary(const char* name)
{
    return m_libraryLoader(name);
}
