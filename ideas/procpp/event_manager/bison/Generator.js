var json = require('JSON');

var eval = function (ast)
{
	console.log(json.stringify(ast, null, 2));
};

exports.eval = eval;
