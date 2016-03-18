var common = require('./Common');


var TypeField = function (field)
{
	this.calc_fixed_size = function ()
	{
		if (field.n) {
			if (isNaN(field.n)) return 0;

			return field.n * common.calc_builtin_type_size(field.type);
		}

		return common.calc_builtin_type_size(field.type);

	}

	this.calc_n = function ()
	{
		if (field.n) {
			if (isNaN(field.n)) return 0;

			return field.n;
		}

		return 1;
	}

}

var UserTypeField = function (field)
{
	this.calc_fixed_size = function ()
	{
		if (field.param) return 0;//case

		
		return common.calc_type_fixed_size(field.user_type);
	}
}

var RecordField = function (field)
{
	this.calc_fixed_size = function ()
	{
		return common.calc_record_fixed_size(field.record);
	}
}

var AnyField = function (field)
{
	this.calc_fixed_size = function ()
	{
		return common.calc_any_fixed_size(field.any);
	}
}

exports.TypeField = TypeField;
exports.UserTypeField = UserTypeField;
exports.RecordField = RecordField;
exports.AnyField = AnyField;
