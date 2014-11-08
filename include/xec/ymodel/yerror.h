//
//  oerror.h
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OERROR_H
#define OERROR_H


#include <exception>
#include <string>



class oerror : public std::exception
{
public:

    explicit oerror( const char* format, ... );
    virtual const char* what() const throw();
    
private:

    std::string message;

};



#endif
