Orbita - Simulate and visualize orbits of solar system bodies such as planets, comets and asteroids.
==============

[![License](http://img.shields.io/:license-gpl3-blue.svg)](http://www.gnu.org/licenses/gpl-3.0.html)

# What is Orbita
Orbita is a Qt6/OpenGL based program that renders our solar system in 3D. Positions and orbits of planets, comets and
asteroids are calculated by means of [libnova library](http://libnova.sourceforge.net/).
The [Keplerian elements](https://en.wikipedia.org/wiki/Orbital_elements#Keplerian)
are taken from the [Minor Planet Center](https://minorplanetcenter.net/). For comets download the
latest file [CometEls.txt](https://www.minorplanetcenter.net/iau/MPCORB/CometEls.txt)
and for asteroids [MPCORB.DAT](https://www.minorplanetcenter.net/iau/MPCORB/MPCORB.DAT).

The [Keplerian elements](https://en.wikipedia.org/wiki/Orbital_elements#Keplerian) of files
[CometEls.txt](https://www.minorplanetcenter.net/iau/MPCORB/CometEls.txt) and
[MPCORB.DAT](https://www.minorplanetcenter.net/iau/MPCORB/MPCORB.DAT) are stored in a
[SQLite database](https://www.sqlite.org/index.html). This enables for instance querying
[Jupiter trojan asteroids](https://en.wikipedia.org/wiki/Jupiter_trojan)
```
SELECT * FROM asteroids where type = 9
```
and visualizing their positions:
![jupiter-trojans](http://www.stibor.net/orbita/screenshots/jupiter-trojans.png)
or any other [Keplerian elements](http://en.wikipedia.org/wiki/Orbital_elements#Keplerian)
SQL queries.

## Interactive Animations of Orbita
Visualize trajectory of comet [1I/ʻOumuamua](http://www.stibor.net/orbita/screenshots/oumuamua.gif)

Visualize [first 1957 listed asteroids](http://www.stibor.net/orbita/screenshots/asteroids-x-rotate.gif) in MPCORB.DAT and rotate around X-axis.

Visualize [first 4 listed asteroids](http://www.stibor.net/orbita/screenshots/asteroids-motion.gif) in MPCORB.DAT with orbits, date, distance to earth/sun and magnitude.

## Compile and Download
For compiling orbita [Qt6](https://www.qt.io/download-open-source) and [libnova library](http://libnova.sourceforge.net/)
are required. A binary executable for Windows 64 can be downloaded here: [orbita-0.1.1-win64.zip](http://www.stibor.net/orbita/windows/orbita-0.1.1-win64.zip)

## License
This project is licensed under the [GPL-3 license](http://www.gnu.org/licenses/gpl-3.0.html).
