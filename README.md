## Ylang

Ylang is a dynamic, prototype-based language with clean semantics and a
familiar syntax.

Ylang is licensed under the MIT license.

The language is still under development.  I make no guarantees that it will
work for your use case!

See the [wiki](https://github.com/edmundmk/ylang/wiki) for documentation,
such as there is!


## Features

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
                bindings.append( ^() { return element.invoke( token ); } );
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
        mapping.[ "length" ] = ^() { return 200; };
        assert( mapping.length() == 200 );
    
    
## Implementation

The implementation is a register-based virtual machine.  The compiler uses
an pseudo-SSA form as an intermediate representation and makes an attempt at
sensible register allocation using a modified linear scan algorithm.

## Status

This project is archived.  However, see my latest scripting language,
(kenaf)[https://github.com/edmundmk/kenaf], the implementation of which owes
a lot to what I learned from ylang.

