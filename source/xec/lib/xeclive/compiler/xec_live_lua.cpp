//
//  xec_live_lua.cpp
//
//  Created by Edmund Kapusniak on 04/09/2014.
//  Copyright (c) 2014 Edmund Kapusniak. All rights reserved.
//


#include "xec_live_lua.h"


#if 0

xec_live_lua::xec_live_lua()
{
    opformat( XEC_SEQ_NULL,     BASIC,      "null"                      );
    opformat( XEC_SEQ_ONE,      BASIC,      "1"                         );

    opformat( XEC_SEQ_POS,      UNARY,      "--%a"                      );
    opformat( XEC_SEQ_NEG,      UNARY,      "-%a"                       );
    opformat( XEC_SEQ_NOT,      UNARY,      "not %a"                    );
    opformat( XEC_SEQ_BITNOT,   BASIC,      "__xec_not( %a )"           );

    opformat( XEC_SEQ_MUL,      MULTIPLY,   "%a * %b"                   );
    opformat( XEC_SEQ_DIV,      MULTIPLY,   "%a / %b"                   );
    opformat( XEC_SEQ_MOD,      MULTIPLY,   "%a %% %b"                   );
    opformat( XEC_SEQ_INTDIV,   BASIC,      "__xec_intdiv( %a, %b )"    );
    opformat( XEC_SEQ_ADD,      ADD,        "%a + %b"                   );
    opformat( XEC_SEQ_SUB,      ADD,        "%a - %b"                   );
    opformat( XEC_SEQ_LSL,      BASIC,      "__xec_lsl( %a, %b )"       );
    opformat( XEC_SEQ_LSR,      BASIC,      "__xec_lsr( %a, %b )"       );
    opformat( XEC_SEQ_ASR,      BASIC,      "__xec_asr( %a, %b )"       );
    opformat( XEC_SEQ_BITAND,   BASIC,      "__xec_and( %a, %b )"       );
    opformat( XEC_SEQ_BITXOR,   BASIC,      "__xec_xor( %a, %b )"       );
    opformat( XEC_SEQ_BITOR,    BASIC,      "__xec_or( %a, %b )"        );
    opformat( XEC_SEQ_CONCAT,   -CONCAT,    "%a .. %b"                  );
    opformat( XEC_SEQ_EQ,       COMPARE,    "%a == %b"                  );
    opformat( XEC_SEQ_NE,       COMPARE,    "%a ~= %b"                  );
    opformat( XEC_SEQ_LT,       COMPARE,    "%a < %b"                   );
    opformat( XEC_SEQ_LE,       COMPARE,    "%a <= %b"                  );
    opformat( XEC_SEQ_GT,       COMPARE,    "%a > %b"                   );
    opformat( XEC_SEQ_GE,       COMPARE,    "%a >= %b"                  );
    opformat( XEC_SEQ_IN,       BASIC,      "__xec_in( %a, %b )"        );
    opformat( XEC_SEQ_NOTIN,    UNARY,      "not __xec_in( %a, %b )"    );
    opformat( XEC_SEQ_IS,       BASIC,      "__xec_is( %a, %b )"        );
    opformat( XEC_SEQ_NOTIS,    UNARY,      "not __xec_is( %a, %b )"    );
    opformat( XEC_SEQ_XOR,      BASIC,      "__xec_lxor( %a, %b )"      );
    
    opformat( XEC_SEQ_KEY,      BASIC,      "%a.%k"                     );
    opformat( XEC_SEQ_INKEY,    BASIC,      "%a[ %b ]"                  );
    opformat( XEC_SEQ_INDEX,    BASIC,      "%a.__xec_index[ %b ]"      );
}


xec_live_lua::~xec_live_lua()
{
}


#endif