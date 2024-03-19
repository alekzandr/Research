#include <iostream>
#include <windows.h>
#include <ShlObj.h>
#include <objbase.h>
#include <string>

using namespace std;

void ModifyLNKFile(const wstring& lnkFilePath, const wstring& iconPath) {
    IShellLink* pShellLink = nullptr;

    HRESULT hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (SUCCEEDED(hr)) {
        hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&pShellLink);
        if (SUCCEEDED(hr)) {
            IPersistFile* pPersistFile = nullptr;

            hr = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)&pPersistFile);
            if (SUCCEEDED(hr)) {
                hr = pPersistFile->Load(lnkFilePath.c_str(), STGM_READWRITE);
                if (SUCCEEDED(hr)) {
                    // Set the new icon path
                    pShellLink->SetIconLocation(iconPath.c_str(), 0); // Second parameter is the index of the icon

                    hr = pPersistFile->Save(NULL, TRUE);
                    if (SUCCEEDED(hr)) {
                        cout << "Shortcut icon changed successfully." << endl;
                    }
                    else {
                        cout << "Failed to save the modified shortcut." << endl;
                    }
                }
                else {
                    cout << "Failed to load the shortcut file." << endl;
                }
                pPersistFile->Release();
            }
            else {
                cout << "Failed to query IPersistFile." << endl;
            }
            pShellLink->Release();
        }
        else {
            cout << "Failed to create an instance of IShellLink." << endl;
        }
        CoUninitialize();
    }
    else {
        cout << "Failed to initialize COM." << endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        cout << "Usage: " << argv[0] << " <LNK file path> <New icon path>" << endl;
        return 1;
    }

    // Convert command line arguments to wide strings
    wstring lnkFilePath = wstring(argv[1], argv[1] + strlen(argv[1]));
    wstring iconPath = wstring(argv[2], argv[2] + strlen(argv[2]));

    // Modify the shortcut
    ModifyLNKFile(lnkFilePath, iconPath);

    return 0;
}
