Galaxy
======

Galaxy is a C++, SDL based Space Invaders clone.

Features:
- Random generated levels.
- 5 different types of enemies, including chase kamikazes.
- 3 different interchangeable weapons.
- Several power-up objects.
- Top 10 record.
- Scenes recording.
- Per level stats.


![Galaxy](https://raw.github.com/carlos-jenkins/galaxy/master/media/screenshot.png "Galaxy")


Download
========

- [Microsoft Windows](https://launchpad.net/galaxy-gpl/trunk/1.0/+download/GalaxySetup.exe "Download Galaxy for MS Windows")
- [GNU/Linux](https://github.com/carlos-jenkins/galaxy/tarball/master "Download Galaxy for GNU/Linux")


How to build
============

Install dependencies:

```shell
sudo apt-get install libsdl1.2-dev libsdl-image1.2-dev libsdl-mixer1.2-dev libsdl-ttf2.0-dev
```

Then build and run:

```shell
cd download/dir
make
bin/galaxy
```

This package is developed with SDL (Simple Directmedia Layer):

- http://www.libsdl.org/


Galaxy can also be built in Windows XP, Vista and 7 using MinGW.

- http://www.mingw.org/


Pending
=======

- Sound efects.
- More music options.
- "About" menu.
- No hardcoded levels.
- Add mouse capturing option to the configuration file.
- Center game window when not playing fullscreen.
- Debian package.
- Create automated Windows script to create the installer.


Changelog
=========

2012-10-27 Version 1.1
- Moved to GitHub.
- Created a Makefile.
- Reestructured repository.
- Improved documentation.

2008-11-07 Version 1.0
- First public version.
- First Launchpad release.
- Creation of a changelog.
- Project script implemented.


License
=======

Copyright (C) 2008-2012 Carlos Jenkins <carlos@jenkins.co.cr>

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.

