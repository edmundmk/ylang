//
//  osymbol.h
//
//  Created by Edmund Kapusniak on 26/10/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef OSYMBOL_H
#define OSYMBOL_H


/*
    Symbols are refcounted strings which store their hash.  This is the basis
    of lookup in omodel.  The model's symbol table ensures there is only one
    symbol for each string, removing the need for string comparisons once
    symbols have been created.  Normal strings must be converted to symbols
    before being used in lookup.
*/


/*
    The osymbol class is a strong pointer to a symbol.
*/

class osymbol
{
public:

};


/*
    The osymref class is a weak pointer to a symbol.
*/

class osymref
{
public:

    osymref( const osymbol& s );


};


#endif
