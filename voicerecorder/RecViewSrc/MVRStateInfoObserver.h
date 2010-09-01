/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* This class can be used to observe changes that has been happened in
* CVRStateInfoPanel class 
*
*
*/


#ifndef __MVRSTATEINFOOBSERVER_H__
#define __MVRSTATEINFOOBSERVER_H__

// CLASS DEFINITION
/**
* This class can be used to observe changes that has been happened in
* CVRStateInfoPanel class 
*/
class MVRStateInfoObserver 
	{
	public: // new methods

		/**
		* Returns the current progress bar position value
		* @return Position value
		*/
		virtual TInt ProgressBarPosition() const = 0;
		
	};

#endif // __MVRSTATEINFOOBSERVER_H__

// End of File
