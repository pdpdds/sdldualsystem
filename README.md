# sdldualsystem
SDL Game Programming Using Dual System.

## SDL 1.2 Game List
| Project | Game Name| Link |
| -------- | -------- | -------- |
|clonekeen |Clone Keen     |  http://clonekeen.sourceforge.net/    |
|lastmission |Last Mission     |  https://github.com/dmitrysmagin/last-mission    |
|bermuda |Bermuda Syndrome     |  https://github.com/cyxx/bermuda    |
|theclue |The Clue!     |  http://cosp.sourceforge.net    |
|savagewheels |Savage Wheels     |  https://kenamick.itch.io/savage-wheels    |
|rockbot |Rockbot     |  https://rockbot.upperland.net/    |
|xrick |Xrick     |  https://github.com/fabiensanglard/xrick    |
|tennix |tennix     |  http://icculus.org/tennix/    |
|xargon |xargon     |  https://github.com/Malvineous/xargon    |
|metapacman |metapacman     |  http://os4depot.net/index.php?function=showcontent&file=game/misc/metapacman.lha    |
|retrobattle |Retrobattle     | http://remar.se/andreas/retrobattle     |
|cannonball |Cannonball     | https://archive.org/details/cannonball-outrun     |
|kiloblaster |Kiloblaster     | https://github.com/Malvineous/kiloblaster     |
|wolf4sdl |Wolf4SDL     | http://wolf3d.net/wolf4sdl/     |
|stall | stall     | https://github.com/repu1sion/stall     |

## Build
* Visual Studio 2022 or Higher Needed
* vckpg

```
git clone https://github.com/microsoft/vcpkg
bootstrap-vcpkg.bat
vcpkg integrate install
vcpkg install sdl2
vcpkg install sdl2-image
vcpkg install sdl2-mixer
vcpkg install sdl2-ttf
vcpkg install sdl2-net
vcpkg install sdl2-gfx  
vcpkg install box2d
vcpkg install lua
vcpkg install libmikmod
vcpkg install libflac
vcpkg install smpeg2
vcpkg install libvorbis
vcpkg install libmodplug
vcpkg install curl
vcpkg install tiff
vcpkg install gettext
```

* open thirdprty/sdlcl.sln and build
* open SDL1_Project.sln and build



