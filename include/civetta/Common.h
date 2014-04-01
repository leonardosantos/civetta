#ifndef _CIVETTA_COMMON_H
#define _CIVETTA_COMMON_H

#ifdef _MSC_VER
# pragma once
# pragma warning(disable:4251)  // Disable VC warning about dll linkage required (for private members?)
# pragma warning(disable:4275)  // Disable warning about non dll-interface base class.
#endif

#if defined(_MSC_VER) && defined(CIVETTA_USE_DLL)
  #ifdef CIVETTA_BUILD_DLL
    #define CIVETTA_EXPORT __declspec(dllexport)
  #else
    #define CIVETTA_EXPORT __declspec(dllimport)
  #endif
#else
  #define CIVETTA_EXPORT
#endif

#endif // _CIVETTA_COMMON_H
