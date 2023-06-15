# MungPlex

## Compiling

Open the project in Visual Studio as "Open as local folder".

Using `vcpkg`, make sure to install the following libraries (`vcpkg install xxx:x64-windows`):
* `sol2` ([example project](https://github.com/BullyWiiPlaza/Sol2WithCMake))
* `glfw`

TODOs:
* Migrate `imgui` to using `vcpkg` (e.g. install `imgui[glfw-binding]`)
* Use e.g. `nlohmann-json` instead of `meojson`
* Fix file path for `bool fontLoaded = io.Fonts->AddFontFromFileTTF("F:\\Workspace\\MungPlex\\MungPlex\\resources\\NotoSansJP-Black.ttf", 30, &cfg, io.Fonts->GetGlyphRangesJapanese());` to make it work out-of-the-box on any development machine

To finally compile and launch `MungPlex`, select `MungPlex.exe` as startup item and build/debug as usual in Visual Studio.