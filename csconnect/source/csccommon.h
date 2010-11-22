/*
 +-------------------------------------------------------------------------------+
 | @BEGIN_COPYRIGHT                                                              |
 |                                                                               |
 | Copyright (C) 2010-2015, The Regents of the University of California &        |
 | King Abdullah University of Science and Technology                            |
 | All Rights Reserved.                                                          |
 |                                                                               |
 |                                                                               |
 | Prototyped and developed By:                                                  |
 |    Kai-Uwe Doerr       {kdoerr@usd.edu}                                       |
 |    Christopher Knox    {christopher.knox@kaust.edu.sa}                        |
 |                                                                               |
 | @END_COPYRIGHT                                                           	 |
 +-------------------------------------------------------------------------------+
 |
 | Component  : common headers
 | Filename   : csccommon.h
 | Sourcefile :
 | Language   : C++
 |
 +-------------------------------------------------------------------------------+ */


#ifndef INCLUDED_CSCCOMMON_H
#define INCLUDED_CSCCOMMON_H


/**
 * @file csccommon.h
 * This file is intended for recompilation in order to speed build times
 * and reduce warnings from external projects. As such it should not be included
 * in any header files
 */

/* c headers here */

/* c++ headers here */
#if defined __cplusplus
#include "dbclient.h"
#endif 

#endif
