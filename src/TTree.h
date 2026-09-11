#pragma once

// Standard library
#include <iostream>
#include <string>
#include <thread>
#include <unordered_set>
#include <vector>

// External libraries
#include "serial.h"

// Connection constants
#define SERIAL_PORT_TIMEOUT serial::Timeout::simpleTimeout(5000)
#define BAUD_RATE 9600

// Debug settings
#define DEBUG_MODE 1
#define DEBUG_PRINT(str) if (DEBUG_MODE) std::cerr << str << std::endl;

/**
 * A class for abstracting bulk T-Stick management over a serial connection
 * @author Ian Doherty
 * @date September 2026
 */
class TTree {
public:
    typedef std::unique_ptr<serial::Serial> SerialPort;

    enum TStickStatus {
        DETECTED,
        CONFIGURED
    };
    
    enum PingReturnCode {
        SUCCESS,
        TIMEOUT
    };

    enum ConfigureReturnCode {
        SUCCESS,
        ALREADY_CONFIGURED,
        FAILURE
    };

    struct ConfigSettings {
        std::string ssid;
        std::string pw;
        std::string destinationIp;
        std::string destinationPort;
        std::string tStickDataPort = "8000";
    };

    struct TStickInfo {
        std::string name;
        SerialPort& port;
        TStickStatus status;
        ConfigSettings lastConfig;

        // Hash function
        size_t operator()(const TStickInfo& info) {
            const std::hash<std::string> hasher;
            return hasher(name);
        }
    };

    TTree(
        const ConfigSettings& configSettings,
        bool backgroundScan = true, 
        bool autoSetup = true
    );

    TTree(const TTree& other);
    TTree& operator=(const TTree& other);
    ~TTree();

    void scanSerialPorts();
    PingReturnCode pingTStick(SerialPort& port);
    TStickInfo getTStickInfo(SerialPort& port);
    ConfigureReturnCode configureTStick(SerialPort& port);
    void rebootTStick(SerialPort& port);

    std::unordered_set<TStickInfo> detectedTSticks;
    std::vector<SerialPort> ports;

private:
    // Populates the ports vector (see below); system-dependent
    void getPorts();

    // Background scanner thread task
    void backgroundScanTask();

    ConfigSettings parseConfigJson(const std::string& configJson);

    // Settings in the T-Stick config we intend to change
    ConfigSettings configSettings;

    // True if this object should scan for new T-Stick connections automatically, false otherwise
    bool backgroundScan;

    // True if this object should set up new T-Sticks automatically upon scan, false otherwise
    bool autoSetup;

    // The background scanning thread
    std::unique_ptr<std::thread> backgroundScanThread { nullptr };
};