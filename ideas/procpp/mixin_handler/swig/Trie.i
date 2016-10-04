#ifdef SWIGPHP

%typemap(out) vector<string>
{
    array_init(return_value);

    for (size_t i = 0; i < $1.size(); i++) {
        add_next_index_string(return_value, $1.at(i).c_str(), 1);
    }
}

%typemap(in) vector<string>&    (vector<string> vec)
{
    zval** data;
    HashTable* hash;
    HashPosition ptr;

    hash = Z_ARRVAL_PP($input);

    for (
        zend_hash_internal_pointer_reset_ex(hash, &ptr);
        zend_hash_get_current_data_ex(hash, (void**)&data, &ptr) == SUCCESS;
        zend_hash_move_forward_ex(hash, &ptr)
    )
    {
        zval temp, *str;

        bool is_str = true;

        if (Z_TYPE_PP(data) != IS_STRING)
        {
            temp = **data;
            zval_copy_ctor(&temp);
            convert_to_string(&temp);
            str = &temp;
            is_str = false;
        }
        else str = *data;

        vec.push_back(Z_STRVAL_P(str));

        if (!is_str) zval_dtor(&temp);
    }

    $1 = &vec;
}

#endif

class Trie
{
public:
    Trie(const vector<string>& keywords);

    vector<string> search(const string& text, bool no_overlap = true);

};

#ifdef SWIGPHP

%clear vector<string>;

#endif

