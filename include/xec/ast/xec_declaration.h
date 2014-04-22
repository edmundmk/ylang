//
//  xec_declaration.h
//
//  Created by Edmund Kapusniak on 22/04/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#ifndef XEC_DECLARATION_H
#define XEC_DECLARATION_H


class xec_declaration
{
};



/*
    var name, name;
    var name, name = expr, exrp;
*/

class xec_declaration_variable : public xec_declaration
{
};


/*
    name { decl; decl; }
    name : expr { decl; decl; }
*/

class xec_declaration_object : public xec_declaration
{
};



/*
    name( paren, paren ) { decl, decl }
*/

class xec_declaration_function : public xec_declaration
{
};


/*
    name( paren, paren );
*/

class xec_declaration_prototype : public xec_declaration
{
};




#endif





