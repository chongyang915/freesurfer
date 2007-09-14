/**
 * @file  DataManager.cpp
 * @brief REPLACE_WITH_ONE_LINE_SHORT_DESCRIPTION
 *
 * REPLACE_WITH_LONG_DESCRIPTION_OR_REFERENCE
 */
/*
 * Original Author: REPLACE_WITH_FULL_NAME_OF_CREATING_AUTHOR 
 * CVS Revision Info:
 *    $Author: kteich $
 *    $Date: 2007/09/14 17:57:42 $
 *    $Revision: 1.13 $
 *
 * Copyright (C) 2002-2007,
 * The General Hospital Corporation (Boston, MA). 
 * All rights reserved.
 *
 * Distribution, usage and copying of this software is covered under the
 * terms found in the License Agreement file named 'COPYING' found in the
 * FreeSurfer source code root directory, and duplicated here:
 * https://surfer.nmr.mgh.harvard.edu/fswiki/FreeSurferOpenSourceLicense
 *
 * General inquiries: freesurfer@nmr.mgh.harvard.edu
 * Bug reports: analysis-bugs@nmr.mgh.harvard.edu
 *
 */


#include <iostream>
#include "string_fixed.h"
#include <list>
#include <stdexcept>
#include "DataManager.h"

using namespace std;

template <typename T>
DataLoader<T>::DataLoader() : DebugReporter() {}

template <typename T>
T* DataLoader<T>::GetData( string const& ifnData ) {

  // Iterate over our list of data. For each one, if the file name
  // we're given matches the data, increment the refernce count for
  // that data return a pointer to it.
  typename list<T*>::iterator tData;
  for ( tData = mlData.begin(); tData != mlData.end(); ++tData ) {
    T* data = *tData;
    if ( this->DoesFileNameMatchObject( data, ifnData ) ) {
      maRefs[data]++;
      return data;
    }
  }

  // If we made it here we haven't loaded the data yet, so do so now.
  T* data = this->LoadData( ifnData );

  // Start the ref count at one.
  maRefs[data] = 1;

  // Insert a reference to this data into our list.
  mlData.push_back( data );

  // Return the data.
  return data;
}

template <typename T>
void
DataLoader<T>::ReleaseData( T** ioData ) {

  // Look for this data in our list of data.
  typename list<T*>::iterator tData;
  for ( tData = mlData.begin(); tData != mlData.end(); ++tData ) {
    T* data = *tData;

    // If we found it...
    if ( data == *ioData ) {

      // Decrement our reference count.
      maRefs[data]--;

      // If that was our last reference, remove the data from our list
      // and call our FreeData function.
      if ( 0 == maRefs[data] ) {
        mlData.remove( data );
        this->FreeData( &data );
      }

      // Set the io pointer to NULL.
      *ioData = NULL;
      return;
    }
  }
}


template <typename T>
int
DataLoader<T>::CountReferences( T const* iData ) const {

  // Try to find the reference count for this data and return it.
  typename map<T*,int>::const_iterator tRefs;
  tRefs = maRefs.find( const_cast<T*>(iData) );
  if( tRefs != maRefs.end() )
    return tRefs->second;

  // Not in our list, so return 0.
  return 0;
}



// Generate this instance of the template code.
template class DataLoader<MRI>;

MRI*
MRILoader::LoadData( std::string const& ifnData ) {

  // Use MRIread to load the MRI object. Need to make a non-const,
  // c-string copy of the file name.
  char* fnMRI = strdup( ifnData.c_str() );
  MRI* mri = MRIread( fnMRI );
  free( fnMRI );

  // If the load failed, return an error.
  if ( NULL == mri ) {
    DebugOutput( << "MRIread() failed for " << fnMRI );
    throw runtime_error("Couldn't load MRI.");
  }

  return mri;
}

void
MRILoader::FreeData( MRI** ioMRI ) {

  // Call MRIfree. This will set *ioMRI to NULL if successful.
  MRIfree( ioMRI );
}

bool
MRILoader::DoesFileNameMatchObject( const MRI* iData, 
				    std::string const& ifnData ) const {

  // Look at the fname member in the MRI structure and compare it
  // to the file name we're getting.
  std::string const fnCur( iData->fname );
  return (fnCur == ifnData);
}

// Generate this instance of the template code.
template class DataLoader<MRIS>;

MRIS*
MRISLoader::LoadData( std::string const& ifnData ) {

  // Use MRISread to load the MRIS object. Need to make a non-const,
  // c-string copy of the file name.
  char* fnMRIS = strdup( ifnData.c_str() );
  MRIS* mris = MRISread( fnMRIS );
  free( fnMRIS );

  // If the load failed, return an error.
  if ( NULL == mris ) {
    DebugOutput( << "MRISread() failed for " << fnMRIS );
    throw runtime_error("Couldn't load MRIS.");
  }

  return mris;
}

void
MRISLoader::FreeData( MRIS** ioMRIS ) {

  // Call MRISfree. This will set *ioMRIS to NULL if successful.
  MRISfree( ioMRIS );
}

bool
MRISLoader::DoesFileNameMatchObject( const MRIS* iData, 
				     std::string const& ifnData ) const {

  // Look at the fname member in the MRI structure and compare it
  // to the file name we're getting.
  std::string const fnCur( iData->fname );
  return (fnCur == ifnData);
}


DataManager::DataManager() : DebugReporter() {}

// Each of these maintains and returns a static manager.
DataManager&
DataManager::GetManager() {
  static DataManager sManager;
  return sManager;
}

MRILoader&
DataManager::GetMRILoader() {
  static MRILoader sLoader;
  return sLoader;
}

MRISLoader&
DataManager::GetMRISLoader() {
  static MRISLoader sLoader;
  return sLoader;
}

