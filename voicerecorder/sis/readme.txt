***************************************
* Steps to create Voice Recorder SISX
***************************************

************ Voice Recorder has to be upgraded as "full upgrade" package
************ The DLLs and the resource file will always be upgraded together


1. Upgrade the DLLs version number (refer to: Upgrade dll version number)
2. Build the VoiceRecorder sisx file
3. Build the cenrep sisx file


**************************************
*  Cenrep
**************************************
1. Build centrepconv.exe (refer to IAD wiki)

2. Create the cenrep.txt file using the "cenrep Configuration Tool"
Refer to:
K:\s60\tools\toolsextensions\ConfigurationTool

3. Copy the cenrep .txt file into c:\cenrep\

4. Create the cenrep .cre file:
K:\epoc32\release\winscw\udeb>centrepconv.exe  -nowait  -o c:\cenrep\101F87A3.cre c:\cenrep\101F87A3.txt

5. To create the cenrep sis file:
- copy "101F87A3.cre" into the "\voicerec\VoiceRecorder\sis" directory, then run the "build_cenrep_sisx.bat"


**************************************
* Upgrade dll version number
**************************************
1. Run "change_version.bat" script from \voicerec\VoiceRecorder\sis
