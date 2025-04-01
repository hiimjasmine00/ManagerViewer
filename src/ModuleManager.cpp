#include "ModuleManager.hpp"
#include <filesystem>
#include <Geode/platform/cplatform.h>
#if defined(GEODE_IS_WINDOWS)
#include <Psapi.h>
#elif defined(GEODE_IS_MACOS) || defined(GEODE_IS_IOS)
#include <mach-o/dyld.h>
#elif defined(GEODE_IS_ANDROID)
#include <link.h>
#endif

#ifdef GEODE_IS_ANDROID
int iterateDLPhdr(struct dl_phdr_info* info, size_t size, void* data) {
    if (info->dlpi_name) ((std::vector<ModuleAndAddress>*)data)->push_back({
        std::filesystem::path(info->dlpi_name).filename().string(),
        info->dlpi_name,
        (uintptr_t)info->dlpi_addr
    });
    return 0;
}
#endif

void ModuleManager::init() {
    m_modules.clear();

    #if defined(GEODE_IS_WINDOWS)
    HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, GetCurrentProcessId());
    if (!process) return;

    HMODULE* hMods = nullptr;
    DWORD cbNeeded;
    if (EnumProcessModules(process, nullptr, 0, &cbNeeded)) {
        hMods = new HMODULE[cbNeeded / sizeof(HMODULE)];
        if (EnumProcessModules(process, hMods, cbNeeded, &cbNeeded)) {
            for (unsigned int i = 0; i < (cbNeeded / sizeof(HMODULE)); i++) {
                char szModName[MAX_PATH];
                if (GetModuleFileNameExA(process, hMods[i], szModName, sizeof(szModName))) {
                    m_modules.push_back({
                        std::filesystem::path(szModName).filename().string(),
                        szModName,
                        (uintptr_t)hMods[i]
                    });
                }
            }
        }
        delete[] hMods;
    }
    #elif defined(GEODE_IS_MACOS) || defined(GEODE_IS_IOS)
    for (uint32_t i = 0; i < _dyld_image_count(); i++) {
        std::string fullName = _dyld_get_image_name(i);
        std::string filename = std::filesystem::path(fullName).filename().string();
        m_modules.push_back({
            filename,
            fullName,
            _dyld_get_image_vmaddr_slide(i) + (uintptr_t)(!filename.ends_with(".dylib") * 0x100000000)
        });
    }
    #elif defined(GEODE_IS_ANDROID)
    dl_iterate_phdr(iterateDLPhdr, &m_modules);
    #endif
}
