#include <iostream>
#include <unistd.h>//sleep
#include "cert.h"
#include "utils.h"
#include "conf.h"

using std::cout;
using std::endl;

//csr : openssl req -key domain.key -new -out domain.csr
//sign: openssl x509 -req -in domain.csr -CA rootCA.pem -CAkey rootCA.key -CAcreateserial -out domain.crt -days 500 -sha256
// X 这样的方式必须下一秒才valid
//sign: openssl ca -config openssl.conf -batch -md sha1 -days 365 -startdate YYMMDDHHMMSSZ -cert server.crt -keyfile server.key -in in.csr -out out.crt

// https://www.dynacont.net/documentation/linux/openssl/
// https://datacenteroverlords.com/2012/03/01/creating-your-own-ssl-certificate-authority/
// https://jamielinux.com/docs/openssl-certificate-authority/create-the-root-pair.html
string Cert::cert_for_domain(const string& domain, const string& csr_key, const string& ca_cert, const string& ca_key, bool& ok) {
  auto cert_path = Conf::CERT_DIR + domain + ".crt";
  if (Utils::file_exists(cert_path)) {
    ok = true;
    return cert_path;
  }

  auto t = std::time(nullptr);
  t -= 24*3600;
  auto past_time = Utils::time_format("%y%m%d%H%M%SZ"/*strftime*/, &t);
  auto csr_path = Conf::CERT_DIR + domain + ".csr";
  auto subject = "\"/C=CN/ST=Shanghai/L=ZhangJiang/O=ffan/CN=" + domain + "\"";
  string cmd = "openssl req -key " + csr_key + " -subj " + subject + " -new -out " + csr_path + " && \
                openssl ca -config " + Conf::OPENSSL_CONFIG + " -batch -md sha1 -days 365 -startdate " + past_time + " -cert " + ca_cert + " -keyfile " + ca_key +
                " -in " + csr_path + " -out " + cert_path;

  cout << cmd << endl;

  if (system(cmd.c_str()) == 0) {
    ok = true;
    cout << "cert = " << cert_path << endl;
    return cert_path;
  } else {
    Utils::rm_file(cert_path.c_str());
    Utils::rm_file(csr_path.c_str());
    ok = false;
    return "";
  }
}
