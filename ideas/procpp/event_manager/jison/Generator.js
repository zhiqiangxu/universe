var json = require('JSON');
var extend = require('util')._extend;


var debug = function (obj)
{
	console.log(json.stringify(obj, null, 2));
};



var eval = function (ast)
{
	debug(ast);
}

exports.eval = eval;
