# libls

Replacement to the defunct [libuf](https://github.com/ikeydoherty/libuf) project, to provide helpful C APIs
to portable programs not looking for a heavy library to link to.
Primary target is our game engine.

A [Lispy Snake, Ltd.](https://lispysnake.com/) project.

Relicensed under `ZLib` license with permission from sole author at time of
fork, Ikey Doherty. This relicensing effort is designed to help authors
integrate into their existing projects.

![derghuns](https://raw.githubusercontent.com/lispysnake/libls/master/.github/derghuns.jpg)

**Warning**

This library is under heavy development and should be currently considered unstable.
All development work will be to support the new Lispy Snake 2D engine so APIs and
ABIs are unstable.

### Porting

Right now it is trivial to port from `libuf` to `libls`. Simply revendor `libls` and remove `libuf`.
Replace all `UF_` `uf_` `Uf` namespace usage with `LS_` `ls_` `Ls`. This will cover all previous
API, including the macros and `LsHashmap` type.

### TODO

 - [ ] Cleanup!
 
### Authors

Copyright © 2017-2018 Ikey Doherty

Copyright 2019 Lispy Snake, Ltd.

libuf is available under the terms of the ZLib license.
