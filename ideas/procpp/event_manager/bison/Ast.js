function TypeNode(){

	this.set_parent = function( parent ){
		this.parent = parent;
	}

	this.get_parent = function( ){
		return this.parent;
	}

	this.get_result_class = function() {
	}

	this.get_ntoh = function(size, str) {
		switch (size) {
			case 1:
				return str;
			case 2:
				return 'ntohs(' + str + ')'
			case 4:
				return 'ntohl(' + str + ')';
			default:
				console.log("no ntoh for size " + size);
		}
	}
}


function Protocol(name, nodes)
{
	this.name = name;
	this.nodes = nodes;

}


var ID = 0;

function Record(ob)
{
	for (k in obj) this[k] = obj[k];

	this.id = ID++;

	this.expand_fields = function() {

		var fields = [];
		for (var i = 0; i < this.fields.length; i++) {
			var field = this.fields[i];

			fields = fields.concat(field.expand_fields());

		}

		return fields;

	}

	this.on_message = function() {

		var ResultClass = this.get_result_class();

		var fields = this.expand_fields();

		var cpp = "";

		for (var i = 0; i < fields.length; i++) {
			cpp += "const size_t offset_" + i + " = " + ( i > 0 ? "offset_" + (i-1) + ' + size_' + (i-1) : '0' ) + ";\n";

			var field = fields[i];

			if (field.type) {
				var field_size = Common.calc_builtin_type_size(field.type);
				cpp += "const size_t size_" + i + " = " + field_size + ";\n";//TODO handle n
				cpp += "if (message.length() < offset_" + i + " + size_" + i + ") goto AGAIN;\n";
				cpp += "const " + field.type + " value_" + i + \
					   " = " + this.get_ntoh(field_size, "*reinterpret_cast<*" + field.type">(message.substr(offset_" + i + ", size_" + i + ").c_str())") + ";\n";
				if ('value' in field) {
					cpp += "if (value_" + i + " != " + field.value + ") goto NG;\n";
				}
			}

			if (field.user_type) return new UserTypeField(field);

			if (field.record) return RecordField(field);

			if (field.any) return AnyField(field);


			cpp += "switch(" + fields[i].on_message() + ") {\n";
			cpp += "	case ParseResult::AGAIN:\n";
			cpp += "		goto AGAIN;\n";
			cpp += "	case ParseResult::OK:\n";
			cpp += "		break;\n";
			cpp += "	case ParseResult::NG:\n";
			cpp += "		goto NG;\n";
			cpp += "}\n";
			cpp += "\n";

		}

		cpp += "OK:\n"
		cpp += "	on_ok(client, r);\n"
		cpp += "	return;";
		cpp += "NG:\n";
		cpp += "	erase_buf(client);\n";
		cpp += "	on_error(client);\n";
		cpp += "	return;";
		cpp += "AGAIN:\n";
		cpp += "	on_again(client);\n";
		cpp += "	return;";

	}

}

Record.inheritsFrom(TypeNode);

function Any(fields, name)
{
	this.fields = fields;
	this.name = name;
	this.index = -1;
	this.id = ID++;

	this.on_message = function() {
		for (var i = 0; i < this.fields.length; i++) {
		}
	}

	this.get_size = function(){
		return this.fields[this.index].get_size();
	}

}

Any.inheritsFrom(TypeNode);

function Case(name, params, cases)
{
	this.name = name;
	this.params = params;
	this.cases = cases;
	this.index = -1;

	this.get_size = function(){
		return this.cases[this.index].get_size();
	}
}

Case.inheritsFrom(TypeNode);

function Field(field)
{
	this.toJSON = function()
	{
		return field;
	}

	var TypeField = function (field)
	{
	}

	TypeField.inheritsFrom(this);

	var UserTypeField = function (field)
	{
	}

	UserTypeField.inheritsFrom(this);

	var RecordField = function (field)
	{
	}

	RecordField.inheritsFrom(this);

	var AnyField = function (field)
	{
	}

	AnyField.inheritsFrom(this);

	if (field.type) return new TypeField(field);

	if (field.user_type) return new UserTypeField(field);

	if (field.record) return RecordField(field);

	if (field.any) return AnyField(field);

}

Field.inheritsFrom(TypeNode);
