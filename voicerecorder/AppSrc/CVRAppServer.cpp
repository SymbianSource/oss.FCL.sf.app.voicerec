/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
*     This class is part of new file service framework.
*
*/


#include	<NewFileService.hrh>
#include    <NewFileServiceDefs.h>
#include	"CVRNewFileService.h"
#include	"CVRAppServer.h"

CApaAppServiceBase* CVRAppServer::CreateServiceL( TUid aServiceType ) const
	{
	if ( aServiceType == TUid::Uid( NEWSERVICEUID ) )
		{
		CVRNewFileService* service = CVRNewFileService::NewL();
		iNewFileServiceObserver = service;
		return service;
		}
	else
		{
		return CAknAppServer::CreateServiceL(aServiceType);
		}
	}

MNewFileServiceObserver* CVRAppServer::NewFileServiceObserver() const
	{
	return iNewFileServiceObserver;
	}

// End of File
