/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*  
 * Copyright (C) 2012-2020 Euclid Science Ground Segment    
 *  
 * This library is free software; you can redistribute it and/or modify it under the terms of the GNU Lesser General  
 * Public License as published by the Free Software Foundation; either version 3.0 of the License, or (at your option)  
 * any later version.  
 *  
 * This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied  
 * warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more  
 * details.  
 *  
 * You should have received a copy of the GNU Lesser General Public License along with this library; if not, write to  
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA  
 */    

/**
 * @file VignetPlugin.cpp
 *
 * @date Jan 23, 2020
 * @author mkuemmel@usm.lmu.de
 */

#include "SEFramework/Plugin/StaticPlugin.h"
#include "SEImplementation/Plugin/Vignet/VignetTaskFactory.h"

namespace SourceXtractor {
std::vector<int> VignetTaskFactory::stringToIntVec(const std::string inString, const std::string delimiters)
{
  std::vector<int> result;
  int anInt=0;
  std::size_t first;
  std::size_t last;

  // convert the input string to a vector of strings along the commas
  std::vector<std::string> stringVec=stringSplit(inString, delimiters);

  // go over all members
  for (size_t index=0; index<stringVec.size(); index++)
  {
    // prepare trimming
    first = stringVec[index].find_first_not_of(' ');
    last  = stringVec[index].find_last_not_of(' ');

    try
    {
      // try converting to int and append to result vector
      anInt = boost::lexical_cast<size_t>(stringVec[index].substr(first, last-first+1));
      result.push_back(anInt);
    }
    catch ( const boost::bad_lexical_cast &exc ) // conversion failed, exception thrown by lexical_cast and caught
    {
      throw Elements::Exception() << "Can not convert to 'int': '" << stringVec[index].substr(first, last-first+1) << "'";
    }
  }

  return result;
}

std::vector<std::string> VignetTaskFactory::stringSplit(const std::string inString, const std::string delimiters)
{
  std::vector<std::string> result;
  std::string trimString;
  size_t current;
  size_t next = -1;
  size_t first=0;
  size_t last=0;

  // trim blanks from both sides;
  // return the empty vector if there
  // are only blanks
  first = inString.find_first_not_of(' ');
  if (first == std::string::npos)
    return result;
  last  = inString.find_last_not_of(' ');
  trimString = inString.substr(first, last-first+1);

  do
  { // split along the delimiter
    // and add to the result vector
    current = next + 1;
    next = trimString.find_first_of( delimiters, current );
    result.push_back(trimString.substr( current, next - current ));
  }
  while (next != std::string::npos);

  return result;
}
}
