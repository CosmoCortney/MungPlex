--This script is needed for MungPlex to find the game within Mesen's memory. 
--Save and run this script before attempting to connect to Mesen. To do this go to "Debug/Script Window/Open Script" and open this file.
--Click the little gear and change the selection of "When a script window is opened" to "Load the previous script" (needs to be done only once).
--Click the play button (right-pointing arrow) to execute. 
--Run this script every time you want to connect to Mesen

function setFlag()
  --Write flag into Work RAM
  emu.write(0xD0, 0x4D, 0)
  emu.write(0xD1, 0x75, 0)
  emu.write(0xD2, 0x6E, 0)
  emu.write(0xD3, 0x67, 0)
  emu.write(0xD4, 0x50, 0)
  emu.write(0xD5, 0x6C, 0)
  emu.write(0xD6, 0x65, 0)
  emu.write(0xD7, 0x78, 0)
  
  --Copy first 8 bytes of ROM next to RAM flag
  RAMoffset = 0xD8
  for i = 0x8000, 0x8007, 1 do
    ROMval = emu.read(i, 0, false)
    emu.write(RAMoffset, ROMval, 0)
    RAMoffset = RAMoffset + 1;
  end
end

emu.addEventCallback(setFlag, emu.eventType.endFrame);