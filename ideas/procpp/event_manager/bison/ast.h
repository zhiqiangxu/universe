#pragma once
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

extern int yylineno; /* from lexer */

extern "C"
{
	int yylex(void);
	void yyerror(char *s, ...);
}

class INode
{
public:

	int _type;

};

class Node : public INode
{
public:

};
