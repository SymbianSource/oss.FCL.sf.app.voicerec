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
rem Description:	Bat file for making mpxcenrep.sisx.
rem
Echo please copy key files to ROOT before build sis file
del voicerecordercenrep.sis voicerecordercenrep.sisx
makesis -v voicerecordercenrep.pkg
signsis voicerecordercenrep.sis voicerecordercenrep.sisx Nokia_RnDCert_02.der Nokia_RnDCert_02.key
//del voicerecordercenrep.sis