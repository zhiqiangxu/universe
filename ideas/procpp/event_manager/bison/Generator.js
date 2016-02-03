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

var parse_request_struct = function (type_name, g)
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
