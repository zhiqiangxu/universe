var g = {};

var calc_builtin_type_size = function(builtin_type_name)
{
	return parseInt(builtin_type_name.substr('uint'.length, builtin_type_name.indexOf('_')-'uint'.length)) / 8;//in bytes
}

var calc_type_fixed_size = function (type_name)
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
			fixed_size = calc_record_fixed_size(fields);
			break;
		case 'any':
			fixed_size = calc_any_fixed_size(fields);
			break;
		case 'case':
			break;
	}

	return fixed_size;
}

var calc_record_fixed_size = function(fields)
{
	var fixed_size = 0;

	for (var i = 0; i < fields.length; i++) {
		var size = calc_field_fixed_size(fields[i]);
		if (size > 0) fixed_size += size;
		else break;
	}

	return fixed_size;
}

var calc_any_fixed_size = function(fields)
{
	var fixed_size = -1;

	for (var i = 0; i < fields.length; i++) {
		if (fixed_size < 0) {
			fixed_size = calc_field_fixed_size(fields[i]);

			if (fixed_size == 0) break;

			continue;
		}
		fixed_size = Math.min(calc_field_fixed_size(fields[i]), fixed_size);

		if (fixed_size == 0) break;
	}

	return fixed_size;
}

var calc_field_fixed_size = function(field)
{
	return field_to_class(field).calc_fixed_size();
}

var field_to_class = function(field)
{
	var field_class = require('./Field');//to break cyclic depends

	if (field.type) return new field_class.TypeField(field);

	if (field.user_type) return new field_class.UserTypeField(field);

	if (field.record) return field_class.RecordField(field);

	if (field.any) return field_class.AnyField(field);

	debug('bug happend');
}

var calc_field_n = function(field)
{
	return field_to_class(field).calc_n();
}

exports.calc_builtin_type_size = calc_builtin_type_size;
exports.calc_type_fixed_size = calc_type_fixed_size;
exports.calc_record_fixed_size = calc_record_fixed_size;
exports.calc_any_fixed_size = calc_any_fixed_size;
exports.calc_field_fixed_size = calc_field_fixed_size;
exports.calc_field_n = calc_field_n;
exports.g = g;
