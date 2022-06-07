#!/bin/sh

# php > $x = "\x02\x01/bin/echo\0echo\0Hello world!!'\"\0TEST=foo\0";
# php > var_dump(base64_encode(pack('N', strlen($x)).$x));
# string(68) "AAAALwIBL2Jpbi9lY2hvAC9iaW4vZWNobwBIZWxsbyB3b3JsZCEhJyIAVEVTVD1mb28A"

# php > $x = "\x02\x01\0echo\0Hello world!!'\"\0TEST=foo\0";
# php > var_dump(base64_encode(pack('N', strlen($x)).$x));
# string(68) "AAAALwIBL2Jpbi9lY2hvAC9iaW4vZWNobwBIZWxsbyB3b3JsZCEhJyIAVEVTVD1mb28A"

echo 'AAAAIQIBAGVjaG8ASGVsbG8gd29ybGQhISciAFRFU1Q9Zm9vAA==' | base64 -d | ./execproxy
