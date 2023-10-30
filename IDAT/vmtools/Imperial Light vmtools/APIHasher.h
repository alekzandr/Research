#pragma once
#include <windows.h>
#include <winnt.h>

unsigned int hash_api(const char* name);
void* get_api_address_by_hash(HMODULE module, unsigned int hash);
