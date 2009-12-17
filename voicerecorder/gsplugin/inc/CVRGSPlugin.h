/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:	Main settings view class.
*
*/


#ifndef GS_VOICERECORDERGSPLUGIN_H
#define GS_VOICERECORDERGSPLUGIN_H

// Includes
#include <aknview.h>
#include <eikclb.h>
#include <ConeResLoader.h>
#include <gsplugininterface.h>
#include <gsbaseview.h>


// Classes referenced
class CVRGSPluginContainer;
class CAknViewAppUi;
class CGSBaseView;

// Constants

/** 
* UPDATE THIS:
*
* This UID is used for both the view UID and the ECOM plugin implementation 
* UID.
*/
const TUid KGSVoiceRecorderGSPluginUid = { 0x1020729D };

// CLASS DECLARATION

/**
*  CVRGSPlugin view class.
*
* This is an VoiceRecorderGS of the minimal GS plugin.CGSPluginInterface
*/
class CVRGSPlugin : public CGSBaseView
    {
    
    public: // Constructors and destructor
        
        /**
        * Symbian OS two-phased constructor
        * @return 
        * @since 3.1
        */
        static CVRGSPlugin* NewL( TAny* aInitParams );
            
        /**
        * Destructor.
        */
        ~CVRGSPlugin();       
        

    public: // Functions from base classes
        
        /**
        * From CAknView, returns the views id.
        * @return Returns id of the view.
        * @since 3.1
        */       
        TUid Id() const;
        
         /**
        * From MEikCommandObserver, handles the menu based commands.
        * @param aCommand identifies the given command.
        * @since 3.1
        */
        void HandleCommandL( TInt aCommand );
        
        /**
        * Updates specific value to container 
        * @param aListItem item index to update
        * @param aNewValue new value for the setting
        * @since 3.1
        */
        void UpdateListBoxL( const TInt& aListItem, TInt aNewValue );
        
     public: // From CGSPluginInterface 
     
        /**
        * Method for getting caption of this plugin. This should be the
        * localized name of the settings view to be shown in parent view.
        * @param aCaption pointer to Caption variable
        * @since 3.1
        */
        void GetCaptionL( TDes& aCaption ) const;
        
        /**
        * Creates a new icon of desired type. Override this to provide custom
        * icons. Othervise default icon is used. Ownership of the created icon
        * is transferred to the caller.
        *
        * Icon type UIDs (use these defined constants):
        * KGSIconTypeLbxItem  -   ListBox item icon.
        * KGSIconTypeTab      -   Tab icon.
        *
        * @param aIconType UID Icon type UID of the icon to be created.
        * @return Pointer of the icon. NOTE: Ownership of this icon is
        *         transferred to the caller.
        * @since 3.1
        */
        CGulIcon* CreateIconL( const TUid aIconType );
        
        /**
        * Method for checking, if plugin should be visible and used in GS FW.
        * (for example shown in listbox of the parent view).
        *
        * On default plugin is visible. Overwrite this function to enable or
        * disable your plugin dynamically.
        *
        * @return ETrue if plugin should be visible in GS.
        * @return EFalse if plugin should not be visible in GS.
        * @since 3.1
        */
        TBool Visible() const;
        
        /**
        * Reserved for future use/plugin's custom functionality. This can be
        * overwritten if plugin needs to have custom functionality which cannot
        * be fulfilled otherwise.
        * @since 3.1
        */
        TAny* CustomOperationL( TAny* aParam1, TAny* aParam2 );
        
        /**
        * From CEikAppUi
        * Handle skin change event.
        * @since 3.1
        */
        void HandleClientRectChange();
        
        /**
        * From CAknView Activate this view
        * @param aPrevViewId 
        * @param aCustomMessageId 
        * @param aCustomMessage 
        * @since 3.1
        */
        void DoActivateL( const TVwsViewId& aPrevViewId,
                          TUid aCustomMessageId,
                          const TDesC8& aCustomMessage );
                          
        /**
        * From CAknView Deactivate this view
        * @since 3.1
        */
        void DoDeactivate();
        
        /**
        * Get VoiceRecorderGSPlugin's container.
        * @since 3.1
        */
        CVRGSPluginContainer* Container();
        
    protected:

        /**
        * C++ default constructor.
        */
        CVRGSPlugin();

        /**
        * Symbian OS default constructor.
        */
        void ConstructL();
        
        /**
        * Handles OK key press.
        * @since 3.1
        */
        void HandleListBoxSelectionL();
        
        /**
        * Creates new container.
        * @since 3.1
        */
        void NewContainerL();
 
        void CVRGSPlugin::DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        
    protected: //Data        
       
        RConeResourceLoader iResourceLoader;
        
        // Buffered caption of the plugin. Owned
        HBufC* iCaption; 
        
    private:
    };

#endif // GS_VoiceRecorderGSPLUGIN_H
