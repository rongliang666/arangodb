; arango-packer-template.nsi
;
; this script copies  the installation program
; for arango and the corresponding window runtime
; libraries which the arango installation program needs
;
; The installation program and the runtimes are
; copied to the TEMP directory and then the arango
; installer is started
;--------------------------------

; !include "Library.nsh"

; The name of the installer
Name ""@INSTALLERNAME@"

; The file to write
OutFile "@INSTALLERNAME@.exe"

; The default installation directory
!define APPNAME "Unpacker"
!define COMPANYNAME "Triagens"
InstallDir $TEMP\${COMPANYNAME}\${APPNAME}



; Request application privileges for Windows Vista
RequestExecutionLevel user

;--------------------------------

; Pages
Page instfiles

;--------------------------------

; The stuff to install
Section "" ;No components page, name is not important

<<<<<<< HEAD
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR

  ; examinates if installation program exists and it is running
  IfFileExists "$INSTDIR\@INSTALLERNAME@-internal.exe" 0 install_files
  Fileopen $0 "$INSTDIR\@INSTALLERNAME@-internal.exe" "w"
  IfErrors 0 install_files
    MessageBox MB_OK "@INSTALLERNAME@ is allready runing"
    Quit
 ; files are copied
 install_files:
  FileClose $0
  ; Put file there
=======

  ; Set output path to the installation directory.
  SetOutPath $INSTDIR
  ; Put file there
; !insertmacro InstallLib DLL SHARED NOREBOOT_PROTECTED "exdll.dll" $INSTDIR $TEMP
>>>>>>> 80b8d51a7309d0e40ab44d3032c81e0e81cb4412
	file "..\Installation\Windows\Plugins\SharedMemory\Plugins\@BITS@\msvcr120.dll"
	file "@INSTALLERNAME@-internal.exe"
        Rename "$INSTDIR\Installation\Windows\Plugins\SharedMemory\Plugins\@BITS@\msvcr120.dll" "$INSTDIR\msvcr120.dll"
        Rename "$INSTDIR\Build@BITS@\@INSTALLERNAME@-internal.exe" "$INSTDIR\@INSTALLERNAME@-internal.exe"
        Exec "$INSTDIR\@INSTALLERNAME@-internal.exe"
        Quit
  
SectionEnd ; end the section
