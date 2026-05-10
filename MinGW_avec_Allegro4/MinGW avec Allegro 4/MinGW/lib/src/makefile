# simple but easy to read makefile 
# you should install the dirextx headers before running this!

all : libd3dim.a libd3drm.a libd3dxof.a libddraw.a libdinput.a libdplayx.a \
libdsound.a libdxguid.a libdsetup.a
	@echo move the created libraries to the lib dir.
	@echo happy coding !!

libdxguid.a : dxguid.c
	gcc -c -Wall dxguid.c
	ar -r libdxguid.a dxguid.o
libd3dim.a : d3dim.def
	dlltool --dllname d3dim.dll --def d3dim.def --output-lib libd3dim.a -k
libd3drm.a : d3drm.def
	dlltool --dllname d3drm.dll --def d3drm.def --output-lib libd3drm.a -k
libd3dxof.a : d3dxof.def
	dlltool --dllname d3dxof.dll --def d3dxof.def --output-lib libd3dxof.a -k
libddraw.a : ddraw.def
	dlltool --dllname ddraw.dll --def ddraw.def --output-lib libddraw.a -k
libdinput.a : dinput.def
	dlltool --dllname dinput.dll --def dinput.def --output-lib libdinput.a -k
libdplayx.a : dplayx.def
	dlltool --dllname dplayx.dll --def dplayx.def --output-lib libdplayx.a -k
libdsound.a : dsound.def
	dlltool --dllname dsound.dll --def dsound.def --output-lib libdsound.a -k
libdsetup.a : dsetup.def
	dlltool --dllname dsetup.dll --def dsetup.def --output-lib libdsetup.a -k

