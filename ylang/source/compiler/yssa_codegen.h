//
//  yssa_codegen.h
//
//  Created by Edmund Kapusniak on 04/04/2015.
//  Copyright (c) 2015 Edmund Kapusniak. All rights reserved.
//


#ifndef YSSA_CODEGEN_H
#define YSSA_CODEGEN_H



struct yssa_module;
class yl_function;
class yl_diagnostics;


yl_function yssa_codegen( yl_diagnostics* d, yssa_module* module, bool debugprint );



#endif
