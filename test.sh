#!/bin/sh

# php > $x = "\x02\x01/bin/echo\0echo\0Hello world!!'\"\0TEST=foo\0";
# php > var_dump(base64_encode(pack('N', strlen($x)).$x));
# string(68) "AAAALwIBL2Jpbi9lY2hvAC9iaW4vZWNobwBIZWxsbyB3b3JsZCEhJyIAVEVTVD1mb28A"

echo 'AAAALwIBL2Jpbi9lY2hvAC9iaW4vZWNobwBIZWxsbyB3b3JsZCEhJyIAVEVTVD1mb28A' | base64 -d | ./execproxy
