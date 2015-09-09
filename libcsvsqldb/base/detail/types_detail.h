//
//  types_detail.h
//  csvsqldb
//
//  Copyright (c) 2015 Fürstenberg IT Systems. All rights reserved.
//

#ifndef csvsqldb_types_detail_h
#define csvsqldb_types_detail_h

#include <iostream>


namespace csvsqldb
{
    namespace detail
    {
        struct tag {};
        
        struct any
        {
            template <typename T>
            any(T const&);
        };
        
        tag operator << (std::ostream&, any const&);
        
        typedef char yes;
        typedef char (&no)[2];
        
        no checkit(tag);
        
        template <typename T>
        yes checkit(T const&);
        
        template <typename T>
        struct is_output_streamable_impl
        {
            static typename std::remove_cv<typename std::remove_reference<T>::type>::type const& x;
            static const bool value = sizeof(checkit(std::cout << x)) == sizeof(yes);
        };
    }
}

#endif
