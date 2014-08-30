-- xec_lua.lua
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


-- simple

local meta_simple = {}

function meta_simple.__index( simple, index )
    _G.error( "attempt to index a non-indexable value", 2 )
end

function meta_simple.__newindex( simple, index )
    _G.error( "attempt to index a non-indexable value", 2 )
end

local boolean = {}
local number = {}
local string = {}

setmetatable( boolean, meta_simple )
setmetatable( number, meta_simple )
setmetatable( string, meta_simple )


-- objects

local meta_object = {}

function meta_object.__index( object, index )

    if type( index ) ~= "string" then
        error( "non-string value used as key", 2 )
    end

    if object.__xec_proto then
        return object.__xec_proto[ index ]
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

function object:__this( ... )
end

local function __xec_new_object( proto )
    local object = {}
    object.__xec_proto = proto;
    setmetatable( object, meta_object )
    object:__xec_setup()
    return object
end

local function __xec_new( proto, ... )
    local object = __xec_new_object( proto )
    object:this( ... )
    return object
end


-- arrays

local meta_array = {}

function meta_array.__index( array, index )

    if index % 1 ~= 0 then
        error( "array indexes must be integers" )
    end

    if index < 0 or index >= array.length then
        error( "index out of range" )
    end

    return null

end

function meta_array.__newindex( array, index, value )

    if index % 1 ~= 0 then
        error( "array indexes must be integers" )
    end

    if index < 0 or index >= array.length then
        error( "index out of range" )
    end

    rawset( array, index, value )

end

local array = __xec_new_object( object )

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

local table = __xec_new_object( object )

function table:__xec_setup()
    self.__xec_index = {}
    setmetatable( self.__xec_index, meta_table )
end


-- operators

local function __xec_in( object, key )

    return rawget( object, key ) ~= nil

end

local simple_proto =
{
    [ "boolean"  ] = boolean,
    [ "number"   ] = number,
    [ "string"   ] = string,
    [ "function" ] = function,
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

        object = object.__xec_proto

    end

    return false

end


-- build xec global scope





















-- environment table for xec functions

local xec = {}
_G.setfenv( 1, xec )


-- xec differentiates between 'absent key' and 'null'.

null = {}

local meta_null = {}

function meta_null.__tostring()
    return "null"
end

function meta_null.__index( null, index )
    _G.error( "attempt to index a null value", 2 )
end

function meta_null.__newindex( null, index, value )
    _G.error( "attempt to index a null value", 2 )
end

_G.setmetatable( null, meta_null )


-- xec simple value prototypes

boolean = {}
number = {}
string = {}


local meta_simple = {}

function meta_simple.__index( simple, index )
    _G.error( "attempt to index a non-indexable value", 2 )
end

function meta_simple.__newindex( simple, index )
    _G.error( "attempt to index a non-indexable value", 2 )
end

_G.setmetatable( boolean, meta_simple )
_G.setmetatable( number, meta_simple )
_G.setmetatable( string, meta_simple )



-- xec objects


local meta_object = {}

function meta_object.__index( object, index )

    if _G.type( index ) ~= "string" then
        _G.error( "non-string value used as key", 2 )
    end

    if object.__xec_proto then
        return object.__xec_proto[ index ]
    end

    error( "key '" .. index .. "' not found", 2 )

end

function meta_object.__newindex( object, index, value )

    if _G.type( index ) ~= "string" then
        _G.error( "non-string value used as key", 2 )
    end

    _G.rawset( object, index, value )

end


function __xec_new_object( proto )
    local object = {}
    object.__xec_proto = object;


end

function __xec_new( proto, ... )

end



-- xec metatables





local meta_table = {}

function meta_table.__index( table, index )
    return null
end


-- xec object prototypes

object = {}

function object:__xec_create()
end

function object:this( ... )
end


array = {}
array.__xec_proto = object

function array:__xec_create()
    self.__xec_proto.__xec_create( self )
    self.__xec_array = {}
    _G.setmetatable( self.__xec_array, meta_array )
end


table = {}
table.__xec_proto = object

function table:__xec_create()
    self.__xec_proto.__xec_create( self )
    self.__xec_table = {}
    _G.setmetatable( self.__xec_table, meta_table )
end


_G.setmetatable( object, object_meta )
_G.setmetatable( array, object_meta )
_G.setmetatable( table, object_meta )


-- xec callable prototypes

coroutine = {}

function = {}




-- map from lua type names to xec prototypes

__xec_luaproto = {}
__xec_luaproto[ "boolean" ]  = boolean
__xec_luaproto[ "number" ]   = number
__xec_luaproto[ "string" ]   = string
__xec_luaproto[ "function" ] = function


-- bitwise operators

__xec_not = bit.bnot
__xec_lsl = bit.lshift
__xec_lsr = bit.rshift
__xec_asr = bit.arshift
__xec_and = bit.band
__xec_xor = bit.bxor
__xec_or  = bit.bor

function __xec_test( v )
end

function __xec_lxor( a, b )
end


