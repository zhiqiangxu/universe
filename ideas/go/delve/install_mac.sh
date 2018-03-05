openssl req -new -newkey rsa:2048 -x509 -days 3650 -nodes -config dlv-cert.cfg -extensions codesign_reqext -batch -out dlv-cert.cer -keyout dlv-cert.key

sudo security add-trusted-cert -d -r trustRoot -k /Library/Keychains/System.keychain dlv-cert.cer

sudo security import dlv-cert.key -A -k /Library/Keychains/System.keychain
