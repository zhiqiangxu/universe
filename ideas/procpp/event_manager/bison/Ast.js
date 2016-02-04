
Function.prototype.setParent = function( parent ){
	this.parent = parent;
}

Function.prototype.getParent = function( ){
	return this.parent;
}



function Protocol(name, nodes)
{
	this.name = name;
	this.nodes = nodes;

}


function Record(fields, name)
{
	this.fields = fields;
	this.name = name;

}

function Any(fields, name)
{
	this.fields = fields;
	this.name = name;

}

function Case(name, params, cases)
{
	this.name = name;
	this.params = params;
	this.cases = cases;
}

function Field(field)
{
	this.toJSON = function()
	{
		return field;
	}

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

	TypeField.inheritsFrom(this);

	var UserTypeField = function (field)
	{
		this.calc_fixed_size = function ()
		{
			if (field.param) return 0;//case

			
			return common.calc_type_fixed_size(field.user_type);
		}
	}

	UserTypeField.inheritsFrom(this);

	var RecordField = function (field)
	{
		this.calc_fixed_size = function ()
		{
			return common.calc_record_fixed_size(field.record);
		}
	}

	RecordField.inheritsFrom(this);

	var AnyField = function (field)
	{
		this.calc_fixed_size = function ()
		{
			return common.calc_any_fixed_size(field.any);
		}
	}

	AnyField.inheritsFrom(this);

	if (field.type) return new TypeField(field);

	if (field.user_type) return new UserTypeField(field);

	if (field.record) return RecordField(field);

	if (field.any) return AnyField(field);

}

