#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <concepts>

namespace SdkGen::Memory {
    class IMemory {
    public:
        virtual ~IMemory() = default;
        
        virtual bool ReadSized(std::uintptr_t addr, void* buffer, std::size_t size) = 0;
        virtual bool WriteSized(std::uintptr_t addr, const void* buffer, std::size_t size) = 0;
        
        template <typename T>
        T Read(const std::uintptr_t addr) {
            T value{};
            this->ReadSized(addr, reinterpret_cast<void*>(&value), sizeof(value));
            return value;
        }
        
        template <typename T>
        void Write(const std::uintptr_t addr, const T& value) {
            this->WriteSized(addr, reinterpret_cast<const void*>(&value), sizeof(value));
        }
    };
    
    class UmMemory : public IMemory {
        HANDLE hProcess_ = INVALID_HANDLE_VALUE;
        
    public:
        explicit UmMemory(const HANDLE hProcess = INVALID_HANDLE_VALUE)
            : hProcess_(hProcess) {}
        
        bool ReadSized(const std::uintptr_t addr, void* buffer, const std::size_t size) override {
            SIZE_T bytesRead = 0;
            
            if (!ReadProcessMemory(hProcess_, reinterpret_cast<LPCVOID>(addr), buffer, size, &bytesRead)) {
                return false;
            }
            
            return bytesRead == size;
        }
        
        bool WriteSized(const std::uintptr_t addr, const void* buffer, const std::size_t size) override {
            SIZE_T bytesWritten = 0;
            
            if (!WriteProcessMemory(hProcess_, reinterpret_cast<LPVOID>(addr), buffer, size, &bytesWritten)) {
                return false;
            }
            
            return bytesWritten == size;
        }
    };
}