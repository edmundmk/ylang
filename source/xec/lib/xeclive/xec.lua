-- xec.lua
--
-- Created by Edmund Kapusniak on 30/08/2014.
-- Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
--


local bit = require( "bit" )


-- null is distinct from nil, nil effectively becomes 'undefined'

local null = {}

local meta_null = {}

function meta_null.__tostring()
    return "null"
end

function meta_null.__index( null, index )
    error( "attempt to index a null value", 2 )
end

function meta_null.__newindex( null, index, value )
    error( "attempt to index a null value", 2 )
end

setmetatable( null, meta_null )


-- objects

local meta_object = {}

function meta_object.__index( object, index )

    if type( index ) ~= "string" then
        error( "non-string value used as key", 2 )
    end

    if object.super then
        return object.super[ index ]
    end

    error( "key '" .. index .. "' not found", 2 )

end

function meta_object.__newindex( object, index, value )

    if type( index ) ~= "string" then
        error( "non-string value used as key", 2 )
    end

    rawset( object, index, value )

end

local object = {}
setmetatable( object, meta_object )

function object:__xec_setup()
end

function object:this( ... )
end

local function __xec_obj( proto )
    local object = {}
    object.super = proto;
    setmetatable( object, meta_object )
    object:__xec_setup()
    return object
end

local function __xec_new( proto, ... )
    local object = __xec_obj( proto )
    object:this( ... )
    return object
end

local function __xec_delete( object, key )
    rawset( object, key, nil )
end


-- primitive values

local boolean    = __xec_obj( object )
local number     = __xec_obj( object )
local __function = __xec_obj( object )


-- strings

local string = __xec_obj( object )

function string:__xec_setup()
    error( "cannot inherit from string", 3 )
end

function string:length()
    return _G.string.len( self )
end

local meta_string = getmetatable( "" )
meta_string.__index = string


-- arrays

local meta_array = {}

function meta_array.__index( array, index )

    if index % 1 ~= 0 then
        error( "array indexes must be integers", 2 )
    end

    if index < 0 or index >= array.length then
        error( "index out of range", 2 )
    end

    return null

end

function meta_array.__newindex( array, index, value )

    if index % 1 ~= 0 then
        error( "array indexes must be integers", 2 )
    end

    if index < 0 or index >= array.length then
        error( "index out of range", 2 )
    end

    rawset( array, index, value )

end

local array = __xec_obj( object )

function array:__xec_setup()
    self.__xec_index = {}
    self.__xec_index.length = 0
    setmetatable( self.__xec_index, meta_array )
end


-- tables

local meta_table = {}

function meta_table.__index( table, index )
    return null
end

local table = __xec_obj( object )

function table:__xec_setup()
    self.__xec_index = {}
    setmetatable( self.__xec_index, meta_table )
end


-- coroutines

local coroutine = __xec_obj( object )


-- operators

local function __xec_intdiv( a, b )
    return math.floor( a / b )
end

local function __xec_test( v )
    return v and v ~= null and v ~= 0;
end

local function __xec_lxor( a, b )
    return not __xec_test( a ) ~= not __xec_test( b )
end

local function __xec_tkey( object, key )
    while object do
        if __xec_test( rawget( object, key ) ) then
            return true
        end

        object = object.super
    end

    return false
end

local function __xec_in( object, key )
    return rawget( object, key ) ~= nil
end

local simple_proto =
{
    [ "boolean"  ] = boolean,
    [ "number"   ] = number,
    [ "string"   ] = string,
    [ "function" ] = __function
}

local function __xec_is( object, proto )

    local simple = simple_proto[ type( object ) ]
    if simple then
        return proto == simple
    end

    while object do

        if object == proto then
            return true
        end

        object = object.super

    end

    return false
end


-- build xec global scope

local g = __xec_obj( object )


-- null

g.null              = null


-- builtin prototypes

g.boolean           = boolean
g.number            = number
g.string            = string
g.object            = object
g[ "function" ]     = __function
g.coroutine         = coroutine


-- operations

g.__xec_obj     = __xec_obj
g.__xec_new     = __xec_new
g.__xec_delete  = __xec_delete

g.__xec_intdiv  = __xec_intdiv
g.__xec_not     = bit.bnot
g.__xec_lsl     = bit.lshift
g.__xec_lsr     = bit.rshift
g.__xec_asr     = bit.arshift
g.__xec_and     = bit.band
g.__xec_xor     = bit.bxor
g.__xec_or      = bit.bor

g.__xec_test    = __xec_test
g.__xec_lxor    = __xec_lxor

g.__xec_tkey    = __xec_tkey
g.__xec_in      = __xec_in
g.__xec_is      = __xec_is


return g;









