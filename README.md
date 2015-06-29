ylang
=====

ylang is a dynamic, prototype-based language with clean semantics and a
familiar syntax.

The language is still under development.  Do not expect it to work at all.
Important parts of the language and of the virtual machine are not yet
implemented.


Features
--------

- Object literals, including out-of-line methods.

        def silver_sword : sword
        {
            var description = "Those who play with the devil's toys will "
                    "be brought by degrees to wield his sword.";
            var bonus;
            def damage();
        }
        
        def silver_sword.damage()
        {
            return roll( "3d8" ) + bonus;
        }

- Function closures.

        def bindall( list, token )
        {
            var bindings = [];
            for ( var element : list )
            {
                bindings.append( ?() { return element.invoke( token ); } );
            }
        }

- Multiple return values.

        def unpack( op ) 
        {
            return op & MOP, ( op & MA ) >> SA, ( op & MB ) >> SB; 
        }
        
        var a, b, c = unpack( op ) ...;
        var abc = [ unpack( op ) ... ];
        
        process( a, b, c );
        process( abc[] ... );
        process( unpack( op ) ... );

- Block scoping of variables.

        var i = 0;
        for ( var i = 4; i < 20; ++i )
        {
            if ( i_need_more_i )
            {
                var i = 30;
            }
            assert( i >= 4 && i < 20 );
        }
        assert( i == 0 );

- No accidental assignments into the global scope.

        var type = null;
        type = "ok";
        typo = null; // !!! error - typo hasn't been declared
        
        def global.global_function() { return "ok"; }
        global.global_variable = "ok";

- Clear separation between property keys and the indexes of a table or array.

        var mapping = {};
        mapping[ "length" ] = 5;
        assert( mapping.length() == 1 );
        assert( mapping.[ "length" ] is function );
        mapping.[ "length" ] = ?() { return 200; };
        assert( mapping.length() == 200 );
    

Afterword
---------

Poor workmen may blame their tools, but programmers inevitably get frustrated
with the idosyncracies of programming languages.  I think perhaps every
programmer has a language grammar hidden in the contours of their synapses.
ylang is my attempt to create an clean language for scripting which matches my
own mental model.

Like Lua, I have tried to keep it simple.  Multiple return values provide an
alternative to reference parameters or lots of small custom structures.  Unlike
Lua, it is hard to pollute ylang's global namespace with typos, and array
indexes are zero-based.

Like Javascript, it has prototype-based inheritance and a C-like braces and
semicolons syntax, but it fixes the broken scoping rules, adds object literals
actually useful for prototypes, and removes the need to type 'function' out
longhand every three lines.


_Edmund Kapusniak_

        

