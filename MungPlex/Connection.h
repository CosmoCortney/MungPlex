#pragma once
#include <iostream>
#include <stdio.h>
#include "GLFW/glfw3.h"
//#include "glad/glad.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "examples/libs/emscripten/emscripten_mainloop_stub.h"
#include<string>
#include "json5.hpp"
#include<fstream>
#include <iomanip>
#include "Xertz.h"
#include <Windows.h>
#include "MungPlexConfig.h"
#include <sstream>

namespace MungPlex
{
    struct GameEntity
    {
        std::string Entity = "";
        int Location;
        std::string Datatype = "";
        int Size;
        bool Hex;
        std::string Value = "";
    };

    struct SystemRegion
    {
        std::string Label = "";
        unsigned long long Base;
        unsigned long long Size;
        void* BaseLocationProcess = nullptr;

    };

    class Connection
    {
    private:
        Connection()
        {
            _emulators.push_back(std::pair<std::string, int>("Dolphin", 0));
            _emulators.push_back(std::pair<std::string, int>("Project64", 1));
        }

        ~Connection() {};
        Connection(const Connection&) = delete;
        Connection(Connection&&) = delete;
        void operator=(const Connection&) = delete;
        void operator=(Connection&&) = delete;
        static Connection& GetInstance()
        {
            static Connection Instance;
            return Instance;
        }

        int _currentPID;
        void InitProcess(std::string& processName, int connectionType, std::pair<std::string, int> emulator = std::pair<std::string, int>());
        void InitDolphin();
        std::vector<std::pair<std::string, int>> _emulators{};
        void LoadSystemInformationJSON(std::string& emuName);
        void ParseJsonToEntities();
        void ObtainGameEntities(void* location);
        std::vector<GameEntity> _gameEntities{};
        std::vector<SystemRegion> _systemRegions{};
 
        /*   static ProcessSelect instance;
           ProcessSelect();

           QString gameTitle;
           std::vector<unsigned long long> ranges;
           std::vector<RangeLayout> rangeMap;
           std::vector<MemRegion> memRegions;
           std::vector<QStringList> info;
           std::vector<MemRegion>* processMemRegions;
           Hook* hook;
           QStringList gameInfoHeader;
           QStringList gameInfo;
           bool BE;

           void initializePJ64();
           void initializeDolphin();
           void initializeCemu();
           void initializeGBA();
           void initializeGB();
           void initializeGBC();
           void initializeDesmume();
           void initializeCitra();
           void initializeMM();
           void initializeGG();
           void initializeMD();
           void initializeCD();
           void initializeYabause();
           void initializeNullDC();
           void initializeEPSXE();
           void initializePCSX2();
           void initializeRPCS3();
           void initializePPSSPP();
           void initializePC();
           */
    public:
        static void DrawWindow();
        static void SystemInformations();
        /*
        ProcessSelect(const ProcessSelect&) = delete;
        static ProcessSelect& getInstance();
        std::vector<RangeLayout> getRangeMap();
        void initialize(Hook& hook, std::vector<MemRegion>& processMemRegions, int systemType);
        void setBE(bool val);
        bool isBE();
        QStringList* getInfoHeader();
        QStringList* getInfoData();
        */
    };
}