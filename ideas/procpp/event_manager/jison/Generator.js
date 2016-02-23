var json = require('JSON');
var extend = require('util')._extend;


var debug = function (obj)
{
	console.log(json.stringify(obj, null, 2));
};

var exit = function(msg)
{
	debug(msg);
	process.exit();
}

var traversal = (function(obj, callback)
{
	function f(obj, callback) {
		if (typeof(obj) == 'object') {
			if (obj instanceof Array) {
				for (var i in obj) {
					f(obj[i], callback);
					callback(obj[i]);
				}
			} else {
				for (var k in obj) {
					f(obj[k], callback);
					callback(obj[k], k);
				}

			}
		}
	}

	return f;
})();

var auto_anonymous_id = function()
{
	if (typeof this._auto_id == 'undefined') {
		this._auto_id = 0;
	}

	this._auto_id++;
	return this._auto_id;
}

var auto_field_id = function()
{
	if (typeof this._auto_id == 'undefined') {
		this._auto_id = 0;
	}

	this._auto_id++;
	return this._auto_id;
}

var get_all_annonymous = function(ast)
{
	var result = [];

	return result;
}

var get_all_annonymous_field = function(ast)
{
	var result = [];

	return result;
}

var get_field_type = function(field)
{
}

var resolve = function(exp, field)
{
}

var assign_auto_name_to_annonymous = function(ast)
{
	var annonymous = get_all_annonymous(ast);//reference
	for (var i = 0; i < annonymous.length; i++) {
		var prefix = annonymous[i].record ? 'record_' : 'any_';
		annonymous[i].auto_name = prefix + auto_anonymous_id();
	}

}

var assign_auto_name_to_annonymous_field = function(ast)
{
	var fields = get_all_annonymous_field(ast);//reference
	for (var i = 0; i < fields.length; i++) {
		fields[i].auto_name = '_' + auto_field_id();
	}

}
/*struct related*/
var struct_record = function(struct)
{
	if (struct.name) {
		var class_name = struct.name;
	} else if (struct.auto_name) {
		// TODO move to internal namespace
		var class_name = struct.auto_name;
	}

	var code = 'class ' + class_name + " {\n";
	code	+= "public:					\n";

	var fields = struct.fields ? struct.fields : struct.record;
	for (var i = 0 ; i < fields; i++) {
		var field = fields[i];
		var field_name = field.name ? field.name : field.auto_name;

		if (!field_name) exit("field has neither name nor auto_name");

		if (field.type) {
			var field_type = field.type;
		} else if (field.user_type) {
			var field_type = field.user_type;
		} else if (field.anonymous) {
			var field_type = field.anonymous.auto_name;
		}

		if (field.n) {

			var ret = resolve(field.n, field);
			if (ret.constant) {
				var n = ret.value;
				code += field_type + " " + field_name + "[" + n + "];\n";
			} else {
				code += "vector<" + field_type + "> " + field_name + ";\n";//POD
			}

		} else {

			code += field_type + " " + field_name + ";\n";

		}
	}

	code += "		void parseFromStream(IStringStream& s) {\n";
	code += parse_record(struct);
	code += "		}\n";

	code += "};\n"

	return code;
}

var struct_any = function(struct)
{
	if (struct.name) {
		var class_name = struct.name;
	} else if (struct.auto_name) {
		// TODO move to internal namespace
		var class_name = struct.auto_name;
	}

	var code = 'class ' + class_name + " {\n";
	code	+= "private:					\n";
	code 	+= "	int8_t _idx;\n";
	code	+= "public:					\n";
	code	+= "	struct {\n";

	var fields = struct.fields ? struct.fields : struct.any;
	for (var i = 0 ; i < fields; i++) {
		var field = fields[i];
		var field_name = field.name ? field.name : field.auto_name;

		if (!field_name) exit("field has neither name nor auto_name");

		if (field.type) {
			var field_type = field.type;
		} else if (field.user_type) {
			var field_type = field.user_type;
		} else if (field.anonymous) {
			var field_type = field.anonymous.auto_name;
		}

		if (field.n) {

			var ret = resolve(field.n, field);
			if (ret.constant) {
				var n = ret.value;
				code += field_type + " " + field_name + "[" + n + "];\n";
			} else {
				code += "vector<" + field_type + "> " + field_name + ";\n";
			}

		} else {

			code += field_type + " " + field_name + ";\n";

		}
	}

	code += "	} _u;\n"

	code += "		void parseFromStream(IStringStream& s) {\n";
	code += parse_any(struct);
	code += "		}\n";

	code += "};\n";
}

var struct_case = function(struct)
{
	if (struct.name) {
		var class_name = struct.name;
	} else if (struct.auto_name) {
		// TODO move to internal namespace
		var class_name = struct.auto_name;
	}

	var code = 'class ' + class_name + " {\n";
	code	+= "public:					\n";
	code 	+= "	int8_t _idx;\n";
	code	+= "	struct {\n";

	var cases = struct.cases;
	for (var i = 0 ; i < cases; i++) {
		var field = cases[i].result_field;
		var field_name = field.name ? field.name : field.auto_name;

		if (!field_name) exit("field has neither name nor auto_name");

		if (field.type) {
			var field_type = field.type;
		} else if (field.user_type) {
			var field_type = field.user_type;
		} else if (field.anonymous) {
			var field_type = field.anonymous.auto_name;
		}

		if (field.n) {

			var ret = resolve(field.n, field);
			if (ret.constant) {
				var n = ret.value;
				code += field_type + " " + field_name + "[" + n + "];\n";
			} else {
				code += "vector<" + field_type + "> " + field_name + ";\n";
			}

		} else {

			code += field_type + " " + field_name + ";\n";

		}
	}

	code += "	} _u;\n"

	code += "		void parseFromStream(IStringStream& s) {\n";
	code += parse_case(struct);
	code += "		}\n";

	code += "};\n";
}

/*parse related*/
var parse_record = function(struct)
{
	var statements = [];
	var fields = struct.fields ? struct.fields : struct.record;
	for (var i = 0; i < fields.length; i++) {
		var field = fields[i];

		if (field.type) {

			if (field.n) {
				var ret = resolve(field.n, field);
				if (ret.constant) {
					statements.push({fixed:1, field:field});
				} else {
					statements.push({fixed:0, field:field});
				}
			} else {
				statements.push({fixed:1, field:field});
			}

		} else if (field.user_type) {
			statements.push({fixed:0, field:field});
		} else if (field.anonymous) {
			statements.push({fixed:0, field:field});
		}

	}

	var groups = [];
	for (var i = 0; i < statements.length; i++) {
		if (statements[i].fixed) {
			if (!groups.length) groups.push([]);

			groups[groups.length-1].push(statements.field);

		} else {
			groups.push([statements.field]);
		}
	}

	var code = '';
	for (var i = 0; i < groups.length; i++) {
		group = groups[i];
		if (group[0].type) {
		} else {
			if (group.length != 1) exit("two non plain type in one group");

			var field = group[0];
			switch(get_field_type(field)) {
				case 'record':
				case 'any':
					var field_name = field.name ? field.name : field.auto_name;
					code += field_name + ".parseFromStream(s);\n";
					break;
				case 'case':
					break;
				case 'type':
					break;
			}
		}
	}

	return code;

}

var parse_any = function(struct)
{
}

var parse_case = function(struct)
{
}

var eval = function (ast)
{
	debug(ast);

	assign_auto_name_to_annonymous(ast);
}

exports.eval = eval;
