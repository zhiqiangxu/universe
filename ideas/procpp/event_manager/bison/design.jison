%{
var json = require('JSON');
%}
%lex

%%

\s+                   	;

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

"0x"[0-9A-F]+ 			return 'NUMBER';

[0-9]+ 					return 'NUMBER';

[_A-Za-z][_A-Za-z0-9]* 	return 'NAME';

<<EOF>>             	return 'EOF';

/lex

%start protocol

%%

protocol
	: PROTOCOL NAME '{' statements '}' EOF
		{ console.log(json.stringify($4, null, 2)); $$ = $4; }
	;

statements
	: statements statement
		{ $1.push($2); $$ = $1; }
	|
		{ $$ = []; }
	;

statement
	: states
		{ $$ = $1; }
	| flow
		{ $$ = $1; }
	| type
		{ $$ = $1; }
	;

states
	: STATES '{' names '}'
		{ $$ = {type:'states', names:$3}; }
	;

names
	: NAME
		{ $$ = [$1]; }
	| names NAME
		{ $1.push($2);$$ = $1; }
	;

flow
	: FLOW NAME '{' NAME '=>' NAME '}'
		{ $$ = {type:'flow', state:$2, request:$4, response:$6 }; }
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
	: RECORD NAME params '{' fields '}'
		{ $$ = {subtype:'record', name:$2, params: $3, fields:$5}; }
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
	| fields field
		{ $1.push($2); $$ = $1; }
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
	| NAME
		{ $$ = {user_type:$1}; }
	;

any
	: ANY NAME '{' fields '}'
		{ $$ = {subtype:'any', name:$2, fields:$4}; }
	;

case
	: CASE NAME '(' NAME NAME ')' OF '{' cases '}'
		{ $$ = {subtype:'case', name:$2, param_type:$4, param:$5,  }; }
	;

cases
	: NAME '.' NAME '=>' field
		{ $$ = [ {type_name:$1, field_name:$3, mapped: $5} ]; }
	| cases NAME '.' NAME '=>' field
		{ $1.push( {type_name:$2, field_name:$4, mapped: $6} ); $$ = $1; }
	;


