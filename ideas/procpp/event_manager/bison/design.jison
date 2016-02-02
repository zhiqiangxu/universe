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

"{" 					return '{';

"}" 					return '}';

"[" 					return '[';

"]" 					return ']';

":" 					return ':';

[0-9]+ 					return 'NUMBER';

[A-Za-z][A-Za-z0-9]* 	return 'NAME';


<<EOF>>             	return 'EOF';

/lex

%start protocol

%%

protocol
	: PROTOCOL NAME '{' statements '}'
		{ }
	;

statements
	: states
		{}
	| flow
		{}
	| type
		{}
	;

states
	: STATES '{' names '}'
		{ }
	;

names
	: NAME
		{}
	| names NAME
		{}
	;

flow
	: FLOW NAME '{' NAME ASSOC_ARROW NAME '}'
		{}
	;


type
	: record
		{}
	| any
		{}
	| case
		{}
	;

record
	: RECORD NAME '{' fields '}'
		{}
	;

fields
	: field
		{}
	| fields field
		{}
;

field
	: TYPE NUMBER
		{}
	| TYPE NUMBER ':' NAME
		{}
	| TYPE NAME
		{}
	| TYPE NAME '[' NUMBER ']'
		{}
	| TYPE NAME '[' NAME ']'
		{}
	| NAME
		{}
	| NAME '(' NAME ')'
		{}
	;

any
	: ANY NAME '{' fields '}'
		{}
	;

case
	: CASE NAME '(' NAME NAME ')' OF '{' NAME '.' NAME ASSOC_ARROW field '}'
		{}
	;



