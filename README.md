# Pictūra Mediocritas [![TravisCI Build Status](https://travis-ci.org/LoungeCPP/PicturaMediocritas.svg?branch=master)](https://travis-ci.org/LoungeCPP/PicturaMediocritas) [![AppVeyor Build Status](https://ci.appveyor.com/api/projects/status/13nn4a1r52ubgg9d/branch/master?svg=true)](https://ci.appveyor.com/project/nabijaczleweli/picturamediocritas/branch/master) [![Licence](https://img.shields.io/badge/license-MIT-blue.svg?style=flat)](LICENSE)
Like aurea mediocritas, but with frames in a video instead.

Or, you know, get an average frame from a video.

## [Manpage](https://cdn.rawgit.com/LoungeCPP/PicturaMediocritas/man/pictura-mediocritas.1.html)

## Examples

```sh
# TODO: concretify
$ pictura-mediocritas "video.mp4"
```

For details, see the [manpage](https://cdn.rawgit.com/LoungeCPP/PicturaMediocritas/man/pictura-mediocritas.1.html).

## Installation

Either acquire a binary release from [the releases page](https://github.com/LoungeCPP/PicturaMediocritas/releases),
or build it yourself:

```sh
$ git clone --recursive https://github.com/LoungeCPP/PicturaMediocritas
$ cd PicturaMediocritas
$ make
$ install out/pictura-mediocritas $(wherever)
```

## Contributing

Imperative commit messages, format your code with clang-format, you know the drill.

Branch names *should* be in the format `{issue}-{name}-{desc}`, where
`issue` is the issue # this branch closes,
`name` is your name (or a shorthand, like "nab" for "nabijaczleweli"), and
`desc` is a few-word description of the changes in the branch (like "rework-dir-listing", "second-time").

Non-FF PRs (ergo, merge commits) are illegal.
