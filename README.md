# MungPlex

MungPlex is a cheat search tool and game enhancer for various emulators and PC games. It allows you to experience and mung up your favorite games in multiple ways - hence the name MungPlex. Version 2.0.0 has re-implemented all features of the depricated repository and comes with some new features. 

The following feature illustration also guides you through the general usage of MungPlex.

## Before You Proceed
If the app looks off go to Settings and change the UI scale.
Even though this tool should not cause any damage to your games or system, it is possible to corrupt save data and cause blue screen and therefore result in data loss. It is good to have a backup of your save data and have every opened document and project saved.
It is also possible for flashing lights and sudden loud noises to appear when editing random values. Keep this in mind especially if you are easily startled or suffer from epilepsy.
Also do not use this tool for cheating online in public sessions/rooms! Only losers do that and you may get banned from your game's network.
None of the contributors of this project bear any warranty or responsibility for any damage happening.

## Releases
Find the latest release [here](https://github.com/CosmoCortney/MungPlex/releases/)

## Connection
The Connection frame allows you to connect to the supported emulators or any PC game. 

### Connect To An Emulator
On the top-left you will find the tab item *Emulator*.
Below you can select the target emulator from the drop-down menu.
As of version 2.0.0 the following emulators and systems are supported:
- Mesen (SNES)
- Project64 (Nintendo 64)
- Dolphin (GameCube, Triforce, Wii, WiiWare)
- Cemu (Wii U)
- Yuzu (Switch) Experimental!
- melonDS (Nintendo DS)

<img src="MungPlex\resources\img\manual\MungPlex_connection_01.png" width="256">

Before clicking *Connect* ensure the emulator AND game are running.

#### Special Cases
Some emulators need special requirements in order to be connected to.

##### Mesen
Open Mesen and disable rewind by going to `Settings/Preferences/Advanced/` and uncheck `Allow rewind to use up to...`. Also apply the contained lua script `MungPlex/resources/setMesenMungPlexFlag.lua`: In Mesen navigate to `Debug/Script Window/Open Script` and open `setMesenMungPlexFlag.lua`. Click the little gear and change the selection of `When a script window is opened` to `Load the previous script` (needs to be done only once).
Click the play button (arrow pointing to the right) to execute. 
Run this script every time you want to connect to Mesen.

##### Yuzu
The Nintendo Switch uses [ASLR (Address space layout randomization)](https://en.wikipedia.org/wiki/Address_space_layout_randomization) which causes base addresses to be randomized. Therefore no base can be determined and cheats have to be search again every time you start a game. But this should become easier if we find a way to figure out the base address each time.

### Connect To A PC Game
On the tab bar click the tab labeled *Native Application*. Under the sub-tab of *Application* you can select from all windowed applications. There you should find any PC game that is currently running. Alternatively you can select from any process of your PC by clicking the *Process* tab instead. 


<img src="MungPlex\resources\img\manual\MungPlex_connection_03.png" width="256">

If a game crashes or MungPlex has been opened before it, click *Refresh List* to refresh the process lists. Once a game has been selected click *Connect*.

<img src="MungPlex\resources\img\manual\MungPlex_connection_02.png" width="256">

Once Connected you can use MungPlex and open the Memory Viewer down below.

## Memory Viewer
A simple memory viewer to view and edit any byte in real-time.
To open click *Open Memory Viewer* in the *Connection* frame.

<img src="MungPlex\resources\img\manual\MungPlex_memoryviewer_01.png" width="256">

- `Region`: Select the desired memory region
- `Jump to Address`: Jump to Address (d'uh)
- `Read Size`: Change the size of the viewed memory by either typing any hexadecimal value or using the +/- buttons.
- `Options`: Change view options
- `Editing`: Simply click the byte(s) you wanna edit and type the desired hex values.

## Search
This frame allows for scanning a game's memory for values of several types under any desired conditions.

### Value Type Options
Here you can specify all value-related configurations.

<img src="MungPlex\resources\img\manual\MungPlex_search_01.png" height="180">

- `Value Type`: Select the major value type.
  - `Primitive`: This covers all common primitive value types (signed/unsigned integers of 1, 2, 4 and 8 bytes, as well as floating point values of single and double precision)
  - `Array`: Allows you to specify an array of any length of unsigned integers (1, 2, 4 and 8 bytes). Additionally you can define wildcards (indices to ignore) by simply typing a # for the corresponding index. The hex check-box won't affect the numeral base so you have to prepend *0x* for hex values. Example (unsigned int16 array): ´0x1337, 069, 420, #´. 0x1337 (hex), 069 (oct), 420 (dec), # (wildcard)
  - `Text`: Allows you to search for text values (strings) of different types of encodings.
Currently supported encodings: 
    - `ASCII`
    - `UTF-8`
    - `UTF-16 LE`
    - `UTF-16 BE`
    - `UTF-32 LE`
    - `UTF-32 BE`
    - `Shift-Jis`
    - `ISO-8859-1` to `ISO-8859-16`
  - `Color`: Allows you to search for color values. If you don't know what color type certain objects or textures use you may need to try different types.
    - `RGB888`: Common 24-bit color value commonly used in 3D games
    - `RGBA8888`: Same as above but with an additional alpha channel
    - `RGB565`: 16-bit color value commonly used in textures of GameCube games
    - `RGBF`: Floating point/HDR variant of RGB888. Commonly used of colors may do smooth transitions like the engine flames in F-Zero GX or ink in Splatoon
    - `RGBAF`: Same as RGBF but with an additional alpha channel

- `Primitive/Array/Text/Color Type `: 
Selections for the subsidiary data types.

- `Big Endian `: 
Check this if the values should be treated as big endian (reversed byte order). The best option is auto-selected but you can always alter this by yourself if needed.

- `Signed `: 
Check this if primitive integral types should be signed.

### Range Options
Here you can select the memory ranges to be scanned.

<img src="MungPlex\resources\img\manual\MungPlex_search_02.png" height="128">

- `Region`: Select a specific region.
- `Start at (hex)`: Define the address of where the memory scan should start.
- `End at (hex)`: Define the address of where the memory scan should end.
- `Cross-Region`: Checking this will consider scanning all available memory regions. By altering *Start at* and *End at* you can skip certain regions. 
- `Re-reorder Region`: Some emulators and memory areas may be reordered in 4-byte chunks of the opposite endiannes. If this is the case you may check this. The best option is auto-selected but you can change if needed.
- `Write`: Only accessible when connected to a PC game. This ensures all regions also have write permissions.
- `Exec. `: Only accessible when connected to a PC game. This adds regions with execute permissions to the region list.

### Search Options
Here you can control the memory scan.

<img src="MungPlex\resources\img\manual\MungPlex_search_03.png" height="240">

- `Counter Iteration`: Select the counter iteration you want to compare against. The last one is always auto-selected so if you happen to make an error during search you can always jump back.
- `Alignment`: Specifies the byte alignment of each scanned value. An alignment of 4 is usually the best for any kind of value. Values of type int8 and int16 may use an alignment of 1 or 2 respectively.

- `Comparision Type`: 
  - Unknown Value: Compares the following iteration's values against the current/counter iteration ones.
  - Known Value: Scans the memory against a specific value

- `Condition Type`: 
  - `Equal`: Scans for equality
  - `Unequal`: Scans for changed values
  - `Greater`: Scan for values greater than the current or known value
  - `Greater or Equal`: Scan for values greater than or equal to the current or known value
  - `Lower`: Scan for values lower than the current or known value
  - `Lower or Equal`: Scan for values lower than or equal to the current or known value
  - `Increased by`: Scans for values increased by a specific amount
  - `Decreased by`: Scans for values decreased by a specific amount
  - `Value Between`: Scans for values between 2 specific values
  - `Value Not Between`: Scans for values outside the range of 2 specific values
  - `AND`: Scans for values where all true bits are set within the current or known value
  - `OR`: Scans for values where at least one true bit is set within the current or known value

- `Value`: The known value, increment, decrement or range begin to be scanned for.
- `Secondary Value`: Range end for range-based seraches.
- `Precision (%)`: Accuracy for floating point and color searches. Best is a value of 90 - 100%.
- `Cached`: Check if you want previous search results to be held in RAM (not recommended). This may speed up your search but also increases memory usage.
- `Values are hex`: Interprets input integral values as hexadecimal. Do not prepend *0x*. Results of integrals and arrays are also affected. Arrays to be scanned for are not.
- `Case Sensitive`: Whether strings to be searched for should be case sensitive (recommended). Case insensitive searches are slow.
- `Color Select`: Use this to select the target color value.
- `Color Wheel`: Use color wheel instead of square.
- `Pick color from screen`: Allows you to pick any color of the screen by click.
- `Search`: Performs a new search or new iteration.
- `Reset`: Resets current search.

### Results Panel
Here are all results listed. Values can be altered and written back to the game for testing.

<img src="MungPlex\resources\img\manual\MungPlex_search_04.png" height="360">


- `Results`: Displays the amount of found values.
- `Address`: The address of the selected results item.
- `Value`: The value of the selected reselts item.
- `Page`: Page navigation through all results. Enter any number to jump to the desired page or click the +/- buttons.
- `Previous`: Check this if you want to poke the previous value to the game.
- `Multi-Poke`: Check this if you want to poke multiple values to the game. To select multiple values hold *ctrl* (de: *strg*) and click each item you want. 
To select a range of results click the first desired item and hold *shift* while clicking the last one.
Sadly there's some bug going on so you might need to click again if the selection didn't update.
- `Poke`: Writes all selected values to the game.
- `Max. results per page`: Specify how many results you want to have been drawn on each page.

## Cheats
This frame allows you to execute cheats of your search results as lua scripts. If you don't know the lua programming language you can check out the [official lua reference manual](https://www.lua.org/manual/5.4/). 

### MungPlex Custom Lua Functions and Variables
In order to interact with games it is necessary to have custom functionalities.
In the following you will learn about all additional features required to create MungPlex cheat codes.

#### Read Functions
The following functions can be used to read data from game memory:
- `bool ReadBool(uint64 address)`: returns the boolean value located at `address` of the game's memory
- `int8 ReadInt8(uint64 address)`: returns the signed 8-bit value located at `address` of the game's memory
- `int16 ReadInt16(uint64 address)`: returns the signed 16-bit value located at `address` of the game's memory
- `int32 ReadInt32(uint64 address)`: returns the signed 32-bit value located at `address` of the game's memory
- `int64 ReadInt64(uint64 address)`: returns the signed 64-bit value located at `address` of the game's memory
- `uint8 ReadUInt8(uint64 address)`: returns the unsigned 8-bit value located at `address` of the game's memory
- `uint16 ReadUInt16(uint64 address)`: returns the unsigned 16-bit value located at `address` of the game's memory
- `uint32 ReadUInt32(uint64 address)`: returns the unsigned 32-bit value located at `address` of the game's memory
- `uint64 ReadUInt64(uint64 address)`: returns the unsigned 64-bit value located at `address` of the game's memory
- `float ReadFloat(uint64 address)`: returns the float value located at `address` of the game's memory
- `double ReadDouble(uint64 address)`: returns the double value located at `address` of the game's memory

#### Write Functions
- `WriteBool(uint64 address, bool value)`: writes the boolean `value` to `address` of the game's memory
- `WriteInt8(uint64 address, int8 value)`: writes the int8 `value` to `address` of the game's memory
- `WriteInt16(uint64 address, int16 value)`: writes the int16 `value` to `address` of the game's memory
- `WriteInt32(uint64 address, int32 value)`: writes the int32 `value` to `address` of the game's memory
- `WriteInt64(uint64 address, int64 value)`: writes the int64 `value` to `address` of the game's memory
- `WriteFloat(uint64 address, float value)`: writes the float `value` to `address` of the game's memory
- `WriteDouble(uint64 address, double value)`: writes the double `value` to `address` of the game's memory

#### Registers
These variables can be used to store and keep values across execution cycles and different cheats.
- `INTREG00` - `INTREG31`: Integer registers
- `NUMREG00` - `NUMREG31`: Float registers
- `BOOLREG00` - `BOOLREG31`: Boolean registers

### Cheat List
A list of all saved cheats. Check each one you want to be active.

<img src="MungPlex\resources\img\manual\MungPlex_cheats_01.png" height="64">

### Cheat Information
This can be used to edit an existing cheat or add a new one.

<img src="MungPlex\resources\img\manual\MungPlex_cheats_02.png" height="666">

- `Title`: The Cheat's title
- `Hacker(s)`: Who has made the cheat
- `Lua Cheat`: The Lua Cheat script
- `Description`: Description and usage information about the cheat
- `Add To List`: Adds new cheat to list
- `Update Entry`: Updates the currently selected entry
- `Delete Entry`: Deletes the selected cheat from the list

### Cheat Control
Gives you further control of the cheat(s).

<img src="MungPlex\resources\img\manual\MungPlex_cheats_03.png" height="86">

- `Cheat List/Text Cheat`: Whether to execute all selected cheats or the cheat in the right text field (*Lua Cheat*)
- `Interval`: How many times a secon the cheat(s) should be executed. If the game visibly overwrites your values you may move the slider to the right.
- `Apply/Terminate Cheats`: Turns cheats on or off. If some syntactical error appears you will be notified bt the log window.

## Pointer Search
This frame allows scanning memory dumps for pointer paths. The used [pointer search engine](https://github.com/BullyWiiPlaza/Universal-Pointer-Searcher-Engine) was developed by BullyWiiPlaza. 


### Add File
Opens a file dialog to add memory dumps to the list.

<img src="MungPlex\resources\img\manual\MungPlex_pointersearch_01.png" height="86">

- `Starting Address`: The memory dump's virtual starting address.
- `Target Address`: Where the value has been found within the memory dump.
- `Correspondence`: The corresponding Search. 0 = initial, 1 = second, ...

## Scan Options
<img src="MungPlex\resources\img\manual\MungPlex_pointersearch_02.png" height="440">

- `System Preset`: Auto-selects certain settings depending on the selected system.
- `Big Endian`: Whether the memory dumps are big endian (reversed byteorder)
- `Print Visited Addresses`: Additionally print visited addresses
- `Print Module Names`: Whether to print file/module names instead of initial address
- `Minimum Offset`: Smallest offset value to be considered. Negative values allowed. A lower value may increase results count but also the scan time
- `Maximum Offset`: Biggest offset value to be considered. A bigger value may increase results count but also increase scan time
- `Min. Pointer Depth`: Minimum pointer depth level. A value of 1 means a single pointer redirection is being considered. Values bigger than 1 mean that pointers may redirect to other pointers. This value is usually always 1
- `Max. Pointer Depth`: Maximum pointer depth level. A value of 1 means a single pointer redirection is considered. Values bigger than 1 mean that pointers may redirect to other pointers. This value can be the same as *Min. Pointer Depth* if you don't want any extra depth. A higher value will increase the results count but also scan time
- `Address Width`: Address width of the dump's system
- `Results File`: Where to save the results file
- `Max. Pointer Count`: Maximum amount of pointers to be generated. Smaller values may decrease scan time and but also the likeability to find working pointer paths
- `Input Filetype`: Whether to use Memory Dumps or Pointer Maps
- `Clear List`: Clears the dump file list
- `Scan`: Performs the pointer scan
- `Results`: Lists all results
- `Region`: Select region to be dumped
- `Dump`: Dump selected region 

## Process Information
Displays various information about the connected game/process

## Settings
Define various settings and preferences.

### General Settings
- `Documents Path`: Where MungPlex saves everything. It is recommended picking a place that is on an SSD for better performance since this tool may handle large files
- `UI-Scale`: Scale of the app. Change this if it looks off.
- `Default Active Window`
- `Color Theme`

### Search Settings
- `Case Sensitive by default`
- `Color Wheel by default`
- `Default Alignment`
- `Values are hex by default`
- `Cached Searches by default`

### Cheats Settings
- `Cheat List by default`
- `Default Interval`


## Compiling

Open the project in Visual Studio as "Open as local folder".

Using `vcpkg`, make sure to install the following libraries (`vcpkg install xxx:x64-windows`):
* `sol2` ([example project](https://github.com/BullyWiiPlaza/Sol2WithCMake))
* `glfw`
* `nlohmann-json`

To finally compile and launch `MungPlex`, select `MungPlex.exe` as startup item and build/debug as usual in Visual Studio.

## TODO
- General
  - Migrate `imgui` to using `vcpkg` (e.g. install `imgui[glfw-binding]`)
  - Texture Streaming
  - Corruptor
  - Value watch and control
  - Data Conversion
- Search 
  - Text Types: `JIS X 0201`, `JIS X 0208`, Pokemon game text encodings, `EUC-KR`, `EUC-JP`, `EUC-CN`, `EUC-TW`, `Big5`, `base64`, ...
  - Color types: `RGB332`, `RGB444`, `RGBA4444`, `RGB555`, `RGBA5551`, `RGB5A3`, `IA8`, ...
  - Array types: float, double
- Pointer Search
  - Fix potential problems with argument list
- Connection
  - Mesen NES support
  - More Emulators: Visual Boy Advance, Fusion, Yabause, NullDC, EPSXE, PCSX2, RPCS3, PPSSPP, Citra, TeknoParrot, ...
- Cheats
  - Syntax Highlighting, more OS functionalities



