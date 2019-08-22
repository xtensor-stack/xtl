# ![xtl](docs/source/xtl.svg)

[![Travis](https://travis-ci.org/QuantStack/xtl.svg?branch=master)](https://travis-ci.org/QuantStack/xtl)
[![Appveyor](https://ci.appveyor.com/api/projects/status/g9bldap2wirlue9w?svg=true)](https://ci.appveyor.com/project/QuantStack/xtl)
[![Azure](https://dev.azure.com/johanmabille/johanmabille/_apis/build/status/QuantStack.xtl?branchName=master)](https://dev.azure.com/johanmabille/johanmabille/_build/latest?definitionId=1&branchName=master)
[![Documentation Status](http://readthedocs.org/projects/xtl/badge/?version=latest)](https://xtl.readthedocs.io/en/latest/?badge=latest)
[![Join the Gitter Chat](https://badges.gitter.im/Join%20Chat.svg)](https://gitter.im/QuantStack/Lobby?utm_source=badge&utm_medium=badge&utm_campaign=pr-badge&utm_content=badge)

Basic tools (containers, algorithms) used by other quantstack packages

## Installation

### Package managers

If you are using Conan to manage your dependencies, merely add `xtl/x.y.z@omaralvarez/public-conan` to your requires, where x.y.z is the release version you want to use. Please file issues in [conan-xtl](https://github.com/omaralvarez/conan-xtl) if you experience problems with the packages. Sample `conanfile.txt`:

```
[requires]
xtl/0.6.5@omaralvarez/public-conan

[generators]
cmake
```

We also provide a package for the conda package manager:

```bash
conda install -c conda-forge xtl
```

### Install from sources

`xtl` is a header-only library.

You can directly install it from the sources:

```bash
cmake -D CMAKE_INSTALL_PREFIX=your_install_prefix
make install
```

## Documentation

To get started with using `xtl`, check out the full documentation

http://xtl.readthedocs.io/


## Building the HTML documentation

xtl's documentation is built with three tools

 - [doxygen](http://www.doxygen.org)
 - [sphinx](http://www.sphinx-doc.org)
 - [breathe](https://breathe.readthedocs.io)

While doxygen must be installed separately, you can install breathe by typing

```bash
pip install breathe
```

Breathe can also be installed with `conda`

```bash
conda install -c conda-forge breathe
```

Finally, build the documentation with

```bash
make html
```

from the `docs` subdirectory.

## License

We use a shared copyright model that enables all contributors to maintain the
copyright on their contributions.

This software is licensed under the BSD-3-Clause license. See the [LICENSE](LICENSE) file for details.
