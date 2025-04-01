// ModuleManager stores all the modules (be they .dlls, .so's, or .dylibs) that are loaded into the process, along with their base addresses.

#include <string>
#include <vector>

struct ModuleAndAddress {
    std::string name;
    std::string fullName;
    uintptr_t address;
};

class ModuleManager {
private:
    inline static ModuleManager* instance = nullptr;

    ModuleManager() {}
    std::vector<ModuleAndAddress> m_modules;
public:
    static ModuleManager* get() {
        if (!instance) {
            instance = new ModuleManager();
            instance->init();
        }
        return instance;
    }

    void init();

    const std::vector<ModuleAndAddress>& getModules() {
        return m_modules;
    }
};
