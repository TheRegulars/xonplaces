# xonplaces

This is my fork of darkplaces game engine, main intent of this fork to make new engine
that could run xonotic and would be much more modern. This fork can't run any
other game that supported by darkplaces.

## Agenda

I don't really think that moving game to daemon/FTEqw is good option. In the same
time, I don't think that staying on darkplaces in it's current state is good
option either. In my opinion good option is creating fork of darkplaces that
would be intended for xonotic and will be part of xonotic project. Pushing
changes back to darkplaces often takes large amount of time even for simple
changes, also it has it's complications cause it should remain compatible with large
amount of other games including Quake 1. By creating own fork it becomes free from
additional burden of backward compatibility that takes decades and creates hub
that will allow accept changes much quicker.

## Steps

1. Code cleanup. By this I mean removing legacy stuff that isn't used by Xonotic
   and moving codebase to modern libraries. For example, moving image loading to
   sdl_image could bring support of newer image formats as well reduce large
   portions of code. Also, I think that engine should prefer using cross
   platform API's over not cross platform. They provide much more similar
   codepaths on different environments and because of this debugging/testing is
   much simpler and environment dependent bugs a less likely.

1. Splitting code into separate libraries. Once cleanup is done we should split code
   into different
   components/libraries. Each component should have it's own public API which
   should be documented. Also, it would be much easier to make unittests for
   different part of game engine. For example, it possible to create library for
   working of dp color strings, parsing bsp files, discovering game servers, etc.
   Also, these libraries potentially could be used for something different, for
   example server discovery library could be used to make website with list of
   xon servers, etc. Components could be implemented in different languages, for
   example it possible to use C or C++ or even Rust with C ABI.
