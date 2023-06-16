# MungPlex

## Compiling

Open the project in Visual Studio as "Open as local folder".

Using `vcpkg`, make sure to install the following libraries (`vcpkg install xxx:x64-windows`):
* `sol2` ([example project](https://github.com/BullyWiiPlaza/Sol2WithCMake))
* `glfw`
* `nlohmann-json`

TODOs:
* Migrate `imgui` to using `vcpkg` (e.g. install `imgui[glfw-binding]`)

To finally compile and launch `MungPlex`, select `MungPlex.exe` as startup item and build/debug as usual in Visual Studio.