/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
*     CVRRecView is an CAknView inherited view class that implements
*     the recorder view functionality of Voicer Recorder.
*     It owns the view container (which, in turn, owns all the ui controls)
*     and the view model.
*
*/


#ifndef __CVRRECVIEW_H__
#define __CVRRECVIEW_H__

// INCLUDES
#include <aknview.h>
#include <VoiceRecorderContexts.h>
#include <voicerecorder.rsg>
#include <AknWaitDialog.h>

#include "MVRSelectionProvider.h"
#include "MVRLayoutChangeObserver.h"
#include "voicerecorder.hrh"
#include "VRUSBStateHanlder.h"

// FORWARD DECLARATIONS
class CAknLaunchAppService;
class CSendUi;
class CVRRecViewContainer;
class CVRRecViewActivationContainer;
class CVRRecViewModel;

enum TDialogTypeID {
        EDialogForWaitStorageCard = 0,
        EDialogForWaitUSBPluggingOut
    };

// CLASS DEFINITION
/**
 * CVRRecView is an CAknView inherited view class that implements
 * the recorder view functionality of Voicer Recorder.
 * It owns the view container (which, in turn, owns all the ui controls)
 * and the view model.
 */
class CVRRecView : public CAknView,
        public MVRSelectionProvider,
        public MVRLayoutChangeObserver,
        public MVRUSBStateObserver,
        public MProgressDialogCallback
    {
private:
    // nested classes

        /*
        * One shot class for model activation. Receives a pointer to
        * constructed CVRRecViewModel object and activates it asynchronically
        * when Activate() is called.
        */
        NONSHARABLE_CLASS( CVRRecViewModelActivator ) 
            : public CAsyncOneShot
            {
            public:  // constructor and destructor
                CVRRecViewModelActivator( CVRRecViewModel* aModel );
                ~CVRRecViewModelActivator(); 

            public: // New functions:

				/**
				 * Activate model in given context
                 */
                void Activate( TVRRecViewContexts aContext, TCallBack aCallback );
                
            private: // Functions from base classes

                /**
                 * From CActive.
                 */
                void RunL();
                void DoCancel();

            private: // Data:

                /*
                * State machine model, not owned
                */
                CVRRecViewModel* iModel;

				/*
				* Context that is used to activate model in RunL
				*/                
                TVRRecViewContexts iContext;
                
                /*
                * Callback that is called after model is activated
                */
                TCallBack iCallback;
            };
        
        /*
        * One shot class for note pop-up
        */
        NONSHARABLE_CLASS( CVRRecViewDialogActivator ) 
            : public CAsyncOneShot
            {
            public:  // constructor and destructor
                CVRRecViewDialogActivator( CVRRecViewModel* aModel);
                ~CVRRecViewDialogActivator(); 
                
            public: // new method
                void SetDialogType(TDialogTypeID aType);
                
                void SetViewContexts(TVRRecViewContexts aContext);
                
            private: // Functions from base classes

                /**
                 * From CActive.
                 */
                void RunL();
                void DoCancel();

            private: // Data:

                /*
                * State machine model, not owned
                */
                CVRRecViewModel* iModel;
                
                /*
                * The type of dialog
                */
                TDialogTypeID iType;
                
                /*
                * Context that is used to activate model
                */                
                TVRRecViewContexts iContext;  
            };
	
	
	public: // Constructors and destructor

		/**
		* Static constructor.
		* @param aViewResourceId The resource used in view construction.
		* @param aModelResourceId The resource used in model construction.
		*/
		IMPORT_C static CVRRecView* NewLC( TInt aViewResourceId,
										   TInt aModelResourceId );

		/**
		* Destructor
		*/
		~CVRRecView();

	private: // Constructors

		/**
		* Default constructor.
		* The inline saves a few bytes.
		*/
		inline CVRRecView() {};

		/**
		* 2nd phase constructor.
		* @param aViewResourceId The resource used in view construction.
		* @param aModelResourceId The resource used in model construction.
		*/
		void ConstructL( TInt aViewResourceId, TInt aModelResourceId );

	public: // from MVRSelectionProvider

		/**
		* Returns an array of currently selected files.
		* Ownership is transferred. Caller must destroy the returned array.
		* @param aStyle Used to return the style of selection
		*               (empty/focused/marked)
		* @return An array of selected files (0 or more).
		*/
		MDesCArray* GetSelectedFilesLC( TVRSelectionStyle& aStyle ) const;

		/**
		* Pass the file handle to recview.
		* Recorded memo is placed there.
		* @param aFile File handle
		*/
		void SetFileHandle( RFile& aFile );
		
		/**
		* Return the recorded open file handle
		* @param aFile File handle
		*/
		RFile& GetFile();		

	public: // from MVRLayoutChangeObserver

		/**
		* This method gets called by the AppUI when a dynamic layout change
		* event occurs.
		*/
		void LayoutChangedL( TInt aType );

	public: // from CAknView
		
		/**
		* Returns the id of the view.
		* @return KVRRecViewUID
		*/
		TUid Id() const;

		/**
        * Handles menu commands.
 		* @param aCommand Command identifier.
        */
		void HandleCommandL( TInt aCommandId );

	protected: // from CAknView

		/**
		* Dynamically initialises a menu pane. The Uikon framework calls this
		* function, immediately before the menu pane is activated.
		* @param aResourceId Resource ID identifying a menu pane to initialise
		* @param aMenuPane The in-memory representation of the menu pane.
		*/
		void DynInitMenuPaneL( TInt aResourceId, CEikMenuPane* aMenuPane );

        /** 
         * Foreground event handling function.
         *
         * Called by UI Framework when view switches to and from foreground
         *  
         * @param aForeground True if view is switched to foreground else false.
         */	
//		void HandleForegroundEventL( TBool aForeground );
		
	private: // from CAknView
		
        /**
 		* Is called by the framework when the view is activated.
		* @param aPrevViewId Id of the previous view that was active
		* @param aCustomMessageId Id of the custom message passed
		* @param aCustomMessage Descriptor containing the custom message data
        */
		void DoActivateL( const TVwsViewId& aPrevViewId,
						  TUid aCustomMessageId,
						  const TDesC8& aCustomMessage );

        /**
 		* Is called by the framework when the view is deactivated.
        */
		void DoDeactivate();

	protected: // from MCoeView

		/**
		* Is called by the view server when the application goes to background.
		*/
		void ViewDeactivated();

		/**
		* Is called by the view server when the application goes to the foreground.
		*/
		void ViewActivatedL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId,const TDesC8& aCustomMessage);

	public: // from CAknView

		/**
		* Returns a pointer to the CBA container
		* Calls base class Cba() (which is protected)
		*/
		CEikButtonGroupContainer* Cba() const;

    	/**
		* Returns a the StateId
		* 
		*/
        TUint GetStateId() const;

		/**
		* Returns embedding status.
		* @param ETrue if the current context is an embedded one.
		*/
		TBool IsEmbedded() const;

        /**
		* Returns the active quality setting.
		* @return Quality setting of the active memo
		*/
		TVRQuality Quality() const;

	private: // new methods

		/**
		* Dynamically adds a Send Via menu item plus associated cascade
		* to the given menu pane.
		* @param aResourceId Resource ID identifying a menu pane
		* @param aMenuPane The in-memory representation of the menu pane.
		*/
		void AddSendToMenuL( TInt aResourceId, CEikMenuPane* aMenuPane );

		/*
		* Trappable version of DoActivateL. Activates an empty UI container
		* and starts asynchronic model activation.
		* See CAknView::DoActivateL for parameters
		*/
		void ReallyDoActivateL( const TVwsViewId& aPrevViewId,
								  TUid aCustomMessageId,
								  const TDesC8& aCustomMessage );

    /*
     * Called by iModelActivator object after model has been
     * fully activated.
     * @param aRecView Always a valid pointer to CVRRecView object
     * @return Return value required by TCallBack, not used.
     */
    static TInt ActivationCallBack(TAny* aRecView);

    /*
     * Non-static method called by ActivationCallBack. Activates
     * the actual UI container and deletes the empty one.
     */
    void ActivateContainerL();    
    
    /*
     * Check the current state of drives
     */
    void CheckDriveState();    
    
    /**
    * This method show a dialog to warn user to insert the SD card.
    * @return if user insert the SD card or not
    */
    static TBool ShowDialogForWaitStorageCardL();
    
    /**
     * Make user insert the SD card, and choose SD card as the memo storage
     */
    void SetDriveL();
   
public:
    virtual TInt HandleUsbPlugInL();
    virtual TInt HandleUsbPlugOutL();
    
    virtual void DialogDismissedL( TInt aButtonId );

private:
    // data

    /**
     * Pointer to the UI control container. Owned.
     */
    CVRRecViewContainer* iContainer;

    /**
     * Pointer to the container that is displayed during
     * model construction. Owned
     */
    CVRRecViewActivationContainer* iActivationContainer;

		/**
		* Pointer to the view model (statemachine). Owned.
		*/
		CVRRecViewModel* iModel;

		/**
		* SendUi used to implement the Send Via feature. Owned.
		*/
		CSendUi* iSendUi;			
		
		/**
		* One shot object that is used to activate model in correct context
		* asynchronically. Owned.
		*/
		CVRRecViewModelActivator* iModelActivator;
		
		/**
        * One shot object that is used to pop up the note
        */
		CVRRecViewDialogActivator* iDialogActivator;

    /**
     * The type of dialog
     */
	TDialogTypeID iDialogType;
		
    /**
     * The command that passed to Handlecommand
     * 
     */
    TInt iCommand;

    CAknLaunchAppService* iLaunchService;
    
    CAknWaitDialog* iUsbWaitDialog;
    
    CVRUSBStateHanlder* iUSBStateHandler;
    
    /*
    * Context that is used to activate model
    */                
    TVRRecViewContexts iContext;
    
    /**
    * Own: Indicates USB connection dialog is open or not.
    * If value is ETrue, don't try to open it again.
    */
    TBool iUSBConnectedDialogOpen;
    };

#endif // __CVRRECVIEW_H__

