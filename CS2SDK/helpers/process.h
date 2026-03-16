#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <TlHelp32.h>

#include <memory>
#include <string>

#include "memory.h"

namespace SdkGen {
    class Process {
        DWORD pid_ = 0;
        HANDLE handle_ = INVALID_HANDLE_VALUE;
        
        std::uintptr_t baseAddress_ = 0;
        
        std::unique_ptr<Memory::IMemory> memory_;
        
    public:
        Process() = default;
        
        ~Process() {
            if (handle_ && handle_ != INVALID_HANDLE_VALUE) {
                CloseHandle(handle_);
            }
        }
        
        explicit Process(const std::wstring_view name) {
            Attach(name);
        }
        
        bool Attach(const std::wstring_view name) {
            HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            
            if (hSnapshot == INVALID_HANDLE_VALUE) {
                return false;
            }
            
            PROCESSENTRY32W entry;
            entry.dwSize = sizeof(PROCESSENTRY32W);
            
            if (!Process32FirstW(hSnapshot, &entry)) {
                CloseHandle(hSnapshot);
                return false;
            }
            
            do {
                if (name == entry.szExeFile) {
                    pid_ = entry.th32ProcessID;
                    break;
                }
            } while (Process32NextW(hSnapshot, &entry));
            
            CloseHandle(hSnapshot);
            
            if (!pid_) {
                return false;
            }
            
            handle_ = OpenProcess(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, FALSE, pid_);
            if (!handle_) {
                return false;
            }
            
            hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid_);
            if (hSnapshot == INVALID_HANDLE_VALUE) {
                CloseHandle(hSnapshot);
                return false;
            }
            
            MODULEENTRY32 mod{};
            mod.dwSize = sizeof(MODULEENTRY32);
            
            if (Module32First(hSnapshot, &mod)) {
                CloseHandle(hSnapshot);
                baseAddress_ = reinterpret_cast<std::uintptr_t>(mod.modBaseAddr);
            }
            
            memory_ = std::make_unique<Memory::UmMemory>(handle_);
            return true;
        }

        [[nodiscard]] Memory::IMemory& Memory() const {
            return *memory_;
        }

        [[nodiscard]] void* Handle() const {
            return handle_;
        }

        [[nodiscard]] DWORD Pid() const {
            return pid_;
        }

        [[nodiscard]] std::uintptr_t BaseAddress() const {
            return baseAddress_;
        }
        
        template <typename T>
        void SetMemoryBackend(T backend) {
            static_assert(std::derived_from<T, Memory::IMemory>);
            memory_ = std::make_unique<T>(std::move(backend));
        }
    };
}
