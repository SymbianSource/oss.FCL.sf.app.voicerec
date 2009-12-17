rem
rem Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
rem All rights reserved.
rem This component and the accompanying materials are made available
rem under the terms of "Eclipse Public License v1.0"
rem which accompanies this distribution, and is available
rem at the URL "http://www.eclipse.org/legal/epl-v10.html".
rem
rem Initial Contributors:
rem Nokia Corporation - initial contribution.
rem
rem Contributors:
rem
rem Description:	A utility to change version of the binaries.
rem

elftran.exe -version 15.9 \epoc32\release\armv5\urel\VoiceRecorder.exe
elftran.exe -version 15.9 \epoc32\release\armv5\urel\VoiceRecorderUtils.dll
elftran.exe -version 15.9 \epoc32\release\armv5\urel\VoiceRecorderRecView.dll
elftran.exe -version 15.9 \epoc32\release\armv5\urel\NewService.dll
elftran.exe -version 15.9 \epoc32\release\armv5\urel\GSVoiceRecorderPlugin.dll

elftran.exe -version 15.9 \epoc32\release\armv5\udeb\VoiceRecorder.exe
elftran.exe -version 15.9 \epoc32\release\armv5\udeb\VoiceRecorderUtils.dll
elftran.exe -version 15.9 \epoc32\release\armv5\udeb\VoiceRecorderRecView.dll
elftran.exe -version 15.9 \epoc32\release\armv5\udeb\NewService.dll
elftran.exe -version 15.9 \epoc32\release\armv5\udeb\GSVoiceRecorderPlugin.dll