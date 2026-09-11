#include "TTree.h"

// External libraries
#include "GetComPortList.h"
#include "json.hpp"

using json = nlohmann::json;

TTree::TTree(const ConfigSettings& configSettings, bool backgroundScan, bool autoSetup) :
    configSettings(configSettings),
    backgroundScan(backgroundScan),
    autoSetup(autoSetup)
{
    if (backgroundScan) {
        backgroundScanThread = std::make_unique<std::thread>(backgroundScanTask);
    }

    getPorts();
}

TTree::TTree(const TTree& other) : 
    configSettings(other.configSettings),
    backgroundScan(other.backgroundScan),
    autoSetup(other.autoSetup)
{
    if (backgroundScan) {
        backgroundScanThread = std::make_unique<std::thread>(backgroundScanTask);
    }

    getPorts();
}

TTree& TTree::operator=(const TTree& other) {
    if (backgroundScan) {
        backgroundScan = false;
        backgroundScanThread->join();
        backgroundScanThread.reset();
    }

    configSettings = other.configSettings;
    backgroundScan = other.backgroundScan;
    autoSetup = other.autoSetup;
    ports = other.ports;

    if (backgroundScan) {
        backgroundScanThread = std::make_unique<std::thread>(backgroundScanTask);
    }

    return *this;
}

TTree::~TTree() {
    if (backgroundScan) {
        backgroundScan = false;
        backgroundScanThread->join();
    }
}

void TTree::backgroundScanTask() {
    while (backgroundScan) {
        scanSerialPorts();
    }
}

void TTree::getPorts() {
    ports.clear();

    std::vector<std::string> portPaths = GetComPortList::get_list_serial_ports();

    for (const std::string& portPath : portPaths) {
        ports.push_back(std::make_unique<serial::Serial>(portPath, BAUD_RATE, SERIAL_PORT_TIMEOUT));
    }
}

void TTree::scanSerialPorts() {
    for (SerialPort& port : ports) {
        if (!port->isOpen()) {
            try {
                port->open();
                DEBUG_PRINT(std::format("Found new serial device on port {}", port->getPort()));
            }
            catch (...) { }
        }

        if (port->isOpen()) {
            auto rc = pingTStick(port);

            if (rc == PingReturnCode::SUCCESS) {
                auto info = getTStickInfo(port);
                
                if (autoSetup) {
                    
                }
            }
        }
    }
}

TTree::PingReturnCode TTree::pingTStick(SerialPort& port) {
    port->write("ping");
    std::string response = port->read(4);
}

TTree::TStickInfo TTree::getTStickInfo(SerialPort& port) {
    // TODO: Error checking
    port->write("whatareyou");
    std::string name = port->readline(65536UL, ">>>").substr(3);

    DEBUG_PRINT(std::format("T-Stick name: {}", name));

    port->write("readconfig");
    std::string configJson = port->readline(65536UL, ">>>").substr(3);
    ConfigSettings config = parseConfigJson(configJson);


    return TStickInfo {
        .name = name,
        .port = port,
        .status = TStickStatus::DETECTED,
        .lastConfig = config
    };
}

TTree::ConfigSettings TTree::parseConfigJson(const std::string& jsonString) {
    json asJson = json::parse(jsonString);
    // return {
    //     .ssid = parsedJson["ssid"],
    //     .destinationIp = 
    // };
}