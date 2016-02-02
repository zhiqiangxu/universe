%{

/***bison rule***/
/***
If action code returns, scanning resumes on the next call to yylex();
If it doesn’t return, scanning resumes immediately.
***/

#include "ast.h"
#include "design.tab.h"

%}

%option noyywrap nodefault yylineno

%%

"Protocol" { return PROTOCOL; }

"states" { return STATES; }

"flow" { return FLOW; }

"record" { return RECORD; }

"any" { return ANY; }

"case" { return CASE; }

"of" { return OF; }


"=>" { return ASSOC_ARROW; }

"{" |

"}" |

"[" |

"]" |

":" { return yytext[0]; }


[0-9]+ { return NUMBER; }

[A-Za-z][A-Za-z0-9]* { return NAME; }


\n |
. { printf("unmatched\n"); }

%%

int main()
{
	int token;

	/*
		extern FILE* yyin;
		yyin=fopen("fileYouWantOpen","r");
	*/
	while (token = yylex()) {
		printf("hello%d\n", token);
	}

	return 0;
}
