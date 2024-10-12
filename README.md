# MungPlex

MungPlex is a versatile game hacking and cheating tool capable of manipulating and enhancing games running on PC and several Emulators, in real-time. It allows you to experience and mung up your favorite games in many-sided ways - hence the name MungPlex. Version 2.0.0 has re-implemented all features of the deprecated repository and comes with many new ones. You can also join the affiliated [Discord server](https://discord.gg/wU62ZTvQRj) to discuss anything about it, ask questions or contribute to the project!

![image](https://github.com/user-attachments/assets/864c1518-61ee-45c4-85d9-f24635092d40)


## Before You Proceed
If the app looks off go to *Settings*, change the UI scale and restart MungPlex.</br>
It might happen for MungPlex to be unable to connect to a game. This may be due to changes to the emulator that make MungPlex incapable of finding the emulated RAM and/or game ID. If this is the case you can reach out to the MungPlex Discord server and report any problems.</br>
</br>
Even though this tool should not cause any damage to your games or system, it is possible to corrupt savegames and cause bluescreens when messing with PC games and therefore result in data loss. It is always good to have a backup of your savegames and have every opened document and project saved.
It is also possible for flashing lights and sudden loud noises to appear when editing random values. Keep this in mind especially if you are easily startled or suffer from epilepsy.
Also do not use this tool for cheating online in public sessions/rooms! Only losers do that and you may get banned from your game's network.
None of the contributors of this project bear any warranty or responsibility for any damage happening.

## MungPlex Wiki
The [MungPlex Wiki](https://github.com/CosmoCortney/MungPlex/wiki) provides a lot of useful information about MungPlex, hacking techniques, and computer science relevant for hacking games. Please refer to it if you are not sure about the usage of MungPlex or any of its features. If all explanations don't help feel free to reach out to the [Discord server](https://discord.gg/wU62ZTvQRj) and ask your question. This may also help to improve the wiki for everyone 😊

## Table of contents
- [Features](#features)
- [Releases](#releases)
- [Downloading the Source](#downloading)
- [Compiling](#compiling)
- [ToDos](#todo)
- [Special Thanks](#kiitos)

<a name="features"></a>
## Features
This is a brief list of all features worth mentioning. Check the wiki for more details.

<ul>
  <li><a href="https://github.com/CosmoCortney/MungPlex/wiki/Connection-Window">Connection</a>
    <ul>
      <li><details><summary>Connect to emulators</summary>
        <ul>
          <li>Mesen (SNES)</li>
          <li>Project64 (Nintendo 64)</li>
          <li>Dolphin (GameCube, Triforce, Wii, WiiWare)</li>
          <li>Cemu (Wii U)</li>
          <li>Yuzu (Switch) Experimental!</li>
          <li>mGBA (GameBoy (Color/Advance))</li>
          <li>melonDS (Nintendo DS)</li>
          <li>Lime3DS (3DS)</li>
          <li>No$psx (PS1)</li>
          <li>PCSX2 (PS2)</li>
          <li>RPCS3 (PS3)</li>
          <li>PPSSPP (PSP)</li>
          <li>Fusion (Mega Drive, 32X, Mega-CD)</li>
        </ul></details>
      </li>
      <li>Connect to native applications (PC games)</li>
    </ul>
  </li>
  <li>Logging</li>
  <li><a href="https://github.com/CosmoCortney/MungPlex/wiki/Search-Options">Search</a>
    <ul>
      <li><details><summary>Cheat/Value searches</summary>
        <ul>
          <li>Data Types
            <ul>
              <li>Primitives
                <ul>
                  <li>Integers (8, 16, 32, 64 bit) as either signed or unsigned</li>
                  <li>Floats with single and double precision</li>
                </ul>
              </li>
              <li>Arrays
                <ul>
                  <li>Supports unsigned integers</li>
                  <li>wildcards</li>
                </ul>
              </li>
              <li>Colors
                <ul>
                  <li>RGB888 (RGB)</li>
                  <li>RGBA8888 (RGBA)</li>
                  <li>RGB565</li>
                  <li>RGB5A3</li>
                  <li>RGBF</li>
                  <li>RGBAF</li>
                </ul>
              </li>
              <li>String/Text
                <ul>
                  <li>ASCII</li>
                  <li>UTF-8</li>
                  <li>UTF-16 LE</li>
                  <li>UTF-16 BE</li>
                  <li>UTF-32 LE</li>
                  <li>UTF-32 BE</li>
                  <li>Shift-Jis CP932</li>
                  <li>KS X 1001</li>
                  <li>Jis X 0201 Full Width Katakana</li>
                  <li>Jis X 0201 Half Width Katakana</li>
                  <li>ISO-8859-1 - ISO-8859-16</li>
                  <li>Pokémon Gen I English</li>
                  <li>Pokémon Gen I French/German</li>
                  <li>Pokémon Gen I Italian/Spanish</li>
                  <li>Pokémon Gen I Japanese</li>
                </ul>
              </li>
            </ul>
          </li></details>
          <li><details><summary>Range Options</summary>
            <ul>
              <li>Define search range</li>
              <li>Cross-Region searches</li>
              <li>Decide whether to include executable and read-only ranges</li>
              <li>Endianness select</li>
              <li>Region re-reordering</li>
            </ul></details>
          </li>
          <li><details><summary>Search Options</summary>
            <ul>
              <li>Select counter iteration to compare against</li>
              <li>Define value alignment</li>
              <li>Comparison Types
                <ul>
                  <li>Unknown Value</li>
                  <li>Known Value</li>
                </ul>
              </li>
              <li>Condition Types
                <ul>
                  <li>Equal</li>
                  <li>Unequal</li>
                  <li>Greater</li>
                  <li>Greater or Equal</li>
                  <li>Lower</li>
                  <li>Lower or Equal</li>
                  <li>Increased by</li>
                  <li>Decreased by</li>
                  <li>Value Between</li>
                  <li>Value Not Between</li>
                  <li>AND</li>
                  <li>OR</li>
                </ul>
              </li>
              <li>Accuracy
                <ul>
                  <li>applicable on floats and Colors</li>
                </ul>
              </li>
              <li>Cached Searches</li>
              <li>Searches without undo features</li>
              <li>Hex integers</li>
              <li>Case-sensitivity switch for string searches</li>
              <li>On-screen color picker</li>
            </ul></details>
          </li>
        </ul>
      </li>
      <li><details><summary>Testing Results</summary>
        <ul>
          <li>Results List
            <ul>
              <li>Columns for last results, previous results, difference, and real-time</li>
              <li>Poke/write either one value to a specific address or to all selected</li>
              <li>Options to scroll through result pages</li>
              <li>Define the max. amount of results per page</li>
            </ul>
          </li>
        </ul></details>
      </li>
    </ul>
  </li>
  <li><a href="https://github.com/CosmoCortney/MungPlex/wiki/Cheats-Window">Cheats</a>
    <ul>
      <li><details><summary>Custom Lua Functions and Variables</summary>
        <ul>
          <li>Read Functions
            <ul>
              <li>Boolen, integers (signed or unsigned), floats (single, double)</li>
              <li>Arrays of integers (signed or unsigned), floats (single, double)</li>
            </ul>
          </li>
          <li>Write Functions
            <ul>
              <li>Boolen, integers (signed or unsigned), floats (single, double)</li>
              <li>Tables of integers (signed or unsigned), floats (single, double)</li>
            </ul>
          </li>
          <li>RAM Fill and Slide
            <ul>
              <li>integers (signed or unsigned), floats (single, double)</li>
            </ul>
          </li>
          <li>Logging
            <ul>
              <li>Text, boolean integers (signed or unsigned), floats (single, double)</li>
            </ul>
          </li>
          <li>Range Check</li>
          <li>Memory Copy</li>
          <li>Registers
            <ul>
              <li>32 for each, integer, number and bool</li>
              <li>Get Module Addresses</li>
            </ul>
          </li>
        </ul></details>
          <li>Managing Cheat Lists</li>
          <li><details><summary>Convert old school cheats to Lua</summary>
            <ul>
              <li>GCN Action Replay</li>
            </ul></details>
          </li>
          <li><details><summary>Cheat Control</summary>
            <ul>
              <li>Whether to take cheats from list or text field</li>
              <li>Define execution rate</li>
            </ul></details>
          </li>
      </li>
    </ul>
  </li>
  <li><a href="https://github.com/CosmoCortney/MungPlex/wiki/Pointer-Search">Pointer Search</a>
    <ul>
      <li>Memory Dump select</li>
      <li>Cross-region scans</li>
      <li>Up to 4 layers</li>
      <li>Memory Dumpung</li>
    </ul>
  </li>
  <li><a href="https://github.com/CosmoCortney/MungPlex/wiki/Watch-&-Control">Watch & Control</a>
    <ul>
      <li>List items to watch and control values</li>
      <li><details><summary>Supported View Types</summary>
        <ul>
          <li>Integral: Int (8, 16, 32, 64 bit. Signed and unsigned)
            <ul>
              <li>Slider in Write Mode</li>
              <li>Histogram plotting in Read Mode</li>
            </ul>
          </li>
          <li>Float: Signle, Double
            <ul>
              <li>Slider in Write Mode</li>
              <li>Graph plotting in Read Mode</li>
            </ul>
          </li>
          <li>Boolen</li>
          <li>DIP Switch</li>
        </ul></details>
      </li>
      <li>Switch between Read and Write mode individually</li>
    </ul>
  </li>


 <li><a href="https://github.com/CosmoCortney/MungPlex/wiki/Device-Control">Device Control</a>
    <ul>
      <li>Create lists of devices to be controlled by games and vice versa!</li>
      <li><details><summary>Supported Devices</summary>
        <ul>
          <li>Lovense Toys</li>
        </ul></details>
      </li>
  </li>
  <li><a href="https://github.com/CosmoCortney/MungPlex/wiki/Data-Conversion">Data Conversion</a>
    <ul>
      <li>Convert Endiannesses</li>
      <li>Convert Float &lt;-&gt; Int</li>
      <li>Convert between color types</li>
      <li>View Text in different encodings as hex</li>
    </ul>
  </li>
  <li><a href="https://github.com/CosmoCortney/MungPlex/wiki/Memory-Viewer">Memory Viewer</a>
    <ul>
      <li>Just a simple memory viewer</li>
      <li>Open up to 16 instances at once</li>
    </ul>
  </li>
  <li>Process Information
    <ul>
      <li>Display Game/Process Information</li>
      <li>Display Process Regions</li>
      <li>Display Process Modules</li>
    </ul>
  </li>
  <li>Settings
    <ul>
      <li>Save many preferences</li>
    </ul>
  </li>
</ul>

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
