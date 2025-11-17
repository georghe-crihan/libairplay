#include <string>
#include <vector>
#include <iostream>

#include "airplay_device.hpp"
#include "airplay_browser.hpp"

__attribute__((weak)) int main() {
    std::cout << "[+] Fetching AirPlay devices:" << std::endl;
    const auto devices = airplay_browser::get_devices();
    for (auto device : devices) {
        std::cout << "     > " << device.first << std::endl;
        std::cout << "     > " << device.second.get_printable_address() << std::endl;
    }

    std::cout << "[+] Connecting to device:" << std::endl;
    airplay_device appletv(devices.begin()->second);
//    airplay_device appletv(devices.at("Samsung Q50 Series LR"));
    std::cout << appletv.send_message(MessageType::GetServices) << std::endl;

    return 0;
}
