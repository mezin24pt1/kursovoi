#include "crypto.h"

string auth(string salt, string pass){

CPP::SHA224 sha224;
string hash;
CPP::StringSource(
    string(salt).append(pass), 
    true,
        new CPP::HashFilter(
            sha224,
            new CPP::HexEncoder(
                new CPP::StringSink(hash)))
);

return hash;
}
