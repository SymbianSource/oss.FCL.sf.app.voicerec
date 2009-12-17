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
rem Description:	Checks voicerecorder code respects the naming
rem             	conventions for leaving functions.
rem

@echo off
leavescan ..\RecViewSrc\CVRButtonPanel.cpp
leavescan ..\RecViewSrc\CVRLabel.cpp
leavescan ..\RecViewSrc\CVRRecViewContainer.cpp
leavescan ..\RecViewSrc\CVRRecViewModel.cpp
leavescan ..\RecViewSrc\CVRRecView.cpp
leavescan ..\RecViewSrc\CVRMdaRecorder.cpp 
leavescan ..\RecViewSrc\CVRSystemEventHandler.cpp
leavescan ..\RecViewSrc\CVRMemo.cpp
leavescan ..\RecViewSrc\CVRStateInfoPanel.cpp
leavescan ..\RecViewSrc\TVRState.cpp
leavescan ..\UtilsSrc\VRUtils.cpp
leavescan ..\UtilsSrc\CVRFileRecognizer.cpp
leavescan ..\AppSrc\CVRApplication.cpp 
leavescan ..\AppSrc\CVRDocument.cpp 
leavescan ..\AppSrc\CVRAppUI.cpp
leavescan ..\AppSrc\CVRNewFileService.cpp
leavescan ..\AppSrc\CVRAppServer.cpp
leavescan ..\NewServiceSrc\CNewFileServiceClientImpl.cpp
leavescan ..\NewServiceSrc\CNewFileServiceDiscovery.cpp
leavescan ..\NewServiceSrc\RNewFileServiceClient.cpp
leavescan ..\NewServiceSrc\CNewFileServiceBase.cpp
