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
*     This class is part of application framework.
*
*/


#ifndef __CVRAPPSERVER_H__
#define __CVRAPPSERVER_H__

// INCLUDES
#include <AknServerApp.h>
#include <CNewFileServiceBase.h>

// CONSTANTS

// CLASS DECLARATION
/**
* This class is part of new file service framework.
*/
class CVRAppServer
	: public CAknAppServer
	{

	public: // from CAknAppServer
		CApaAppServiceBase* CreateServiceL( TUid aServiceType ) const;
		
	public:
		MNewFileServiceObserver* NewFileServiceObserver() const;

	private:
		mutable MNewFileServiceObserver* iNewFileServiceObserver;
	};

#endif // __CVNEWFILESERVICE_H__

