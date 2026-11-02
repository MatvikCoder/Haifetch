// Базовые библиотеки
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>      
#include <sys/sysinfo.h>
#include <sys/utsname.h>

// Функция для форматирования времени работы (Uptime)
std::string getUptime() {
    struct sysinfo info;
    if (sysinfo(&info) != 0) return "Unknown";
    
    long uptime_secs = info.uptime;
    int hours = uptime_secs / 3600;
    int minutes = (uptime_secs % 3600) / 60;
    
    std::string result = "";
    if (hours > 0) result += std::to_string(hours) + " hours, ";
    result += std::to_string(minutes) + " mins";
    return result;
}

// Функция для получения Shell
std::string getShell() {
    // Проверяем переменную SHELL
    const char* shell_env = std::getenv("SHELL");
    if (shell_env) {
        std::string shell_path(shell_env);
        // Обрезаем путь (/bin/bash -> bash)
        size_t last_slash = shell_path.find_last_of('/');
        if (last_slash != std::string::npos) {
            return shell_path.substr(last_slash + 1);
        }
        return shell_path;
    }
    return "Unknown";
}

// Функция для получения названия видеокарты (GPU)
std::string getGPU() {
    // В Linux данные о видеокартах лежат в /sys/class/drm/card*/device/device и vendor
    // Но самый простой текстовый способ без разбора PCI-id — прочитать uevent первой карты
    std::ifstream ueventFile("/sys/class/drm/card0/device/uevent");
    std::string line;
    std::string driver = "";
    
    if (ueventFile.is_open()) {
        while (getline(ueventFile, line)) {
            if (line.find("DRIVER=") == 0) {
                driver = line.substr(7); // Получаем имя драйвера (amdgpu, radeon, nvidia, i915, nouveau и подобное)
                break;
            }
        }
        ueventFile.close();
    }
 
    // Эта функция выдаст чип через sysfs (если доступно):
    std::ifstream subsysFile("/sys/class/drm/card0/device/subsystem_device");
    if (!driver.empty()) {
        // Красиво форматируем базовую информацию
        if (driver == "amdgpu" || driver == "radeon") return "AMD ATI Radeon GPU";
        if (driver == "nvidia") return "NVIDIA GPU";
        if (driver == "i915" || driver == "xe") return "Intel HD/UHD Graphics";
        return driver + " GPU";
    }
    
    return "Unknown GPU";
}

int main() {
    // Хост и ядро
    struct utsname buffer;
    if (uname(&buffer) == 0) {
        std::cout << "Hostname: " << buffer.nodename << std::endl;
        std::cout << "Kernel:   " << buffer.release << std::endl;
    }

    // Время работы и шелл
    std::cout << "Uptime:   " << getUptime() << std::endl;
    std::cout << "Shell:    " << getShell() << std::endl;

    // Процессор
    std::ifstream cpuFile("/proc/cpuinfo");
    std::string line;
    if (cpuFile.is_open()) {
        while (getline(cpuFile, line)) {
            if (line.find("model name") != std::string::npos) {
                size_t pos = line.find(": ");
                std::cout << "CPU:      " << line.substr(pos + 2) << std::endl;
                break;
            }
        }
        cpuFile.close();
    }

    // Графический процессор
    std::cout << "GPU:      " << getGPU() << std::endl;

    // Память
    struct sysinfo memInfo;
    if (sysinfo(&memInfo) == 0) {
        long long totalRAM = (memInfo.totalram * memInfo.mem_unit) / (1024 * 1024);
        long long freeRAM = (memInfo.freeram * memInfo.mem_unit) / (1024 * 1024);
        std::cout << "Memory:   " << (totalRAM - freeRAM) << " MiB / " << totalRAM << " MiB" << std::endl;
    }

    return 0;
}

