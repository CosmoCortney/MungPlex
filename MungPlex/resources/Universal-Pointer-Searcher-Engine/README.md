## Universal Pointer Searcher

This is a severely reworked `C++` command line version of the [`Universal Pointer Searcher`](https://github.com/BullyWiiPlaza/Universal-Pointer-Searcher) which offers you a very flexible way of performing pointer searches which furthermore supports finding pointers on memory dumps of any system/console out there (and not just one specific system like all other pointer searchers I have seen).

## Requirements

### Windows

This application may require you to install the `Microsoft Visual C++ Redistributable for Visual Studio 2015, 2017, 2019 and 2022` from [here](https://support.microsoft.com/en-us/topic/the-latest-supported-visual-c-downloads-2647da03-1eea-4433-9aff-95f26a218cc0):

```
>dumpbin /dependents UniversalPointerSearcher.exe
Microsoft (R) COFF/PE Dumper Version 14.28.29336.0
Copyright (C) Microsoft Corporation.  All rights reserved.


Dump of file UniversalPointerSearcher.exe

File Type: EXECUTABLE IMAGE

  Image has the following dependencies:

    KERNEL32.dll
    USER32.dll
    bcrypt.dll
    WLDAP32.dll
    ADVAPI32.dll
    CRYPT32.dll
    WS2_32.dll
    WTSAPI32.dll
    KERNEL32.dll
    USER32.dll
    KERNEL32.dll
    USER32.dll
```

### Linux

The application may require you to install [`Intel TBB`](https://software.intel.com/content/www/us/en/develop/tools/oneapi/components/onetbb.html) via e.g. `sudo apt install libtbb-dev`. The application has the following dependencies:

```
$ ldd UniversalPointerSearcher.elf
    linux-vdso.so.1 (0x00007ffe2db9a000)
    libdl.so.2 => /lib/x86_64-linux-gnu/libdl.so.2 (0x00007f7c34edf000)
    libpthread.so.0 => /lib/x86_64-linux-gnu/libpthread.so.0 (0x00007f7c34ebc000)
    libstdc++.so.6 => /usr/lib/x86_64-linux-gnu/libstdc++.so.6 (0x00007f7c34cdb000)
    libm.so.6 => /lib/x86_64-linux-gnu/libm.so.6 (0x00007f7c34b8c000)
    libgcc_s.so.1 => /lib/x86_64-linux-gnu/libgcc_s.so.1 (0x00007f7c34b71000)
    libc.so.6 => /lib/x86_64-linux-gnu/libc.so.6 (0x00007f7c3497f000)
    libtbb.so.2 => /usr/lib/x86_64-linux-gnu/libtbb.so.2 (0x00007f7c34937000)
    /lib64/ld-linux-x86-64.so.2 (0x00007f7c365f6000)
```

## Files

[The release](https://github.com/BullyWiiPlaza/Universal-Pointer-Searcher-Engine/releases) contains the **README.md**, the **LICENSE**, the compiled **UniversalPointerSearcher.exe** (**Windows** version), the **DLLs** for the **Windows** version, the **UniversalpointerSearcher.macho** (**MacOS X** version) and the compiled **UniversalPointerSearcher.elf** (**Linux** version).

## Usage

This is a command line application. You have to pass the respective information such as source memory dump file path and starting address to the application via your system's command line (e.g. `cmd.exe`). Alternatively, there are more optional options to configure the pointer searcher. You can list them by passing the `--help` flag:

```
>UniversalPointerSearcher.exe --help
2022-08-28 17:37:23.122 (   0.000s) [        D9350E37]               main.cpp:40    INFO| Windows C++ Universal Pointer Searcher Engine v3.7
Release Build
Copyright (c) 2018 - 2022 BullyWiiPlaza Productions
2022-08-28 17:37:23.132 (   0.009s) [        D9350E37]   ProgramValidator.cpp:106   INFO| Validating application...
2022-08-28 17:37:23.134 (   0.012s) [        D9350E37]   ProgramValidator.cpp:61    INFO| Validating environment...
2022-08-28 17:37:23.138 (   0.016s) [        D9350E37]   ProgramValidator.cpp:113   INFO| Retrieving latest version details...
2022-08-28 17:37:23.936 (   0.814s) [        D9350E37]   ProgramValidator.cpp:140   INFO| Verifying binary...
2022-08-28 17:37:23.989 (   0.867s) [        D9350E37]   ProgramValidator.cpp:166   INFO| Application validation passed...
Usage: UniversalPointerSearcher.exe [OPTIONS]

Options:
  -h,--help                   Print this help message and exit
  --verbose BOOLEAN           Whether to perform verbose logging of the pointer searcher output
  --working-directory TEXT    The directory to use as a relative base for resolving file paths
  --initial-file-path TEXT ...
                              The file path to a memory snapshot file to add to the initial input or a folder to parse
  --input-memory-pointers-file-path TEXT
                              The file path memory pointers will be loaded and validated from on all added memory snapshots
  --store-memory-pointers-file-path TEXT
                              The file path to store all memory pointer results to
  --file-extensions TEXT ...  The file extension(s) of the memory snapshot file(s) to add from a folder (requires file names representing the hexadecimal starting address of the memory snapshot file respectively)
  --initial-starting-address TEXT ...
                              The starting address of each memory snapshot file
  --target-address TEXT ...   The target address of the memory snapshot(s)
  --endian TEXT               All memory snapshot file's endian (= byte order)
  --address-size UINT         Every memory snapshot file's address size (in bytes)
  --initial-pointer-address-range TEXT ...
                              The memory snapshot's pointer address range (useful for speeding up slow pointer map parsing). Each range consists of a positive numeric start value, a comma and a positive numeric end value (inclusive).
  --comparison-file-path TEXT ...
                              The file path to a memory snapshot file to add as comparison or a folder to parse
  --comparison-starting-address TEXT ...
                              The starting address of each comparison memory snapshot file
  --scan-deeper-by INT        Specifies how many additional levels of depth the pointer searcher should add to the provided memory pointers, by default 0.
  --write-pointer-maps-file-paths TEXT ...
                              The file paths to write the generated pointer maps to
  --compress-pointer-maps BOOLEAN ...
                              Whether to compress the respective written pointer map (default = true)
  --read-pointer-maps-file-paths TEXT ...
                              The file paths to read the pointer maps from
  --pointer-offset-range TEXT The pointer offset range to use for the pointer search. Each range consists of a potentially negative start value, a comma and a potentially negative end value (inclusive)
  --exclude-cycles BOOLEAN    Whether cycles are excluded in the resulting memory pointers list (reduces redundant results)
  --pointer-depth-range TEXT  The pointer depth range for the pointer search (may yield a lot more results). Each range consists of a positive start value, a comma and a positive end value (inclusive)
  --maximum-pointers-count INT
                              The pointer count where the pointer searcher will stop searching for more
  --last-pointer-offsets TEXT The enforced last pointer offsets of all memory pointers for the pointer search
  --pointer-sorting-type Memory pointer sorting type in {no sorting = 0, by address = 1, by absolute offset sum and by address = 2, by offset count, by absolute offset sum and by address = 3}:{0,1,2,3}
                              The memory pointer sorting type to use
  --maximum-pointers-printed-count UINT
                              The maximum amount of pointers printed by the pointer searcher (useful for performance reasons)
  --print-visited-addresses   Whether to print the visited addresses for each discovered memory pointer
  --print-module-file-names   Whether the base addresses of memory pointers will be expressed with the module file's file name
  --maximum-memory-utilization-fraction FLOAT:FLOAT in [0 - 1]
                              Whether to stop collecting memory pointers when the RAM is full at least the provided percentage to avoid out of memory crashes
```

If you do not pass certain command line options, meaningful defaults will be chosen for you automatically which may or may not be what you wanted.

Command line parsing works via `CLI11` so you may want to consult the documentation [here](https://cliutils.github.io/CLI11/book/) if you have any questions on how to pass certain information to the application.

Some pointer searcher demonstration runs can be seen [here](https://www.youtube.com/watch?v=FPbmaZVXkDw) or for learning in the section below.

**Note:**

Some of the command line switches may be renamed or changed since the creation of the video so always consult the `--help` documentation when in doubt.

### Examples

The following command line options can be used to perform different use cases. On Windows, you need to type `UniversalPointerSearcher.exe` followed by the command line flags. On Linux you need to type `./UniversalPointerSearcher` instead.

* Performing a pointer search using 1 initial memory dump and 3 comparison memory dumps:

  ```
  --initial-file-path D:\Cpp\PointerSearcher\test_memory_dumps\mario_kart_8\39CEB148.bin --initial-starting-address 0x30000000 --address-size 4 --comparison-file-path D:\Cpp\PointerSearcher\test_memory_dumps\mario_kart_8\39CF14D8.bin %% D:\Cpp\PointerSearcher\test_memory_dumps\mario_kart_8\39CF14D8.bin %% D:\Cpp\PointerSearcher\test_memory_dumps\mario_kart_8\39CF14D8.bin --comparison-starting-address 0x30000000 %% 0x30000000 %% 0x30000000 --endian big --target-address 39CEB148 39CF14D8 39CF14D8 39CF14D8
  ```

  Note that the `%%` signs separate the memory snapshot file groups respectively. If you don't use them, the pointer searcher will assume you mean 1 memory snapshot with 3 memory snapshot files which would probably yield an error.

* Validating existing pointers against an initial memory dump and a comparison memory dump, also find pointers with (an) additional depth level(s) and write them to a file:

  ```
  --input-memory-pointers-file-path D:\Cpp\PointerSearcher\test_memory_dumps\mario_kart_8\MemoryPointers.txt --initial-file-path D:\Cpp\PointerSearcher\test_memory_dumps\mario_kart_8\39CF14D8.bin --comparison-file-path D:\Cpp\PointerSearcher\test_memory_dumps\mario_kart_8\39CF14D8.bin --comparison-starting-address 0x30000000 --target-address 0x39CF14D8 0x39CF14D8 --initial-starting-address 0x30000000 --address-size 4 --endian big --scan-deeper-by 2 --store-memory-pointers-file-path D:\Cpp\PointerSearcher\test_memory_dumps\mario_kart_8\MemoryPointersUpdated.txt
  ```

  Note that the `MemoryPointers.txt` is a text file containing memory pointers, one per line. An example of such a text file would look like the following:

  ```
  [0x86409AFC] + 0x12E4
  [0x80409AFC] + 0x14
  [[0x86409AFC] + 0x14] + 0x10
  ```

  You can always output all memory pointers found by the pointer searcher into a text file by using the `--store-memory-pointers-file-path` command line option. The file can then be re-used as input for deeper pointer depth scans via `--scan-deeper-by`.
  
* Performing a pointer search by parsing an entire folder:

  ```
  --initial-file-path D:\Cpp\PointerSearcher\test_memory_dumps\smash_switch_2\Dump1 --file-extensions bin --target-address 0x5FDF030420 --address-size 8 --endian little
  ```

  The preconditions for this to work are that all memory snapshot files/modules have to be named by the module name (optional), an underscore (`_`) and the starting address of that memory snapshot. For example: `Heap0000_5F3E000000.bin` or `5F3E000000.bin`. If you want to specify a different file extension, you may use the respective command line option to do so.

* Performing a pointer search with multiple memory snapshot files:

  ```
  --initial-file-path "D:\Cpp\PointerSearcher\test_memory_dumps\super_mario_galaxy_2\SB4E01_DUMP80.bin" "D:\Cpp\PointerSearcher\test_memory_dumps\super_mario_galaxy_2\SB4E01_DUMP90.bin" --initial-starting-address 0x80000000 0x90000000 --address-size 4 --target-address 0x81458EEC 0x81458EEC --comparison-file-path "D:\Cpp\PointerSearcher\test_memory_dumps\super_mario_galaxy_2\SB4E01_DUMP80.bin" "D:\Cpp\PointerSearcher\test_memory_dumps\super_mario_galaxy_2\SB4E01_DUMP90.bin" --comparison-starting-address 0x80000000 0x90000000 --endian big --pointer-offset-range 0x0,0x400 --pointer-depth-range 1,3 --maximum-pointers-printed-count 1000
  ```

* Writing (compressed) pointer maps:

  ```
  --initial-file-path "D:\Cpp\PointerSearcher\test_memory_dumps\super_mario_galaxy_2\SB4E01_DUMP80.bin" "D:\Cpp\PointerSearcher\test_memory_dumps\super_mario_galaxy_2\SB4E01_DUMP90.bin" --initial-starting-address 0x80000000 0x90000000 --address-size 4 --target-address 0x81458EEC --endian big --write-pointer-maps-file-paths "D:\Cpp\PointerSearcher\test_memory_dumps\super_mario_galaxy_2\PointerMap.bin"
  ```

* Reading/Using (compressed) pointer maps for a pointer search:

  ```
  --address-size 4 --target-address 0x81458EEC --read-pointer-maps-file-paths "D:\Cpp\PointerSearcher\test_memory_dumps\super_mario_galaxy_2\PointerMap.bin"
  ```

### Saving/Re-running Searches

Keep in mind that you can write batch/shell scripts in order to save your command line command for re-use later. More information can be found [here](https://bullywiihacks.forumotion.com/t6494-#32192).

## Glossary

For your better understanding of how this tool operates/is meant to be used, here are a few important terms.

* [**Memory pointer**](https://en.wikipedia.org/wiki/Pointer_(computer_programming))

  A memory pointer consists of a base address and a variable number of offsets. An example for a memory pointer may be: *[[[0x86409AFC] + 0x12E4] + 0x199C] + 0x1A30*

  Here, the base address is *0x86409AFC* and the offsets are *0x12E4*, *0x199C* and *0x1A30*.

  If you want to follow a memory pointer manually, you need to look up the base address in the memory, take the value stored there, add the 1st offset to that value, look up the value stored at the result, add the 2nd offset, look up the value stored at that result and so on until all offsets have been processed. The final result will then be the destination address. Hopefully, you landed at the right address. Since following pointers manually is tedious, this application offers ways of doing this automatically via its validation mode.

* **Memory dump**
  
  This is a file representing a direct copy of the RAM from a starting address to an end address. Typically it covers the whole memory range or a more relevant fraction of it.
  
* **Pointer search**
  
  This is the act of finding potentially viable memory pointers based on provided memory dumps.
  
* **Pointer validation**
  
  In this step, all existing memory pointers are checked against all provided comparison memory snapshots. This means that the pointers will be followed on each comparison memory snapshot and if they do not reach the given target address, the respective pointer will be excluded from the final results.
  
* **Address**
  
  In computing, an address is an absolute (hexadecimal) number which uniquely specifies a location in memory.
  
* **Offset**
  
  This is a (hexadecimal) number which is kind of a relative address. It can be added to an address to get somewhere else in memory. Furthermore, offsets in e.g. memory dumps start at 0 but the address at that location may not be 0 (see starting address).
  
* **Starting address**

  This is the address where you started dumping the memory from.
  
* **Target address**

  This is the address you're trying to reach in the pointer search. The amount of provided target addresses must be 1 for each memory snapshot.

* **Memory snapshot file**

  This is a memory dump which has a starting address (and a certain length). The end address is of course given by starting address + length.

* **Pointer address range**

  This is a range consisting of a starting and an end address. By specifying a range, you can exclude collecting address value pairs outside of that particular range.

* **Memory snapshot**

  This is a collection of multiple memory snapshot files which form a complete memory snapshot. On some systems there are different memory regions which have to be dumped specifically, e.g. [`MEM80` and `MEM90`](https://www.youtube.com/watch?v=90NM58D2xYc) on the `Nintendo Wii`. On other systems, there may only be 1 contiguous memory region to be dumped, hence the memory snapshot is the same as that file.

  Consequently, memory snapshot can only have 1 target address when performing a pointer search.

* **[Endian](https://en.wikipedia.org/wiki/Endianness)**

  This is the byte order of all memory snapshot files. The endian is fixed and given by the operating system you dumped the memory from. It can either be little or big endian.

* **Address size**

  Similar to the endian, the address size is fixed on the system you're dumping memory from. On 32-bit systems the address size is 32-bit, on 64-bit systems the address size is 64-bit and so on. When passing a value for the address size, the value has to be 1 (= 8 bit), 2 (= 16 bit), 4 (= 32 bit) or 8 (= 64 bit). Any other values are invalid.

* **Pointer map**

  This is a file containing the address value pairs retrieved from a memory snapshot which have been determined to be viable for a pointer search.

* **[Compressed](https://en.wikipedia.org/wiki/Data_compression) pointer map**

  Simply a pointer map which has furthermore been compressed with a compression algorithm to save disc space.

* **Pointer depth range**

  This is the range of depth levels to search pointers at which is equivalent to the amount of offsets the resulting memory pointers will have. A pointer depth of 3 will e.g. yield a memory pointer with 3 offsets.

  Note:
  Using a depth of e.g. 5 or higher could make your RAM usage "explode" due to many pointers being added to the list so be careful when you modify this setting.
  
* **Pointer offset range**

  This is the range of pointer offsets to consider valid. Any offset which happens to be outside of that range will cause the pointer result to be removed from the list of viable memory pointers. It is a good idea to keep the offset range reasonable low to avoid nonsensical pointers with huge offsets to be outputted.

* **Visited addresses**

  The visited addresses are the addresses which are visited when following a memory pointer. It is a chain of addresses and the amount of addresses is equal to the base address + the amount of pointer offsets. When excluding cycles, duplicated addresses in the visited addresses chain will cause the respective memory pointer to be excluded.

## Anti Virus Problems

If your anti virus marks the pointer searcher as a virus, it's a false positive. The program is protected for security reasons and might cause some anti virus vendors to get worried due to the added protection like packing. There is absolutely no malicious code.

## Bug Reports/Feature Requests

If you find a definite bug or a lacking feature, please create a **detailed** GitHub issue (e.g. with all input and output data as well as error messages if applicable) and I will try to help you out.

## Performance Tips

The more pointer depth you are asking for and the more viable pointers are found for the pointer search, the slower it will take. You can try the following measures to improve the performance:

* Reduce the (upper) pointer depth
* Reduce the pointer offset range size

The pointer searches with many small memory dump files being added as a unit (memory snapshots) could be sped up as well at the pointer map parsing stage by generating a pointer map. Using compression slows the loading down a bit due to decompression but it will save a lot of disk space.

## Contributing

If you appreciate this tool, [feel free to donate any amount of money to me on PayPal](https://www.paypal.me/bullywiiplaza) to support further development and updates.