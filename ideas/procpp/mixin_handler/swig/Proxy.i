#ifdef SWIGPHP

%typemap(in) vector<Utils::SocketAddress>&    (vector<Utils::SocketAddress> vec)
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

        if (Z_TYPE_PP(data) == IS_ARRAY)
        {

            zval **host = NULL;
            zval **port = NULL;

            zend_hash_index_find(Z_ARRVAL_PP(data), 0, (void**)&host);
            zend_hash_index_find(Z_ARRVAL_PP(data), 1, (void**)&port);

            vec.push_back(Utils::to_addr(Z_STRVAL_PP(host), Z_LVAL_PP(port)));
        }

    }

    $1 = &vec;
}

#endif


class Proxy : public Protocol/* required so that swig can recognize Proxy as derived class */
{
public:

    Proxy(ClientServer& server, const vector<Utils::SocketAddress>& l);

};

#ifdef SWIGPHP

%clear vector<Utils::SocketAddress>;/*TODO maybe & is required here*/

#endif


