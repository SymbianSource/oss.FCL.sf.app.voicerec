/*
* Copyright (c) 2002 - 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Class that observer remote controller framework events
*                and forwards them as key events to observe
*
*/


// INCLUDE FILES

#include <w32std.h>
#include <remconcoreapitarget.h>
#include <remconinterfaceselector.h>
#include <AknDef.h>
#include <aknconsts.h>

#include "MVRVolumeEventObserver.h"
#include "CVRRemConObserver.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// CVRRemConObserver::CVRRemConObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CVRRemConObserver::CVRRemConObserver( MVRVolumeEventObserver* aObserver )
	: iObserver( aObserver )
    {
    }

// ----------------------------------------------------------------------------
// CVRRemConObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// ----------------------------------------------------------------------------
//
void CVRRemConObserver::ConstructL()
    {
    iInterfaceSelector = CRemConInterfaceSelector::NewL();
    
    iCoreTarget = CRemConCoreApiTarget::NewL( *iInterfaceSelector, *this );
    
    iInterfaceSelector->OpenTargetL();
   
    iVolumeRepeatTimer = CPeriodic::NewL( CActive::EPriorityHigh );       
    }

// ----------------------------------------------------------------------------
// CVRRemConObserver::NewL
// Two-phased constructor.
// ----------------------------------------------------------------------------
//
CVRRemConObserver* CVRRemConObserver::NewL( MVRVolumeEventObserver* aObserver )
    {
    CVRRemConObserver* self = new( ELeave ) CVRRemConObserver( aObserver );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// ----------------------------------------------------------------------------
// CVRRemConObserver::~CVRRemConObserver
// Destructor
// ----------------------------------------------------------------------------
//    
CVRRemConObserver::~CVRRemConObserver()
    {
    delete iInterfaceSelector; // it internally deletes iCoreTarget
    
    if( iVolumeRepeatTimer->IsActive() ) 
        {
        iVolumeRepeatTimer->Cancel(); 
        }
    delete iVolumeRepeatTimer;    
    }


// ----------------------------------------------------------------------------
// CVRRemConObserver::MrccatoCommand
// 
// ----------------------------------------------------------------------------
//  
EXPORT_C void CVRRemConObserver::MrccatoCommand( 
										TRemConCoreApiOperationId aOperationId, 
										TRemConCoreApiButtonAction aButtonAct )
	{
	TKeyEvent myEvent;

    if( iVolumeRepeatTimer->IsActive())  
        {
        iVolumeRepeatTimer->Cancel();
        }	

	switch( aOperationId )	
		{
		case ERemConCoreApiVolumeUp:
			{
			// Simulate a "right button event" for CAknVolumeControl
			myEvent.iCode = EKeyRightArrow;
			break;
			}
		case ERemConCoreApiVolumeDown:
			{
			// Simulate a "left button event" for CAknVolumeControl
			myEvent.iCode = EKeyLeftArrow;
			break;
			}
		default:
			{
			// Ignore other commands
			return;
			}
		}

	// Convert the remote control event to a button click and send it to 
	// volume control
	switch( aButtonAct )
		{
		case ERemConCoreApiButtonClick:
			{
			TRAP_IGNORE( iObserver->HandleVolumeChangeL( myEvent, 
														 EEventKey ) );
			break;
			}
		case ERemConCoreApiButtonPress:
			{
			// Start repeat
            iVolumeChange = myEvent;
            iVolumeRepeatTimer->Cancel();
            iVolumeRepeatTimer->Start(
                KAknStandardKeyboardRepeatRate,
                KAknStandardKeyboardRepeatRate,
                TCallBack( DoHandleVolumeRepeatL, this )  );
			break;
			}
		case ERemConCoreApiButtonRelease:
			{
			iVolumeChange.iCode = 0;
			break;
			}
		default:
			{
			break;
			}
		}

	}


// ----------------------------------------------------------------------------
// CVRRemConObserver::DoHandleVolumeRepeatL
// 
// ----------------------------------------------------------------------------
//  
TInt CVRRemConObserver::DoHandleVolumeRepeatL( TAny* aAny )
    {
  	// Repeat last keypress
    CVRRemConObserver* self = reinterpret_cast< CVRRemConObserver* >( aAny );
	if ( self->iObserver->HandleVolumeChangeL( self->iVolumeChange, EEventKey )
		!= KErrNone )
		{
		self->iVolumeChange.iCode = 0;
		self->iVolumeRepeatTimer->Cancel();
		}
    return KErrNone;
    }
    
//  End of File  
