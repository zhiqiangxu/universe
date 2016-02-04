var json = require('JSON');
var swig = require('swig');
var common = require('./Common');
var extend = require('util')._extend;


var locals = {};
locals = extend(common, locals);

var g = common.g;

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

var parse_field = function (field)
{
	return swig.render('templates/field.cpp', { locals: extend({ field : field }, locals) });
}

var parse_struct = function (flow_state, type_name, end)
{
	debug("fixed size of " + type_name + " is " + common.calc_type_fixed_size(type_name));

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

var parse_flow = function(flow_state, flow)
{
	parse_struct(flow_state, flow.request, 'Server');
	parse_struct(flow_state, flow.response, 'Client');
}

var eval = function (ast)
{
	//debug(ast);

	g.protocol = ast.name;
	locals.g = g;

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
		parse_flow(flow_state, g.flow[flow_state]);
	}
}

exports.eval = eval;
