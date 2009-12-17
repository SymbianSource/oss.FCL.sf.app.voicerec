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
* Description:  Main settings view class
*
*/


// INCLUDE FILES
#include "CVRGSPlugin.h"
#include "CVRGSPluginContainer.h"

#include "VRConsts.h"
#include <voicerecorder.rsg>
#include "voicerecorder.hrh"

#include "VRUtils.h"

#include <f32file.h> // For icon generation
#include <gsvoicerecorderplugin.mbg> // For icon generation

#include <data_caging_path_literals.hrh>
#include <avkon.hrh>
#include <aknViewAppUi.h>
#include <akntitle.h>  // CAknTitlePane
#include <featmgr.h>
#include <StringLoader.h>     //for StringLoader
#include <gsfwviewuids.h>     //for KGSMainViewUid
#include <hlplch.h>             // For HlpLauncher
#include <bautils.h>            // for BaflUtils
#include <AknQueryDialog.h>

// ========================= MEMBER FUNCTIONS ================================

// ---------------------------------------------------------------------------
// CVRGSPlugin::CVRGSPlugin
// C++ constructor
// ---------------------------------------------------------------------------
//
CVRGSPlugin::CVRGSPlugin()
    : iResourceLoader( *iCoeEnv  )
    {
    
    }


// ---------------------------------------------------------------------------
// CVRGSPlugin::NewLC
// Symbian OS two-phased constructor
// ---------------------------------------------------------------------------
//
CVRGSPlugin* CVRGSPlugin::NewL( TAny* /*aInitParams*/ )
    {
    CVRGSPlugin* self = new(ELeave) CVRGSPlugin();
	CleanupStack::PushL(self);
	self->ConstructL();
	CleanupStack::Pop(self);
	return self;
    }


// ---------------------------------------------------------------------------
// CVRGSPlugin::ConstructL
// Symbian OS 2nd phase constructor
// ---------------------------------------------------------------------------
//
void CVRGSPlugin::ConstructL()
    {
#ifdef _DEBUG    
    RDebug::Print(_L("[CVRGSPlugin] ConstructL()" ));
    RDebug::Print( _L( "[CVRGSPlugin] Loading resource from :" ) );
    RDebug::Print( KVRAppResourceFile );
#endif
    
     // Find the resource file
    TParse parse;
    parse.Set( KVRAppResourceFile,    
               &KDC_APP_RESOURCE_DIR, 
               NULL );
    TFileName fileName( parse.FullName() );
    
    // Get language of resource file
    BaflUtils::NearestLanguageFile( iCoeEnv->FsSession(), fileName );

    // Open resource file
    iResourceLoader.OpenL( fileName );    
    FeatureManager::InitializeLibL();
    
    BaseConstructL( R_VOREC_GS_SETTING_LIST_VIEW );
    
    iCaption = StringLoader::LoadL( R_VOREC_GS_SETTINGS_TITLE );
    }


// Destructor
CVRGSPlugin::~CVRGSPlugin()
    {
    FeatureManager::UnInitializeLib();

    if( iContainer )
        {
        if ( iAppUi )
            {
            iAppUi->RemoveFromViewStack( *this, iContainer );
            }
        delete iContainer;
        iContainer=NULL;
        }  
        
    iResourceLoader.Close(); 
    
    delete iCaption; 
    }


// ---------------------------------------------------------------------------
// CVRGSPlugin::Id
// Return UID of view
// ---------------------------------------------------------------------------
//
TUid CVRGSPlugin::Id() const
    {
    return KGSVoiceRecorderGSPluginUid;
    }


// ---------------------------------------------------------------------------
// CVRGSPlugin::HandleCommandL
// Handle commands
// ---------------------------------------------------------------------------
//
void CVRGSPlugin::HandleCommandL( TInt aCommand )
    {
   
    switch ( aCommand )
        {
        case ECmdChange:
        case EAknCmdOpen:
            // Delegate to container
            Container()->HandleListBoxSelectionL( EAknCmdOpen );
            break;
        case ECmdMSK:
        	// Delegate to container
        	// Selection view is not open when command is ECmdMSK
            Container()->HandleListBoxSelectionL( ECmdMSK );
        	break;          
        case EAknSoftkeyBack:
        case ECmdCancel:   
            // Activate previous view
#ifdef _DEBUG                
    RDebug::Print(_L("VRGS: activating view 0x%x"), iPrevViewId.iViewUid.iUid);                            
#endif
            iAppUi->ActivateLocalViewL( iPrevViewId.iViewUid );
            break; 
        case EAknCmdHelp:
            if( FeatureManager::FeatureSupported( KFeatureIdHelp ) )
                {
                HlpLauncher::LaunchHelpApplicationL(
                    iEikonEnv->WsSession(), iAppUi->AppHelpContextL() );
                }
            break;           
        case ECmdOk: // Exit selected from options menu
            {
            iAppUi->HandleCommandL( EEikCmdExit ); 
            break;
            }
        default:
            iAppUi->HandleCommandL( aCommand );
            break;
        } 
    }
    

// ---------------------------------------------------------------------------
// CVRGSPlugin::DoActivate
// Activate this view
// ---------------------------------------------------------------------------
//
    
void CVRGSPlugin::DoActivateL( const TVwsViewId& aPrevViewId,
                                  TUid aCustomMessageId,
                                  const TDesC8& aCustomMessage )
    {       
    CGSBaseView::DoActivateL( aPrevViewId, aCustomMessageId, aCustomMessage );
    }


// ---------------------------------------------------------------------------
// CVRGSPlugin::DoDeactivate
// Deactivate this view
// ---------------------------------------------------------------------------
//
void CVRGSPlugin::DoDeactivate()
    {
    CGSBaseView::DoDeactivate();
    }
    

// -----------------------------------------------------------------------------
// CVRGSPlugin::HandleClientRectChange
// Handle screen size change.
// -----------------------------------------------------------------------------
//
void CVRGSPlugin::HandleClientRectChange()
    {
    if ( iContainer )
        {
        iContainer->SetRect( ClientRect() );
        } 
    }


// ---------------------------------------------------------------------------
// CVRGSPlugin::Container
// Return handle to container class.
// ---------------------------------------------------------------------------
//
CVRGSPluginContainer* CVRGSPlugin::Container()
    {
    return static_cast<CVRGSPluginContainer*>( iContainer );
    }
    
    
// ---------------------------------------------------------------------------
// CVRGSPlugin::HandleListBoxSelectionL
// Handle any user actions while in the list view.
// ---------------------------------------------------------------------------
//
void CVRGSPlugin::HandleListBoxSelectionL() 
    {
    // Delegate to container
    Container()->HandleListBoxSelectionL( ECmdChange );
    }


// ---------------------------------------------------------------------------
// CVRGSPlugin::GetCaptionL
// Caption text for view to be displayed in NaviPane.
// ---------------------------------------------------------------------------
//
void CVRGSPlugin::GetCaptionL( TDes& aCaption ) const
    {
    aCaption.Copy( *iCaption );
    }
    
    
// ---------------------------------------------------------------------------
// CVRGSPlugin::NewContainerL
// Creates new iContainer.
// ---------------------------------------------------------------------------
//
void CVRGSPlugin::NewContainerL()
    {
    iContainer = new( ELeave ) CVRGSPluginContainer;
    }


// ---------------------------------------------------------------------------
// CVRGSPlugin::CreateIconL
// Return the icon, if has one.
// ---------------------------------------------------------------------------
//
CGulIcon* CVRGSPlugin::CreateIconL( const TUid /*aIconType*/ )
    {

	// New implementation
	TParse* fp = new(ELeave) TParse();
    CleanupStack::PushL( fp );
    TInt err = fp->Set( KVRPluginFile, &KDC_APP_BITMAP_DIR, NULL ); 

    if ( err != KErrNone )
        { 
        User::Leave( err );
        }

    TBuf<KMaxFileName> fileName = fp->FullName();
    CleanupStack::PopAndDestroy();  //fp

    CGulIcon* icon;
    
    icon = AknsUtils::CreateGulIconL(
        AknsUtils::SkinInstance(), 
        KAknsIIDQgnPropSetAppsVoirec,
        fileName,
        EMbmGsvoicerecorderpluginQgn_prop_set_apps_voirec,
		EMbmGsvoicerecorderpluginQgn_prop_set_apps_voirec_mask );
    
    return icon;

    }
    
   
// ---------------------------------------------------------------------------
// CVRGSPlugin::Visible()
// Method for checking, if plugin should be visible and used in GS.
// ---------------------------------------------------------------------------
//    
TBool CVRGSPlugin::Visible() const
    {
    if ( VRUtils::FeatureEnabled( EVRFeatureShowQualitySetting ) ||
         FeatureManager::FeatureSupported( KFeatureIdMmc ) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }

    }
    
    
// ---------------------------------------------------------------------------
// CVRGSPlugin::CustomOperationL()
// Reserved for later use.
// ---------------------------------------------------------------------------
//        
TAny* CVRGSPlugin::CustomOperationL( TAny* aParam1, TAny* /*aParam2*/ )
    {
    return aParam1;    
    }


// ---------------------------------------------------------------------------
// CVRSettingsDialog::DynInitMenuPaneL
// 
// ---------------------------------------------------------------------------
//
void CVRGSPlugin::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane )
	{

	switch ( aResourceId )
		{
		case R_VOREC_GS_SETTINGS_MENUPANE:
			{
			// Check if HELP is enabled in FeatureManager, if not, disable the Help menu item
			if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
				{
				aMenuPane->SetItemDimmed(EAknCmdHelp, ETrue);
				}
            break; 
			}
		case R_VR_SETTINGS_MENUPANE:
			{
			// Check if HELP is enabled in FeatureManager, if not, disable the Help menu item
			if (!FeatureManager::FeatureSupported(KFeatureIdHelp))
				{
				aMenuPane->SetItemDimmed(ECmdHelp, ETrue);
				}
            break; 
			}


		default:
			{
			break;
			}
		}
	}   

// End of file


