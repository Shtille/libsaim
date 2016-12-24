libsaim
======

Downloads, stores and renders tiles from any web tiling service.

![earth](http://planetmaker.wthr.us/img/earth_bluemarble_land_ocean_ice_texture_512x256.jpg)

`libsaim` is a small library written in C with few dependencies. It downloads tiles via `libcurl` and places images into efficient and fast local storage (up to 2Gb). Then on any request it gets image from the storage, decompresses it via `libjpeg` or `libpng`, puts into RAM and renders into target buffer very fast.
Really comes in handy when you're going to create some application with satellite imagery.
Tiling service should store JPEG or PNG images in [mercator projection](https://en.wikipedia.org/wiki/Mercator_projection).

How to get
------------

```
git clone https://github.com/Shtille/libsaim.git .
```

Building
--------

### Windows (MinGW makefile)
```
mingw32-make thirdparty
mingw32-make
```
To build via Visual Studio there is a project in `builds` directory.

### Mac OS X
```
make thirdparty
make
```
To build with Xcode or other IDE you will need to include all the sources in your project and add a define `SAIM_STATICLIB` if you're building static library.

### Other Unix platform
You should have `libcurl` installed into `/usr/local/lib` directory before calling to
```
make thirdparty
make
```
or just pass `CURL_PATH` and `CURL_LIB` to `make` command in manner described in the following section.

Dependencies
-------------------
`libsaim` depends on following libraries:
* libcurl
* libjpeg
* libpng
* zlib

All the libraries are included to `thirdparty` folder. But `libcurl` is excluded from build on all platforms but Windows.
Mac OS X provides libcurl with command line tools. So you dont even care about that on these two platforms.
For building on Linux or any other platform you should define path to your `libcurl` in environment variables or pass to `make` command indirectly:
```
make CURL_PATH=<path to curl library directory> CURL_LIB=<path to curl static/dynamic library>
```
All the dependencies are configurable, so i'll count them all:
* CURL_PATH
* CURL_LIB
* JPEG_PATH
* JPEG_LIB
* PNG_PATH
* PNG_LIB
* Z_PATH
* Z_LIB

Also you may specify other installation directory via passing `INSTALL_PATH` to `make` command.

Examples
--------

To use this library in your project just include `saim.h` file in `include` directory.
It provides few functions:
* `saim_init` - library initialization
* `saim_cleanup` - library deinitialization
* `saim_set_target` - setups target buffer sizes and bitness
* `saim_render_aligned` - renders rectangle with all sides parallel to geodetic axis
* `saim_render_common` - renders rectangle that is rotated clock wise

Also there's a working example in `/src/tests/app_c` directory.

If you feel like using another tile service you should copy formatted char string into `string` member of `saim_provider_info` argument.

This string should be something like: `http://my.imagery.server.com/tile/{z}/{y}/{x}`.
- Parameter `{z}` corresponds for a tile level (between `min_lod` and `max_lod`).
- Parameter `{x}` corresponds for a tile x coordinate (between `0` and `(1 << z)-1`).
- Parameter `{y}` corresponds for a tile y coordinate (between `0` and `(1 << z)-1`).

Contributing
------------

I'm open to any contributions that helps `libsaim`!  If you find bugs, please
create an issue or do a pull request :smile:.

License
-------

[zlib/libpng](https://opensource.org/licenses/zlib-license.php)

Copyright (c) 2016 Sviridov Vladimir
