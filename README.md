# MungPlex

MungPlex is a cheat search tool and game enhancer for various emulators and PC games. It allows you to experience and mung up your favorite games in multiple ways - hence the name MungPlex. Version 2.0.0 has re-implemented all features of the depricated repository and comes with some new features. You can also join the affiliated [Discord Server](https://discord.gg/wU62ZTvQRj) (may be renamed).

The following feature illustration also guides you through the general usage of MungPlex.

## Before You Proceed
If the app looks off go to Settings and change the UI scale.</br>
It may happen for MungPlex to be unable to connect to a game. This might be due to changes to the emulator that make MungPlex incapable of finding the emulated RAM and/or game ID. If this is the case you can reach out to the MungPlex Discord Server and report any problems.</br>
</br>
Even though this tool should not cause any damage to your games or system, it is possible to corrupt savegames and cause bluescreens when messing with PC games and therefore result in data loss. It is always good to have a backup of your savegames and have every opened document and project saved.
It is also possible for flashing lights and sudden loud noises to appear when editing random values. Keep this in mind especially if you are easily startled or suffer from epilepsy.
Also do not use this tool for cheating online in public sessions/rooms! Only losers do that and you may get banned from your game's network.
None of the contributors of this project bear any warranty or responsibility for any damage happening.


## Table of contents
- [Releases](#releases)
- [Connection](#connection)
- [Memory Viewer](#memoryviewer)
- [Search](#search)
- [Cheats](#cheats)
- [Pointer Search](#pointersearch)
- [Watch & Control](#watchcontrol)
- [Process Information](#processinformation)
- [Settings](#settings)
- [Data Conversion](#dataconversion)
- [Compiling](#compiling)
- [To Do](#todo)
- [Special Thanks](#kiitos)

<a name="releases"></a>
## Releases
Find the latest release [here](https://github.com/CosmoCortney/MungPlex/releases/)

<a name="connection"></a>
## Connection
The Connection frame features an emulator select and process select for PC games and programs. 

## Discord Rich Presence
You can show what you're doing with MungPlex via Discord Rich Presence. To prevent games and emulators from surpressing MungPlex from the Rich Presence disable Rich Presence in the game/emulator settings.
If you don't want to have this enabled you can leave this option disabled in the settings.

### Connecting To An Emulator
On the top-left you will find the *Emulator* tab item.
Below you can select the target emulator from the drop-down menu.
As of version 2.3.0 the following emulators and systems are supported:
- Mesen (SNES)
- Project64 (Nintendo 64)
- Dolphin (GameCube, Triforce, Wii, WiiWare)
- Cemu (Wii U)
- Yuzu (Switch) Experimental!
- melonDS (Nintendo DS)
- No$psx (PS1)
- PCSX2 (PS2)
- RPCS3 (PS3)
- PPSSPP (PSP)
- Fusion (Mega Drive, 32X, Mega-CD)

The choice of these emulators was made by considering factors like what other debugging features they offer and how easily the emulated RAM, loaded ROM (for older games) and game information can be found.
Therefore an outdated emulator like Fusion has bullet-proof support where on the other hand support for frequently updated emulators like RPCS3 may need to be adjusted from time to time. Multi-platform emulators like 
RetroArch also make it unnecessarily difficult to find everything needed since it's not only the emulator itself that experiences updates but also the underlying cores. 

<img src="MungPlex\resources\img\manual\MungPlex_connection_01.png" width="256">

Before clicking *Connect* ensure the emulator AND game are running.

#### Special Cases
Some emulators need special requirements in order to be connected to.

##### Mesen
Open Mesen and disable rewind by going to `Settings/Preferences/Advanced/` and uncheck `Allow rewind to use up to...`. Also apply the contained lua script `MungPlex/resources/setMesenMungPlexFlag.lua`: In Mesen navigate to `Debug/Script Window/Open Script` and open `setMesenMungPlexFlag.lua`. Click the little gear and change the selection of `When a script window is opened` to `Load the previous script` (needs to be done only once).
Click the play button (arrow pointing to the right) to execute. 
Run this script every time you want to connect to Mesen if it is not executed automatically.

##### Yuzu
Before connecting to Yuzu go to Yuzu's menu, then `Emulation/General/UI` and set for `Row 1 Text:` *Title Name*, and for `Row 2 Text:` *Title ID*. MungPlex requires this to be able to find Title IDs once the Game Name has been detected. If the Title ID could not be found restart the game or emulator entirely and retry.<br>
The Nintendo Switch uses [ASLR (Address space layout randomization)](https://en.wikipedia.org/wiki/Address_space_layout_randomization) which causes base addresses to be randomized. Therefore no base can be determined yet and therefore cheats have to be searched again every time you restart the game. A lot of research has to be done here.

### Connecting To A PC Game
On the tab bar click the *Native Application* tab. Under the sub-tab of *Application* you can select from all windowed applications. There you should find any PC game that is currently running. Alternatively you can select from any process of your PC by clicking the *Processes* tab instead. 


<img src="MungPlex\resources\img\manual\MungPlex_connection_03.png" width="256">

If a game crashes or MungPlex has been opened before it, click *Refresh List* to refresh the process lists. Once a game has been selected click *Connect*.

<img src="MungPlex\resources\img\manual\MungPlex_connection_02.png" width="256">

Once Connected you can use MungPlex and open the Memory Viewer down below.

<a name="memoryviewer"></a>
## Memory Viewer
A simple memory viewer to view and edit any byte in real-time.
To open click *Open Memory Viewer* on the *Connection* frame.

<img src="MungPlex\resources\img\manual\MungPlex_memoryviewer_01.png" width="256">

- `Region`: Select the desired memory region
- `Jump to Address`: Jumps to the target Address (d'uh)
- `Read Size`: Change the size of the viewed memory by either typing any hexadecimal value or using the +/- buttons.
- `Options`: Change view options
- `Editing`: Simply click the byte(s) you wanna edit and type the desired hex values.

<a name="search"></a>
## Search
This frame allows for scanning a game's memory for values of several types under any desired conditions.

### Value Type Options
Here you can specify all value-related configurations.

<img src="MungPlex\resources\img\manual\MungPlex_search_01.png" height="180">

- `Value Type`: Select the major value type.
  - `Primitive`: This covers all common primitive value types (signed/unsigned integers of 1, 2, 4 and 8 bytes, as well as floating point values of single and double precision)
  - `Array`: Allows you to specify an array of any length of unsigned integers (1, 2, 4 and 8 bytes). Additionally you can define wildcards (indices to be ignored) by simply typing a # for the corresponding index. The hex check-box won't affect the numeral base so you have to prepend *0x* for hex values. Example (unsigned int16 array): `0x1337, 069, 420, #` (0x1337 (hex), 069 (oct), 420 (dec), # (wildcard))
  - `Text`: Allows for searching text values (strings) of different types of encodings.
Currently supported encodings: 
    - `ASCII`
    - `UTF-8`
    - `UTF-16 LE`
    - `UTF-16 BE`
    - `UTF-32 LE`
    - `UTF-32 BE`
    - `Shift-Jis`
    - `Jis X 0201 Full Width`
    - `Jis X 0201 Half Width`
    - `ISO-8859-1` - `ISO-8859-16`
  - `Color`: Allows for searching color values. If you don't know what color type certain objects or textures use you may need to try different types.
    - `RGB888`: 24-bit color value commonly used in 3D games
    - `RGBA8888`: Same as above but with an additional 8-bit alpha channel
    - `RGB565`: 16-bit color value commonly used in textures of GameCube/Wii games
    - `RGB5A3`: 16-bit color value with alpha flag commonly used in textures of GameCube/Wii games
    - `RGBF`: Floating point/HDR variant of RGB888. Commonly used for colors that require smooth transitions like the engine flames in F-Zero GX or ink in Splatoon
    - `RGBAF`: Same as RGBF but with an additional alpha channel

- `Primitive/Array/Text/Color Type `: 
Selections for the subsidiary data types.

- `Big Endian `: 
Check this if the values should be treated as big endian (reversed byte order). The best option is auto-selected but you can always alter this by yourself if needed.

- `Signed `: 
Check this if primitive integral types should be signed.

- `Force Alpha `: 
Some Color types (RGB5A3) feature a color bit flag instead of a dedicated variant with alpha channel. Therefore forcing the alpha flag might come in handy.

### Range Options
Here you can select the memory ranges to be scanned.

<img src="MungPlex\resources\img\manual\MungPlex_search_02.png" height="128">

- `Region`: Select a specific region.
- `Start at (hex)`: Define the address of where the memory scan should start.
- `End at (hex)`: Define the address of where the memory scan should end.
- `Cross-Region`: Checking this will consider scanning all available memory regions. By altering *Start at* and *End at* you can bypass certain regions. 
- `Re-reorder Region`: Some emulators and memory areas may be reordered in 4-byte chunks of the opposite endiannes. If this is the case you may check this. The best option is auto-selected but you can change it if desired.
- `Write`: Only available when connected to a PC game. This ensures all regions also have write permissions.
- `Exec. `: Only available when connected to a PC game. This adds regions with execute permissions to the region list.

### Search Options
Here you can control the memory scan.

<img src="MungPlex\resources\img\manual\MungPlex_search_03.png" height="240">

- `Counter Iteration`: Select the counter iteration you want to compare against. The last one is always auto-selected so if you happen to make an mistake during the search process you can always jump back.
- `Alignment`: Specifies the byte alignment of each scanned value. An alignment of 4 is usually the best for big-endian systems and generally any kind of value bigger than 2 bytes. Values of type int8 and int16 may use an alignment of 1 and 2 respectively.

- `Comparision Type`: 
  - Unknown Value: Compares the following iteration's values against the current/counter iteration ones.
  - Known Value: Scans the memory against a specific value

- `Condition Type`: 
  - `Equal`: Scans for equality
  - `Unequal`: Scans for changed values/inequality
  - `Greater`: Scans for values greater than the current or known value
  - `Greater or Equal`: Scans for values greater than or equal to the current or known value
  - `Lower`: Scans for values lower than the current or known value
  - `Lower or Equal`: Scans for values lower than or equal to the current or known value
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
- `Values are hex`: Interprets input integral values as hexadecimal. Do not prepend *0x* to the known value. Results of integrals and arrays are also affected. Arrays to be scanned for are not.
- `Case Sensitive`: Whether strings to be searched for should be case sensitive (recommended). Case insensitive searches are slower but may deliver more results.
- `Color Select`: Use this to select the target color value.
- `Color Wheel`: Use color picker wheel instead of square.
- `Pick color from screen`: Allows for picking any color off the screen by click.
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

<a name="cheats"></a>
## Cheats
This frame allows you to execute cheats of your search results as lua scripts. If you are unfamiliar with the lua programming language you can check out the [official lua reference manual](https://www.lua.org/manual/5.4/). Not deep understand of Lua is required to create simple cheats.

### MungPlex Custom Lua Functions and Variables
In order to interact with games it is necessary to have custom functionalities.
In the following you will learn about all additional features required to create MungPlex cheat codes.

#### Read Functions
The following functions can be used to read data from memory:

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
- `table ReadArrayInt8(const uint64_t address, uint32_t size)`: returns the signed 8-bit array of `size` located at `address` of the game's memory
- `table ReadArrayUInt8(const uint64_t address, uint32_t size)`: returns the unsigned 8-bit array of `size` located at `address` of the game's memory
- `table ReadArrayInt16(const uint64_t address, uint32_t size)`: returns the signed 16-bit array of `size` located at `address` of the game's memory
- `table ReadArrayUInt16(const uint64_t address, uint32_t size)`: returns the unsigned 16-bit array of `size` located at `address` of the game's memory
- `table ReadArrayInt32(const uint64_t address, uint32_t size)`: returns the signed 32-bit array of `size` located at `address` of the game's memory
- `table ReadArrayUInt32(const uint64_t address, uint32_t size)`: returns the unsigned 32-bit array of `size` located at `address` of the game's memory
- `table ReadArrayInt64(const uint64_t address, uint32_t size)`: returns the signed 64-bit array of `size` located at `address` of the game's memory
- `table ReadArrayUInt64(uint64_t address, uint32_t size)`: returns the unsigned 64-bit array of `size` located at `address` of the game's memory
- `table ReadArrayFloat(uint64_t address, uint32_t size)`: returns the float array of `size` located at `address` of the game's memory
- `table ReadArrayDouble(uint64_t address, uint32_t size)`: returns the double array of `size` located at `address` of the game's memory

#### Write Functions
The following functions can be used to write data to game memory:

- `WriteBool(uint64 address, bool value)`: writes the boolean `value` to `address` of the game's memory
- `WriteInt8(uint64 address, int8 value)`: writes the int8 `value` to `address` of the game's memory
- `WriteInt16(uint64 address, int16 value)`: writes the int16 `value` to `address` of the game's memory
- `WriteInt32(uint64 address, int32 value)`: writes the int32 `value` to `address` of the game's memory
- `WriteInt64(uint64 address, int64 value)`: writes the int64 `value` to `address` of the game's memory
- `WriteFloat(uint64 address, float value)`: writes the float `value` to `address` of the game's memory
- `WriteDouble(uint64 address, double value)`: writes the double `value` to `address` of the game's memory
- `WriteArrayInt8(uint64_t address, table array)`: writes the int8 `array` to `address` of the game's memory
- `WriteArrayInt16(uint64_t address, table array)`: writes the int16 `array` to `address` of the game's memory
- `WriteArrayInt32(uint64_t address, table array)`: writes the int23 `array` to `address` of the game's memory
- `WriteArrayInt64(uint64_t address, table array)`: writes the int64 `array` to `address` of the game's memory
- `WriteArrayFloat(uint64_t address, table array)`: writes the float `array` to `address` of the game's memory
- `WriteArrayDouble(uint64_t address, table array)`: writes the double `array` to `address` of the game's memory

#### RAM Fill and Slide
These functions consecutively write values and increment/decrement those alongside the address as many times as defined by `count`. Note that an address increment does not consider the value size. For instance, to consecutively write int32 values the minimum desired address increment would be 4 or -4. Increment values are signed(!). The first write does not apply any increment. If you don't want a value increment just pass a valueIncrement of 0.

- `FillAndSlideInt8(uint64_t address, int64_t addressIncrement, int8_t value, int8_t valueIncrement, uint8_t count)`: consecutively writes `value` + `valueIncrement` to `address` + `addressIncrement` `count` times 
- `FillAndSlideInt16(uint64_t address, int64_t addressIncrement, int16_t value, int16_t valueIncrement, uint16_t count)`: consecutively writes `value` + `valueIncrement` to `address` + `addressIncrement` `count` times 
- `FillAndSlideInt32(uint64_t address, int64_t addressIncrement, int32_t value, int32_t valueIncrement, uint32_t count)`: consecutively writes `value` + `valueIncrement` to `address` + `addressIncrement` `count` times 
- `FillAndSlideInt64(uint64_t address, int64_t addressIncrement, int64_t value, int64_t valueIncrement, uint32_t count)`: consecutively writes `value` + `valueIncrement` to `address` + `addressIncrement` `count` times 
- `FillAndSlideFloat(uint64_t address, int64_t addressIncrement, float value, float valueIncrement, uint32_t count)`: consecutively writes `value` + `valueIncrement` to `address` + `addressIncrement` `count` times 
- `FillAndSlideDouble(uint64_t address, int64_t addressIncrement, double value, double valueIncrement, uint32_t count)`: consecutively writes `value` + `valueIncrement` to `address` + `addressIncrement` `count` times 

#### Log Functions
- `LogText(const char* text)`: Logs the given `text` to the log frame
- `LogUInt8(const uint8_t value, bool hex)`: Logs the given uint8 `value` to the log frame. if `hex` is *true* printed value will be hex
- `LogUInt16(uint16_t value, bool hex)`: Logs the given uint16 `value` to the log frame. if `hex` is *true* printed value will be hex
- `LogUInt32(uint32_t value, bool hex)`: Logs the given uint32 `value` to the log frame. if `hex` is *true* printed value will be hex
- `LogUInt64(uint64_t value, bool hex)`: Logs the given uint64 `value` to the log frame. if `hex` is *true* printed value will be hex
- `LogInt8(int8_t value, bool hex)`: Logs the given int8 `value` to the log frame. if `hex` is *true* printed value will be hex
- `LogInt16(int16_t value, bool hex)`: Logs the given int16 `value` to the log frame. if `hex` is *true* printed value will be hex
- `LogInt32(const int32_t value, bool hex)`: Logs the given int32 `value` to the log frame. if `hex` is *true* printed value will be hex
- `LogInt64(int64_t value, bool hex)`: Logs the given int64 `value` to the log frame. if `hex` is *true* printed value will be hex
- `LogFloat(float value)`: Logs the given float `value` to the log frame
- `LogDouble(double value)`: Logs the given double `value` to the log frame
- `LogBool(bool value)`: Logs the given bool `value` to the log frame

#### Misc. Functions
- bool `IsInRange(uint64_t value, uint64_t start, uint64_t end)`: Checks if value is >= start and < end. This can be used to verify pointers are within a valid range to prevent possible crashes during loading times
<!-- uint64_t `GetModuleAddress(char* moduleName)`: returns the address of a processe's module (e.g. GetModuleAddress("mono.dll")) -->
- `copyMemory(uint64_t source, uint64_t destination, uint32_t size)`: copies the `size` bytes of memory located at `source` ot `destination` 

#### Registers
These variables can be used to store and keep values across execution cycles and different cheats.
- `INTREG00` - `INTREG31`: Integer registers
- `NUMREG00` - `NUMREG31`: Float registers
- `BOOLREG00` - `BOOLREG31`: Boolean registers

#### Module Addresses
Module addresses can be returned by calling the `Modules` field and the target module name in brackets. (Modules["mono.dll"])

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

### Old-School Cheat to Lua Cheat Conversion
This allows you to convert decrypted cheat codes to Lua. Note that some codes may not work because addresses may be shifted on emulators.

<img src="MungPlex\resources\img\manual\MungPlex_cheats_04.png" width="666">

- `Cheat Format`: Select the input cheat format
- `Cheat to be converted`: The decrypted cheat code you wish to be converted
- `Convert to Lua`: A button that does what it says

### Cheat Control
Gives you further control of the cheat(s).

<img src="MungPlex\resources\img\manual\MungPlex_cheats_03.png" height="86">

- `Cheat List/Text Cheat`: Whether to execute all selected cheats or the cheat in the right text field (*Lua Cheat*)
- `Interval`: How many times a secon the cheat(s) should be executed. If the game visibly overwrites your values you may move the slider to the right.
- `Apply/Terminate Cheats`: Turns cheats on or off. If some syntactical error appears you will be notified bt the log window.

<a name="pointersearch"></a>
## Pointer Search
This frame allows scanning memory dumps for pointer paths. The used [pointer search engine](https://github.com/BullyWiiPlaza/Universal-Pointer-Searcher-Engine) was developed by BullyWiiPlaza. 


### Add File
Opens a file dialog to add memory dumps to the list.

<img src="MungPlex\resources\img\manual\MungPlex_pointersearch_01.png" height="86">

- `Starting Address`: The memory dump's virtual starting address.
- `Target Address`: Where the value has been found within the memory dump.
- `Correspondence`: The corresponding Search. 0 = initial, 1 = second, ...

### Scan Options
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

<a name="watchcontrol"></a>
## Watch & Control
This window allows for viewing and controlling certain values of various types.

<img src="MungPlex\resources\img\manual\MungPlex_Watch&Control_01.png" height="520">

### List Settings
- `New Item's Type`: Select the generic type of value watch
  - `Integral`: Integer types
  - `Floats`: Floating point types
  - `Bool`: Boolean value
  - `DIP Switch`: 32-bit DIP Switch

- `Add Item`: Adds a new item of the selected type to the list
- `Save List`: Saves list

### View Settings (All Types)
- `Title`: Name of the entry
- `Active`: Whether the view is active or not
- `Int/Float Type`: Subsidiary type (not applicable for bools and DIP switches) 
- `Delete`: Deletes the entry
- `Write`: Writes the defined value to the game instead of reading it
- `Use Module`: Use the given module for pointer path
- `Pointer Path`: The Address/Pointer path expression where the target value is located
- `Target Addr Range`: Range the pointer path's final address must be within. Ignored if the path only consists of a single address

### Integral View
- `Hex`: Whether to view the value as hex
- `Value (no label)`: The current value as either hex or decimal
- `Plot Range`: Minimum and Maximum values to be represented by the histogram
- `Progress Bar/Slider`: If the View is in read mode a Progress bar will be displayed. In Write more a Slider will be available for quick value adjustment

### Float View
- `Value (no label)`: Current value
- `Plot Range`: Minimum and Maximum values to be represented by the histogram
- `Progress Bar/Slider`: If the View is in read mode a Progress bar will be displayed. In Write more a Slider will be available for quick value adjustment

### Bool View
- `Is Set`: Whether the current boolean value is set

### DIP Switch View
The top-left bit is the least significant bit of the 32-bit DIP. Each entry has a text field to describe what the flag represents

<a name="processinformation"></a>
## Process Information
Displays various information about the connected game/process

<a name="settings"></a>
## Settings
Define various settings and preferences.

### General Settings
- `Set Color`: Define each widget type's color
- `Documents Path`: Where MungPlex saves everything. It is recommended picking a place that is on an SSD for better performance since this tool may handle large files
- `UI-Scale`: Scale of the app. Change this if it looks off
- `Default Active Window`

### Search Settings
- `Case Sensitive by default`
- `Color Wheel by default`
- `Values are hex by default`
- `Cached Searches by default`

### Cheats Settings
- `Cheat List by default`
- `Default Interval`

<a name="dataconversion"></a>
## Data Conversion
A small conversion utility

<img src="MungPlex\resources\img\manual\MungPlex_DataConversion_01.png" height="440">

### Primitive Value Conversion
- Float <-> Hex Conversion: Convert Float/Double to Hex and vice versa
- Change Endianness: Swap the byte order of the selected integer type

### Color Conversion
- Convert RGB(A) to RGB(A)F, RGB565, RGB5A3 and vice versa

### Text Conversion
- Convert UTF-8 Text to UTF-16 Little Endian, UTF-16 Big Endian, UTF-32 Little Endian, UTF-32 Big Endian, Shift-Jis, Jis x 0201 Full Width, Jis X 0201 Half Width, ASCII, ISO-8859-1 - 16 and vice versa

<a name="compiling"></a>
## Compiling

Open the project in Visual Studio as "Open as local folder".

Using `vcpkg`, make sure to install the following libraries (`vcpkg install xxx:x64-windows`):
* `sol2` ([example project](https://github.com/BullyWiiPlaza/Sol2WithCMake))
* `glfw`
* `nlohmann-json`

To finally compile and launch `MungPlex`, select `MungPlex.exe` as startup item and build/debug as usual in Visual Studio.

<a name="todo"></a>
## TODO
- General
  - Migrate `imgui` to using `vcpkg` (e.g. install `imgui[glfw-binding]`)
  - Texture Streaming
  - Corruptor
  - Value watch and control
- Search 
  - Text Types: `JIS X 0208`, Pokemon game text encodings, `EUC-KR`, `EUC-JP`, `EUC-CN`, `EUC-TW`, `Big5`, `base64`, ...
  - Color types: `RGB332`, `RGB444`, `RGBA4444`, `RGB555`, `RGBA5551`, `IA8`, ...
  - Array types: float, double
- Pointer Search
  - Fix potential problems with argument list
- Connection
  - Mesen NES support
  - More Emulators: Visual Boy Advance, Fusion, Yabause, NullDC, EPSXE, PCSX2, RPCS3, Citra, TeknoParrot, ...
- Cheats
  - Syntax Highlighting, more OS functionalities


<a name="kiitos"></a>
## Special Thanks
- Lawn Meower: Idea, code, reverse engineering
- [BullyWiiPlaza](https://github.com/BullyWiiPlaza/): Code, Creation of Pointer Search Engine
- [Divengerss](https://www.youtube.com/channel/UCZDBXfuNiVXXb7Wbh_syiDw): Testing, reporting bugs
