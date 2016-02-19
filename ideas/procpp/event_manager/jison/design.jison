%{

var generator = require("./Generator");

%}
%lex

%%

\n						return 'EOL';
[^\S\n]+				;

"//".*					;

"Protocol"				return 'PROTOCOL';

"states" 				return 'STATES';

"flow" 					return 'FLOW';

"record" 				return 'RECORD';

"any" 					return 'ANY';

"case" 					return 'CASE';

"of" 					return 'OF';

"=>" 					return '=>';

"(" 					return '(';

")" 					return ')';

"{" 					return '{';

"}" 					return '}';

"[" 					return '[';

"]" 					return ']';

"." 					return '.';

":" 					return ':';

"uint"[0-9]+"_t"		return 'TYPE';
"char"					return 'TYPE';
"wchar_t"				return 'TYPE';

"0x"[0-9A-F]+ 			%{ yytext = parseInt(yytext); return 'NUMBER'; %}

[0-9]+ 					%{ yytext = parseInt(yytext); return 'NUMBER'; %}

[_A-Za-z][_A-Za-z0-9]* 	return 'NAME';

<<EOF>>             	return 'EOF';

/lex

%start protocol

%%

protocol
	: PROTOCOL NAME eol '{' eol nodes eol '}' eol EOF
		{ $$ = { type:'protocol', name:$2, nodes:$6 }; generator.eval($$); }
	;

nodes
	: nodes eol node
		{ $1.push($3); $$ = $1; }
	| node
		{ $$ = [$1]; }
	;

node
	: states
		{ $$ = $1; }
	| flow
		{ $$ = $1; }
	| type
		{ $$ = $1; }
	;

states
	: STATES eol '{' eol names eol '}'
		{ $$ = {type:'states', names:$5}; }
	;

names
	: NAME
		{ $$ = [$1]; }
	| names eol NAME
		{ $1.push($3);$$ = $1; }
	;

flow
	: FLOW NAME eol '{' eol NAME '=>' NAME eol '}'
		{ $$ = { type:'flow', state:$2, request:$6, response:$8 }; }
	| FLOW eol '{' eol NAME '=>' NAME eol '}'
		{ $$ = { type:'flow', request:$5, response:$7 } }
	;


type
	: record
		{ $$ = {type:'type', def:$1}; }
	| any
		{ $$ = {type:'type', def:$1}; }
	| case
		{ $$ = {type:'type', def:$1}; }
	;

record
	: RECORD NAME eol '{' eol fields eol '}'
		{ $$ = {subtype:'record', name:$2, fields:$6}; }
	;

params
	:
		{ $$ = null; }
	| '(' NAME NAME ')'
		{ $$ = {type:$2, arg:$3}; }
	;

fields
	: field
		{ $$ = [$1]; }
	| fields one_eol field
		{ $1.push($3); $$ = $1; }
;

field
	: TYPE NUMBER
		{ $$ = {type:$1, value:$2}; }
	| TYPE NUMBER ':' NAME
		{ $$ = {type:$1, value:$2, name:$4}; }
	| TYPE NAME
		{ $$ = {type:$1, name:$2}; }
	| TYPE NAME '[' NUMBER ']'
		{ $$ = {type:$1, name:$2, n:$4}; }
	| TYPE NAME '[' NAME ']'
		{ $$ = {type:$1, name:$2, n:$4}; }
	| NAME '(' NAME ')'
		{ $$ = {user_type:$1, 'param':$3}; }
	| NAME NAME
		{ $$ = {user_type:$1, name:$2}; }
	| NAME
		{ $$ = {user_type:$1}; }
	| anonymous NAME
		{ $$ = { anonymous:$1, name:$2 }; }
	| anonymous NAME '[' NUMBER ']'
		{ $$ = { anonymous:$1, name:$2, n:$4 }; }
	| anonymous NAME '[' NAME ']'
		{ $$ = { anonymous:$1, name:$2, n:$4 }; }
	;

anonymous
	: RECORD eol '{' eol fields eol '}'
		{ $$ = { record:$5 }; }
	| ANY eol '{' eol fields eol '}'
		{ $$ = { any:$5 }; }
	;

any
	: ANY NAME eol '{' eol fields eol '}'
		{ $$ = {subtype:'any', name:$2, fields:$6}; }
	;

case
	: CASE NAME params OF eol '{' eol cases eol '}'
		{ $$ = {subtype:'case', name:$2, params:$3, cases:$8 }; }
	;

cases
	: NAME '.' NAME '=>' field
		{ $$ = [ {type_name:$1, field_name:$3, result_field: $5} ]; }
	| NUMBER '=>' field
		{ $$ = [ {n:$1, result_field:$3} ] }
	| cases one_eol NAME '.' NAME '=>' field
		{ $1.push( {type_name:$3, field_name:$5, result_field: $7} ); $$ = $1; }
	;


eol
	: one_eol
		{}
	|
		{}
	;

one_eol
	: eol EOL
		{}
	;
