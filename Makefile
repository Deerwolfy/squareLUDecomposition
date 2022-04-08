CL_EXE = e:/programs/VisualStudio2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe
LINK_EXE = e:/programs/VisualStudio2019/Community/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/link.exe

WIN_INCLUDE = /I'e:\Windows Kits\10\Include\10.0.19041.0\ucrt' \
	      /I'e:\programs\VisualStudio2019\Community\VC\Tools\MSVC\14.29.30133\include'

MPI_INCLUDE = /I'C:\Program Files (x86)\Microsoft SDKs\MPI\Include' \
	      /I'C:\Program Files (x86)\Microsoft SDKs\MPI\Include\x64'

WIN_LIB = /libpath:'e:\programs\VisualStudio2019\Community\VC\Tools\MSVC\14.29.30133\lib\x64' \
          /libpath:'e:\Windows Kits\10\Lib\10.0.19041.0\um\x64' \
	  /libpath:'e:\Windows Kits\10\Lib\10.0.19041.0\ucrt\x64'
MPI_LIB = /libpath:'C:\Program Files (x86)\Microsoft SDKs\MPI\Lib\x64'

CL = $(CL_EXE) $(WIN_INCLUDE) $(MPI_INCLUDE) /I. /c
LINK = $(LINK_EXE) /machine:x64 /dynamicbase 'msmpi.lib' $(WIN_LIB) $(MPI_LIB)

LU_Parallel : LU_Parallel.obj mat_func.obj
	$(LINK) /out:LU_Parallel.exe LU_Parallel.obj mat_func.obj

LU_Parallel.obj : LU_Parallel.c mat_func.h
	$(CL) LU_Parallel.c

LU_Parallel_PeqN : LU_Parallel_PeqN.obj mat_func.obj
	$(LINK) /out:LU_Parallel_PeqN.exe LU_Parallel_PeqN.obj mat_func.obj

LU_Parallel_PeqN.obj : LU_Parallel_PeqN.c mat_func.h
	$(CL) LU_Parallel_PeqN.c

LU_sequential : LU_sequential.obj mat_func.obj
	$(LINK_EXE) $(WIN_LIB) /machine:x64 /out:LU_sequential.exe LU_sequential.obj mat_func.obj

LU_sequential.obj : LU_sequential.c mat_func.h
	$(CL_EXE) $(WIN_INCLUDE) /I. /c LU_sequential.c

mat_func.obj : mat_func.h mat_func.c
	$(CL) mat_func.c

clean :
	rm *.obj *.exe
