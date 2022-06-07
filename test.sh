#!/bin/sh

# php > $x = "\x02\x01/bin/echo\0echo\0Hello world!!'\"\0TEST=foo\0";
# php > var_dump(base64_encode(pack('N', strlen($x)).$x));
# string(68) "AAAALwIBL2Jpbi9lY2hvAC9iaW4vZWNobwBIZWxsbyB3b3JsZCEhJyIAVEVTVD1mb28A"

# php > $x = "\x02\x01\0echo\0Hello world!!'\"\0TEST=foo\0";
# php > var_dump(base64_encode(pack('N', strlen($x)).$x));
# string(68) "AAAALwIBL2Jpbi9lY2hvAC9iaW4vZWNobwBIZWxsbyB3b3JsZCEhJyIAVEVTVD1mb28A"

# php > $x = "\x03\x01/bin/bash\0sh\0-c\0pwd\0PWD=/tmp\0";
# php >  var_dump(base64_encode(pack('N', strlen($x)).$x));
# string(48) "AAAAHwMBL2Jpbi9iYXNoAHNoAC1jAHB3ZABQV0Q9L3RtcAA="

echo 'AAAAHwMBL2Jpbi9iYXNoAHNoAC1jAHB3ZABQV0Q9L3RtcAA=' | base64 -d | ./execproxy
