@echo off

set CommonCompilerFlags=-MTd -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100   -wd4189 -wd4701 -wd4724 -DDEVELOPER=1 -FC -Z7
set CommonLinkerFlags= -incremental:no -opt:ref user32.lib gdi32.lib winmm.lib


del *.pdb > NUL 2> NUL
cl %CommonCompilerFlags% win32_platform.c -Fmwin32_platform.map /Fegame.exe /link %CommonLinkerFlags%


IF "%*" == "dbg" (
	devenv game.exe /r
	)
IF NOT "%*" == "dbg" (
	del *.pdb
	del *.obj
	del *.map
)