var json = require('JSON');

//it generates:
//
// for server side:
// ParseResult on_message(int client, string message);
// void on_close(int client);
//
// for client side:
// TODO
//
var debug = function (obj)
{
	console.log(json.stringify(obj, null, 2));
};

var exit = function()
{
	process.exit();
}

var map_insert = function(key, value, map)
{
	if (map[key]) {
		debug("Duplicate " + key);
		exit();
	}

	map[key] = value;
}

var calc_builtin_type_size = function(builtin_type_name)
{
	return parseInt(builtin_type_name.substr('uint'.length, builtin_type_name.indexOf('_')-'uint'.length));
}

var calc_field_fixed_size = function(field, g)
{
	if (field.type) {
		if (field.n) {
			if (isNaN(field.n)) return 0;

			return field.n * calc_builtin_type_size(field.type);
		}

		return calc_builtin_type_size(field.type);
	}

	if (field.user_type) {
		if (field.param) return 0;//case

		return calc_type_fixed_size(field.user_type, g);
	}

	if (field.record) return calc_record_fixed_size(field.record, g);

	if (field.any) return calc_any_fixed_size(field.any, g);

	debug("this should not happen");
}

var calc_record_fixed_size = function(fields, g)
{
	var fixed_size = 0;

	for (var i = 0; i < fields.length; i++) {
		var size = calc_field_fixed_size(fields[i], g);
		if (size > 0) fixed_size += size;
		else break;
	}

	return fixed_size;
}

var calc_any_fixed_size = function(fields, g)
{
	var fixed_size = -1;

	for (var i = 0; i < fields.length; i++) {
		if (fixed_size < 0) {
			fixed_size = calc_field_fixed_size(fields[i], g);

			if (fixed_size == 0) break;

			continue;
		}
		fixed_size = Math.min(calc_field_fixed_size(fields[i], g), fixed_size);

		if (fixed_size == 0) break;
	}

	return fixed_size;
}


var calc_type_fixed_size = function (type_name, g)
{
	var node = g.type[type_name];

	if (!node) {
		debug("No such type " + type_name);
		exit();
	}

	var fixed_size = 0;
	var fields = node.def.fields;
	switch (node.def.subtype) {
		case 'record':
			fixed_size = calc_record_fixed_size(fields, g);
			break;
		case 'any':
			fixed_size = calc_any_fixed_size(fields, g);
			break;
		case 'case':
			break;
	}

	return fixed_size;
}

var parse_request_struct = function (type_name, g)
{
	debug("fixed size of " + type_name + " is " + calc_type_fixed_size(type_name, g));
}

var parse_response_struct = function (type_name, g)
{
	var node = g.type[type_name];

	if (!node) {
		debug("No such type " + type_name);
		exit();
	}

	switch (node.def.subtype) {
		case 'record':
			break;
		case 'any':
			break;
		case 'case':
			break;
	}

}

var parse_flow = function(flow_state, flow, g)
{
	parse_request_struct(flow.request, g);
	parse_response_struct(flow.response, g);
}

var eval = function (ast)
{
	//debug(ast);

	var g = { protocol:ast.name };

	for (var i = 0; i < ast.nodes.length; i++) {
		var node = ast.nodes[i];

		switch(node.type) {
			case 'states':
				g.states = node.names;
				break;
			case 'flow':
				if (!g.flow) g.flow = {};

				if (node.state) {
					map_insert(node.state, node, g.flow);
				} else {
					var value = {};
					map_insert('::ANON::', node, g.flow);
				}
				break;
			case 'type':
				if (!g.type) g.type = {};

				switch (node.def.subtype) {
					case 'record':
						if (!g.record) g.record = {};

						map_insert(node.def.name, node, g.record);
						map_insert(node.def.name, node, g.type);
						break;
					case 'any':
						if (!g.any) g.any = {};

						map_insert(node.def.name, node, g.any);
						map_insert(node.def.name, node, g.type);
						break;
					case 'case':
						if (!g.case) g.case = {};

						map_insert(node.def.name, node, g.case);
						map_insert(node.def.name, node, g.type);

						break;
					default:
						debug("Unkown subtype " + node.def.subtype);
						exit();
				}
				break;
			default:
				debug("Unkown type " + node.type);
				exit();
		}
	}

	debug(g);
	for (var flow_state in g.flow) {
		parse_flow(flow_state, g.flow[flow_state], g);
	}
};

exports.eval = eval;
