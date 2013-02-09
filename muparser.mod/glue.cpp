/*
  Copyright (c) 2008-2013 Bruce A Henderson
 
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:
  
  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.
  
  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/ 
#include "muParser.h"

extern "C" {

#include "blitz.h"

	BBObject * _bah_muparser_TmuParserException__create(mu::EErrorCodes code, BBString * msg, BBString * expr, int pos);
	BBArray * _bah_muparser_TmuParserBase__newVarArray(int size);
	void _bah_muparser_TmuParserBase__setVarArray(BBArray * vars, int index, BBString * name, double * variable);
	BBArray * _bah_muparser_TmuParserBase__newConstArray(int size);
	void _bah_muparser_TmuParserBase__setConstArray(BBArray * consts, int index, BBString * name, double value);

	mu::Parser * bmx_muparser_parser_new();
	void bmx_muparser_parser_delete(mu::Parser * par);

	double bmx_muparser_parserbase_eval(mu::ParserBase * par);
	void bmx_muparser_parserbase_setexpr(mu::ParserBase * par, BBString * expr);
	void bmx_muparser_parserbase_definevar(mu::ParserBase * par, BBString * name, double * value);
	void bmx_muparser_parserbase_clearvar(mu::ParserBase * par);
	void bmx_muparser_parserbase_clearfun(mu::ParserBase * par);
	void bmx_muparser_parserbase_clearconst(mu::ParserBase * par);
	void bmx_muparser_parserbase_clearinfixoprt(mu::ParserBase * par);
	void bmx_muparser_parserbase_clearpostfixoprt(mu::ParserBase * par);
	void bmx_muparser_parserbase_clearoprt(mu::ParserBase * par);

	void bmx_muparser_parserbase_definefun0(mu::ParserBase * par, BBString * name, mu::fun_type0 func, bool allowOpt);
	void bmx_muparser_parserbase_definefun1(mu::ParserBase * par, BBString * name, mu::fun_type1 func, bool allowOpt);
	void bmx_muparser_parserbase_definefun2(mu::ParserBase * par, BBString * name, mu::fun_type2 func, bool allowOpt);
	void bmx_muparser_parserbase_definefun3(mu::ParserBase * par, BBString * name, mu::fun_type3 func, bool allowOpt);
	void bmx_muparser_parserbase_definefun4(mu::ParserBase * par, BBString * name, mu::fun_type4 func, bool allowOpt);
	void bmx_muparser_parserbase_definefun5(mu::ParserBase * par, BBString * name, mu::fun_type5 func, bool allowOpt);
	void bmx_muparser_parserbase_definemultfun(mu::ParserBase * par, BBString * name, mu::multfun_type func, bool allowOpt);
	void bmx_muparser_parserbase_defineoprt(mu::ParserBase * par, BBString * name, mu::fun_type2 func, unsigned priority, int associativity, bool allowOpt);
	void bmx_muparser_parserbase_definepostfixoprt(mu::ParserBase * par, BBString * name, mu::fun_type1 func, bool allowOpt);
	void bmx_muparser_parserbase_defineinfixoprt(mu::ParserBase * par, BBString * name, mu::fun_type1 func, int precidence, bool allowOpt);

	void bmx_muparser_parserbase_removevar(mu::ParserBase * par, BBString * name);
	void bmx_muparser_parserbase_enableoptimizer(mu::ParserBase * par, bool isOn);
	void bmx_muparser_parserbase_enablebuiltintoprt(mu::ParserBase * par, bool isOn);
	bool bmx_muparser_parserbase_hasbuiltinoprt(mu::ParserBase * par);

	void bmx_muparser_parserbase_defineconst(mu::ParserBase * par, BBString * name, double value);
	void bmx_muparser_parserbase_definestrconst(mu::ParserBase * par, BBString * name, BBString * value);

	void bmx_muparser_parserbase_setvarfactory(mu::ParserBase * par, mu::facfun_type func, void * userData);
	BBArray * bmx_muparser_parserbase_getvar(mu::ParserBase * par);
	BBArray * bmx_muparser_parserbase_getusedvar(mu::ParserBase * par);
	BBArray * bmx_muparser_parserbase_getconst(mu::ParserBase * par);

}

mu::string_type bbStringToStringType(BBString * s) {
	char *p = bbStringToCString( s );
	mu::string_type t(p);
	bbMemFree( p );
	return t;
}

void bmx_muparser_throw(mu::Parser::exception_type &e) {
	bbExThrow(_bah_muparser_TmuParserException__create(e.GetCode(), bbStringFromCString(e.GetMsg().c_str()), 
		bbStringFromCString(e.GetExpr().c_str()), e.GetPos()));
}


mu::Parser * bmx_muparser_parser_new() {
	return new mu::Parser;
}

void bmx_muparser_parser_delete(mu::Parser * par) {
	delete par;
}

double bmx_muparser_parserbase_eval(mu::ParserBase * par) {
	try {
	
		return par->Eval();

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_setexpr(mu::ParserBase * par, BBString * expr) {
	try {
	
		par->SetExpr(bbStringToStringType(expr));

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_definevar(mu::ParserBase * par, BBString * name, double * value) {
	try {
	
		par->DefineVar(bbStringToStringType(name), value);
		
	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_clearvar(mu::ParserBase * par) {
	try {
	
		par->ClearVar();

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}


void bmx_muparser_parserbase_clearfun(mu::ParserBase * par) {
	try {
	
		par->ClearFun();

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}


void bmx_muparser_parserbase_clearconst(mu::ParserBase * par) {
	try {
	
		par->ClearConst();

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}


void bmx_muparser_parserbase_clearinfixoprt(mu::ParserBase * par) {
	try {
	
		par->ClearInfixOprt();

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}


void bmx_muparser_parserbase_clearpostfixoprt(mu::ParserBase * par) {
	try {
	
		par->ClearPostfixOprt();

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}


void bmx_muparser_parserbase_clearoprt(mu::ParserBase * par) {
	try {
	
		par->ClearOprt();

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_definefun0(mu::ParserBase * par, BBString * name, mu::fun_type0 func, bool allowOpt) {
	try {
	
		par->DefineFun(bbStringToStringType(name), func, allowOpt);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_definefun1(mu::ParserBase * par, BBString * name, mu::fun_type1 func, bool allowOpt) {
	try {
	
		par->DefineFun(bbStringToStringType(name), func, allowOpt);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_definefun2(mu::ParserBase * par, BBString * name, mu::fun_type2 func, bool allowOpt) {
	try {
	
		par->DefineFun(bbStringToStringType(name), func, allowOpt);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_definefun3(mu::ParserBase * par, BBString * name, mu::fun_type3 func, bool allowOpt) {
	try {
	
		par->DefineFun(bbStringToStringType(name), func, allowOpt);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_definefun4(mu::ParserBase * par, BBString * name, mu::fun_type4 func, bool allowOpt) {
	try {
	
		par->DefineFun(bbStringToStringType(name), func, allowOpt);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_definefun5(mu::ParserBase * par, BBString * name, mu::fun_type5 func, bool allowOpt) {
	try {
	
		par->DefineFun(bbStringToStringType(name), func, allowOpt);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_definemultfun(mu::ParserBase * par, BBString * name, mu::multfun_type func, bool allowOpt) {
	try {
	
		par->DefineFun(bbStringToStringType(name), func, allowOpt);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_removevar(mu::ParserBase * par, BBString * name) {
	try {
	
		par->RemoveVar(bbStringToStringType(name));

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_enableoptimizer(mu::ParserBase * par, bool isOn) {
	try {
	
		par->EnableOptimizer(isOn);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_enablebuiltintoprt(mu::ParserBase * par, bool isOn) {
	try {
	
		par->EnableBuiltInOprt(isOn);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

bool bmx_muparser_parserbase_hasbuiltinoprt(mu::ParserBase * par) {
	try {
	
		return par->HasBuiltInOprt();

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_defineconst(mu::ParserBase * par, BBString * name, double value) {
	try {
	
		par->DefineConst(bbStringToStringType(name), value);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_definestrconst(mu::ParserBase * par, BBString * name, BBString * value) {
	try {
	
		par->DefineStrConst(bbStringToStringType(name), bbStringToStringType(value));

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_defineoprt(mu::ParserBase * par, BBString * name, mu::fun_type2 func, unsigned priority, int associativity, bool allowOpt) {
	try {
	
		par->DefineOprt(bbStringToStringType(name), func, priority, (mu::EOprtAssociativity)associativity, allowOpt);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_definepostfixoprt(mu::ParserBase * par, BBString * name, mu::fun_type1 func, bool allowOpt) {
	try {
	
		par->DefinePostfixOprt(bbStringToStringType(name), func, allowOpt);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_defineinfixoprt(mu::ParserBase * par, BBString * name, mu::fun_type1 func, int precidence, bool allowOpt) {
	try {
	
		par->DefineInfixOprt(bbStringToStringType(name), func, precidence, allowOpt);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

void bmx_muparser_parserbase_setvarfactory(mu::ParserBase * par, mu::facfun_type func, void * userData) {
	try {
	
		par->SetVarFactory(func, userData);

	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

BBArray * bmx_muparser_getvariables(const mu::varmap_type & variables) {
	int size = (int)variables.size();
	
	BBArray * varArray = _bah_muparser_TmuParserBase__newVarArray(size);
	
	mu::varmap_type::const_iterator var = variables.begin();
	int index = 0;

	for (; var != variables.end(); ++var) {
		_bah_muparser_TmuParserBase__setVarArray(varArray, index++, bbStringFromCString(var->first.c_str()), var->second);
	}
	
	return varArray;
}

BBArray * bmx_muparser_parserbase_getvar(mu::ParserBase * par) {
	try {
	
		mu::varmap_type variables = par->GetVar();
		return bmx_muparser_getvariables(variables);
		
	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

BBArray * bmx_muparser_parserbase_getusedvar(mu::ParserBase * par) {
	try {
	
		mu::varmap_type variables = par->GetUsedVar();
		return bmx_muparser_getvariables(variables);
		
	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

BBArray * bmx_muparser_parserbase_getconst(mu::ParserBase * par) {
	try {
	
		mu::valmap_type consts = par->GetConst();
		int size = (int)consts.size();
		
		BBArray * constArray = _bah_muparser_TmuParserBase__newConstArray(size);
		
		mu::valmap_type::const_iterator var = consts.begin();
		int index = 0;
	
		for (; var != consts.end(); ++var) {
			_bah_muparser_TmuParserBase__setConstArray(constArray, index++, bbStringFromCString(var->first.c_str()), var->second);
		}
		
		return constArray;
		
	} catch (mu::Parser::exception_type &e) {
		bmx_muparser_throw(e);
	}
}

