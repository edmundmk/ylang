//
//  oerror.h
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OERROR_H
#define OERROR_H


#include <exception>


/*
    Only throw errors with string literals (for now).
*/


class oerror : public std::exception
{
public:

    explicit oerror( const char* error );
    virtual const char* what() const throw();
    
private:

    const char* error;

};






#endif
