//
//  StringUtil.h
//  OpenSFZ
//
//  Created by David Wallin on 10/27/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#ifndef OpenSFZ_StringUtil_h
#define OpenSFZ_StringUtil_h

#include <iostream>
#include <string>

extern std::string stringTrim(const std::string& str,
                       const std::string& whitespace = " \t");

#endif
