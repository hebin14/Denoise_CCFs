from logging import _srcfile
import os

#set the build environment
env = Environment(CC = '/opt/ohpc/pub/mpi/mvapich2-intel/2.3.1/bin/mpicc',
                  CXX = '/opt/ohpc/pub/mpi/mvapich2-intel/2.3.1/bin/mpicxx',
                  CCFLAGD = Split("-g -Wall"),
                  ENV = os.environ
                  )


# find the head file directors
headDirs = []
srcFiles = []
libDirs  = []
libFiles = []
headDirs.append("/home/bxh220006/software/mada/include")
headDirs.append("/opt/ohpc/pub/libs/gnu7/openblas/0.2.20/include")
headDirs.append("/home/bxh220006/software/fftw-3.3.10/include")
headDirs.append("/opt/ohpc/pub/libs/intel/mvapich2/hdf5/1.10.5/include")
headDirs.append("/home/bxh220006/software/asdf-library/include")
headDirs.append("./mylib")
headDirs.append("./similaritylib")
headDirs.append("./iolib")

# libfiles
libFiles = Split("m mpi rsf fftw3f openblas hdf5 asdf")

# libpath
libDirs.append("/opt/ohpc/pub/mpi/mvapich2-intel/2.3.1/lib")

libDirs.append("/home/bxh220006/software/mada/lib")
libDirs.append("/opt/ohpc/pub/libs/intel/mvapich2/hdf5/1.10.5/lib")
libDirs.append("/home/bxh220006/software/asdf-library/lib64")
libDirs.append("/home/bxh220006/software/fftw-3.3.10/lib")
# find all the c files for compile the current target
srcFiles=Split('Masdfio.c ./mylib/mylib.c ./mylib/alloc.c ./iolib/subio.c')
env.Program(target='asdfio',source=srcFiles,CPPPATH=headDirs,LIBPATH=libDirs,LIBS=libFiles)

srcFiles=Split('Masdf_check_waveform.c ./mylib/mylib.c ./mylib/alloc.c ./iolib/subio.c')
env.Program(target='asdfcheck',source=srcFiles,CPPPATH=headDirs,LIBPATH=libDirs,LIBS=libFiles)

srcFiles=Split('Mlocal_corr.c ./mylib/mylib.c ./mylib/alloc.c')
env.Program(target='lcorr',source=srcFiles,CPPPATH=headDirs,LIBPATH=libDirs,LIBS=libFiles)

srcFiles=Split('Mglob_corr.c ./mylib/mylib.c ./mylib/alloc.c')
env.Program(target='gcorr',source=srcFiles,CPPPATH=headDirs,LIBPATH=libDirs,LIBS=libFiles)

srcFiles=Split('Mstack_rms.c ./mylib/mylib.c ./mylib/alloc.c')
env.Program(target='stack_rms',source=srcFiles,CPPPATH=headDirs,LIBPATH=libDirs,LIBS=libFiles)

srcFiles=Split('Madd.c ./mylib/mylib.c ./mylib/alloc.c')
env.Program(target='myadd',source=srcFiles,CPPPATH=headDirs,LIBPATH=libDirs,LIBS=libFiles)

srcFiles=Split('Mthreshold.c  ./mylib/mylib.c ./mylib/alloc.c')
env.Program(target='mythreshold',source=srcFiles,CPPPATH=headDirs,LIBPATH=libDirs,LIBS=libFiles)

# find all the c files for compile the current target
srcFiles=Split('Msimilarity.c ./mylib/mylib.c ./mylib/alloc.c \
        ./similaritylib/conjgrad.c ./similaritylib/divn2.c \
        ./similaritylib/triangle.c ./similaritylib/weight2.c \
         ./similaritylib/adjnull.c   ')
env.Program(target='similarity',source=srcFiles,CPPPATH=headDirs,LIBPATH=libDirs,LIBS=libFiles)

srcFiles=Split('Mstack.c ./mylib/mylib.c ./mylib/alloc.c')
env.Program(target='lstack',source=srcFiles,CPPPATH=headDirs,LIBPATH=libDirs,LIBS=libFiles)

srcFiles=Split('Msnr.c ./mylib/mylib.c ./mylib/alloc.c')
env.Program(target='mysnr',source=srcFiles,CPPPATH=headDirs,LIBPATH=libDirs,LIBS=libFiles)
