fl2k_win
===========
This is a fork of http://git.osmocom.org/osmo-fl2k which compiles for Win64 under Microsoft Visual Studio (verified with VS 2015). See release section for pre-compiled binaries.

To compile it yourself, you first need to acquire pthreads-win32 and libusb and copy the relevant files (see below) in folders called "pthread" and "libusb" placed parallel to the fl2k_win folder. (I verified the VS15 compileability with the subdirectory "Pre-built.2" from ftp://sourceware.org/pub/pthreads-win32/pthreads-w32-2-9-1-release.zip and https://github.com/libusb/libusb/archive/v1.0.22.zip). 

The folder structure has to look like this

    fl2k_win
        include
        src
        vs15
        <and so on>
    
    pthread
        include
        lib
        <and so on>
	
    libusb
        libusb
        msvc
        <and so on>
    
Next you have to open libusb/msvc/libusb_2015.sln and compile libusb for Win64 (Release and Debug Build).
   
Then simply open fl2k_win/vs15/fl2k.sln in Visual Studio and select between debug/release build options.

Original readme from http://git.osmocom.org/osmo-fl2k:
===========
# osmo-fl2k
#### turns FL2000-based USB 3.0 to VGA adapters into low cost DACs

For more information see https://osmocom.org/projects/osmo-fl2k/wiki
