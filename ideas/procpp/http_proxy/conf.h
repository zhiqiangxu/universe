#pragma once

class Conf {
  public:
    static constexpr const char* CERT_DIR = "/home/vagrant/ca/cert/";
    static constexpr const char* ROOT_CERT = "/home/vagrant/opensource/try_asio/server.crt";
    static constexpr const char* ROOT_KEY = "/home/vagrant/opensource/try_asio/server.key";
    static constexpr const char* CSR_KEY = "/home/vagrant/opensource/try_asio/csr.key";
    static constexpr const char* OPENSSL_CONFIG = "/home/vagrant/opensource/try_asio/openssl.conf";
};