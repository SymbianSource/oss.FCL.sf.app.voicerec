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
*     This class encapsulates the functionality associated with renaming
*     files in different situation. The implementation is made according
*     to the Folders v.4.0 specification.
*
*
*/


#ifndef __TVRRENAME_H
#define __TVRRENAME_H

//  INCLUDES
#include <e32base.h>

// CLASS DECLARATION

/**
*  This class provides functionality to use in renaming operations
*/
class TVRRename
    {
    public:  // Constructors and destructor
        
        /**
        * C++ constructor.
		* @param aFs A reference to an open file server session.
        */
        IMPORT_C TVRRename( RFs& aFs );

    public: // New functions
        
        /**
        * This method executes a set of operations to do a file renaming operation
		* according to the Folders specification v.4.0. The method doesn't do the
		* actual renaming of the file, but returns the result path.
		*
        * @param aOldName A descriptor containing path to the file to be renamed.
		* @param aNewName A descriptor with a size of atleast KMaxFileName. The
		*        new path for the renamed is stored here. Do not rely on the contents
		*        of this descriptor if the method returns EFalse.
        * @param aPrompt A resource id to a string that is used as a prompt for the
		*        name query dialog.
        * @return (TBool) ETrue if the user confirmed the operation. 
		*         EFalse if the user canceled the operation.
        */
		IMPORT_C TBool GetNewNameL( const TDesC& aOldName, TDes& aNewName, const TInt aPrompt );

        /**
        * This method executes a set of operations to do a file renaming operation
		* according to the Folders specification v.4.0. This method carries out
		* the actual renaming of the file.
		*
        * @param aName A descriptor containing path to the file to be renamed.
        * @param aPrompt A resource id to a string that is used as a prompt for the
		*        name query dialog.
        * @return (TBool) ETrue if the user confirmed the operation. 
		*         EFalse if the user canceled the operation.
        */
		IMPORT_C TBool RenameL( TDes& aName, const TInt aPrompt );

        /**
        * This method executes a set of operations to do a file renaming operation
		* according to the Folders specification v.4.0. This method carries out
		* the actual renaming of the file.
		*
        * @param aFile A handle to the file to be renamed
        * @param aPrompt A resource id to a string that is used as a prompt for the
		*        name query dialog.
        * @return (TBool) ETrue if the user confirmed the operation. 
		*         EFalse if the user canceled the operation.
        */
		IMPORT_C TBool RenameL( RFile& aFile, const TInt aPrompt );

    private:    // Data

		// A reference to an open file server session
        RFs& iFs;

    };

#endif     // __TVRRENAME_H  
            
// End of File
