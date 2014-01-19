//
//  StringUtil.cpp
//  OpenSFZ
//
//  Created by David Wallin on 10/27/13.
//  Copyright (c) 2013 David Wallin. All rights reserved.
//

#include "StringUtil.h"
std::string stringTrim(const std::string& str,
                       const std::string& whitespace)
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return ""; // no content
    
    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;
    
    return str.substr(strBegin, strRange);
}