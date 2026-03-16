#include "helpers/memory.h"
#include "helpers/process.h"

#include <print>

int main() {
    const SdkGen::Process proc(L"cs2.exe");

    std::uint16_t magic = proc.Memory().Read<uint16_t>(proc.BaseAddress());
    std::println("Magic: {:#06x}", magic);
}
