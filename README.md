# MungPlex

MungPlex is a cheat search tool and game enhancer for various emulators and PC games. It allows you to experience and mung up your favorite games in multiple ways - hence the name MungPlex. Version 2.0.0 has re-implemented all features of the deprecated repository and comes with some new ones. You can also join the affiliated [Discord server](https://discord.gg/wU62ZTvQRj) to discuss anything about it, ask questions or contribute to the project!

![image](https://github.com/user-attachments/assets/864c1518-61ee-45c4-85d9-f24635092d40)


## Before You Proceed
If the app looks off go to *Settings*, change the UI scale and restart MungPlex.</br>
It might happen for MungPlex to be unable to connect to a game. This may be due to changes to the emulator that make MungPlex incapable of finding the emulated RAM and/or game ID. If this is the case you can reach out to the MungPlex [Discord server](https://discord.gg/wU62ZTvQRj) and report any problems.</br>
</br>
Even though this tool should not cause any damage to your games or system, it is possible to corrupt savegames and cause bluescreens when messing with PC games and therefore result in data loss. It is always good to have a backup of your savegames and have every opened document and project saved.
It is also possible for flashing lights and sudden loud noises to appear when editing random values. Keep this in mind especially if you are easily startled or suffer from epilepsy.
Also do not use this tool for cheating online in public sessions/rooms! Only losers do that and you may get banned from your game's network.
None of the contributors of this project bear any warranty or responsibility for any damage happening.

## MungPlex Wiki
The [MungPlex Wiki](https://github.com/CosmoCortney/MungPlex/wiki) provides a lot of useful information about MungPlex, hacking techniques, and computer science relevant for hacking games. Please refer to it if you are not sure about the usage of MungPlex or any of its features. If all explanations don't help feel free to reach out to the [Discord server](https://discord.gg/wU62ZTvQRj) and ask your question. This may also help to improve the wiki for everyone 😊

## Table of contents
- [Releases](#releases)
- [Downloading the Source](#downloading)
- [Compiling](#compiling)
- [ToDos](#todo)
- [Special Thanks](#kiitos)

<a name="releases"></a>
## Releases
Find the latest release [here](https://github.com/CosmoCortney/MungPlex/releases/)

<a name="downloading"></a>
## Downloading the project's source code
- Clone the project using git
- Init all (nested) submodules using ``git submodule update --init --recursive``
- Then update all submodules using ``git pull --recurse-submodules``

<a name="compiling"></a>
## Compiling and Deployment
- update the C++ redistrubutable
- [install vcpkg](https://learn.microsoft.com/de-de/vcpkg/get_started/get-started?pivots=shell-powershell) if not done already
- use vcpkg to install the following packages (``vcpkg install xxx:x64-windows``): 
  - glfw3
  - lua
  - sol2
  - boost
  - nlohmann-json
  - discord-game-sdk
- open the project in Visual Studio as "Open as local folder" or in VSCode, Rider as a cmake project
- set ``main.cpp`` as startup item (VS: right click, ``Set as startup item``)
- compile
- copy the *resources* folder to the exe-containing folder (release build only)
- copy imgui.ini of the previous release to the exe-containing folder (release build only)

<a name="kiitos"></a>
## Special Thanks
- Lawn Meower: Idea, code, reverse engineering, [Wiki](https://github.com/CosmoCortney/MungPlex/wiki)
- [BullyWiiPlaza](https://github.com/BullyWiiPlaza/): Code, Creation of [Pointer Searcher Engine](https://github.com/BullyWiiPlaza/Universal-Pointer-Searcher-Engine)
- [Divengerss](https://www.youtube.com/channel/UCZDBXfuNiVXXb7Wbh_syiDw): Testing, reporting bugs
- [Mippy](https://github.com/Mipppy): Fixing the main frame and creation of the context menu scaffold
