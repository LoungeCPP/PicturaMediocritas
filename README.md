# Pictūra Mediocritas [![TravisCI Build Status](https://travis-ci.org/LoungeCPP/PicturaMediocritas.svg?branch=master)](https://travis-ci.org/LoungeCPP/PicturaMediocritas) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/13nn4a1r52ubgg9d/branch/master?svg=true)](https://ci.appveyor.com/project/nabijaczleweli/picturamediocritas/branch/master) [![Licence](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSE)
Like aurea mediocritas, but with frames in a video instead.

Or, you know, get an average frame from a video.

## Usage

```
usage: pictura-mediocritas in-video [out-image]
```

```sh
$ pictura-mediocritas "video.mp4"
# Averaged frame in   "video.png"

$ pictura-mediocritas "video.mp4" "video-average.jpg"
```

## Installation

```sh
# apt install  libfreeimage-dev libavcodec-dev libavformat-dev libavutil-dev libswscale-dev doctest-dev
# or pacman -S mingw-w64-x86_64-ffmpeg mingw-w64-x86_64-doctest mingw-w64-x86_64-freeimage
$ git clone https://github.com/LoungeCPP/PicturaMediocritas
$ cd PicturaMediocritas
$ make -j
$ make install
```
