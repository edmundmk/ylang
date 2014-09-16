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

    super = rawget( object, "super" )
    if super then
        return super[ index ]
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

function object:__setup()
end

function object:this( ... )
end

local function __newobj( proto )
    local object = {}
    object.super = proto;
    setmetatable( object, meta_object )
    object:__setup()
    return object
end

local function __new( proto, ... )
    local object = __newobj( proto )
    object:this( ... )
    return object
end

local function __delete( object, key )
    rawset( object, key, nil )
end

local function __eachkey( object )
    local function object_next( object, key )
        local key, value = next( object, key )
        return key, key, value
    end
    return object_next, object, nil
end


-- primitive values

local boolean    = __newobj( object )
local number     = __newobj( object )
local __function = __newobj( object )


-- strings

local string = __newobj( object )

function string:__setup()
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

local array = __newobj( object )

function array:__setup()
    self.__index = {}
    self.__index.length = 0
    setmetatable( self.__index, meta_array )
end

function array:__each()
    local function array_next( array, index )
        if index < array.length then
            return index + 1, array[ index ]
        else
            return nil
        end
    end
    return array_next, self.__index, 0
end

function array:length()
    return self.__index.length
end

function array:append( ... )
    local count = select( "#", ... )
    local length = self.__index.length;
    self.__index.length = length + count
    for i = 1, count do
        self.__index[ length - 1 + i ] = select( i, ... )
    end
end

local function __unpack( array )
    return unpack( array.__index, 0, array.__index.length - 1 )
end


-- tables

local meta_table = {}

function meta_table.__index( table, index )
    return null
end

local table = __newobj( object )

function table:__setup()
    self.__index = {}
    setmetatable( self.__index, meta_table )
end

function table:__each()
    local function table_next( table, key )
        local key, value = next( table, key )
        return key, key, value
    end 
    return table_next, self.__index, nil
end


-- coroutines

local coroutine = __newobj( object )


-- operators

local function __mono( mono, ... )
    return mono
end

local function __intdiv( a, b )
    return math.floor( a / b )
end

local function __test( v )
    return v and v ~= null and v ~= 0;
end

local function __lxor( a, b )
    return not __test( a ) ~= not __test( b )
end

local function __in( object, key )
    return rawget( object, key ) ~= nil
end

local simple_proto =
{
    [ "boolean"  ] = boolean,
    [ "number"   ] = number,
    [ "string"   ] = string,
    [ "function" ] = __function
}

local function __is( object, proto )

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

local g = __newobj( object )


-- null

g.null      = null


-- builtin prototypes

g.boolean   = boolean
g.number    = number
g.string    = string
g.object    = object
g.array     = array
g.table     = table
g[ "function" ] = __function
g.coroutine = coroutine


-- used for array and table literals

g.__object  = object
g.__array   = array
g.__table   = table


-- operations

g.__pcall   = pcall
g.__error   = error

g.__newobj  = __newobj
g.__new     = __new
g.__delete  = __delete
g.__eachkey = __eachkey
g.__unpack  = __unpack

g.__mono    = __mono
g.__intdiv  = __intdiv
g.__not     = bit.bnot
g.__lsl     = bit.lshift
g.__lsr     = bit.rshift
g.__asr     = bit.arshift
g.__and     = bit.band
g.__xor     = bit.bxor
g.__or      = bit.bor

g.__test    = __test
g.__lxor    = __lxor

g.__in      = __in
g.__is      = __is


return g






