/*
	Copyright 2010, Sumeet Chhetri 
  
    Licensed under the Apache License, Version 2.0 (the "License"); 
    you may not use this file except in compliance with the License. 
    You may obtain a copy of the License at 
  
        http://www.apache.org/licenses/LICENSE-2.0 
  
    Unless required by applicable law or agreed to in writing, software 
    distributed under the License is distributed on an "AS IS" BASIS, 
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. 
    See the License for the specific language governing permissions and 
    limitations under the License.  
*/
#ifndef JASON_SPIRIT_WRITER
#define JASON_SPIRIT_WRITER

//          Copyright John W. Wilkinson 2007 - 2009.
// Distributed under the MIT License, see accompanying file LICENSE.txt

// json spirit version 4.01

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

#include "json_spirit_value.h"
#include <iostream>

namespace json_spirit
{
    // functions to convert JSON Values to text, 
    // the "formatted" versions add whitespace to format the output nicely

    void         write          ( const Value& value, std::ostream&  os );
    void         write_formatted( const Value& value, std::ostream&  os );
    std::string  write          ( const Value& value );
    std::string  write_formatted( const Value& value );

#ifndef BOOST_NO_STD_WSTRING

    void         write          ( const wValue& value, std::wostream& os );
    void         write_formatted( const wValue& value, std::wostream& os );
    std::wstring write          ( const wValue& value );
    std::wstring write_formatted( const wValue& value );

#endif

    void         write          ( const mValue& value, std::ostream&  os );
    void         write_formatted( const mValue& value, std::ostream&  os );
    std::string  write          ( const mValue& value );
    std::string  write_formatted( const mValue& value );

#ifndef BOOST_NO_STD_WSTRING

    void         write          ( const wmValue& value, std::wostream& os );
    void         write_formatted( const wmValue& value, std::wostream& os );
    std::wstring write          ( const wmValue& value );
    std::wstring write_formatted( const wmValue& value );

#endif
}

#endif
