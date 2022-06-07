# execproxy

This simple C proxy allows calling execve based on stdin data. This is useful
when for example using SSH protocol calls to `exec` which takes a sh string, in
order to pass properly separated `argv` and `envp`.

execproxy will first read 4 bytes from stdin and decode it at the actual data
length encoded in big endian. What follows is the actual call data, and starts
with 2 char values for number of args and env variables. What follows are argv
and envp null terminated values.

## Example packet

```
00000000  00 00 00 2f 02 01 2f 62  69 6e 2f 65 63 68 6f 00  |.../../bin/echo.|
00000010  2f 62 69 6e 2f 65 63 68  6f 00 48 65 6c 6c 6f 20  |/bin/echo.Hello |
00000020  77 6f 72 6c 64 21 21 27  22 00 54 45 53 54 3d 66  |world!!'".TEST=f|
00000030  6f 6f 00                                          |oo.|
00000033
```

This call data length is `0x0000002f` bytes (47 bytes), contains 2 argv values
and 1 envp value. First value is `pathname` which needs to be an absolute path
(no path lookup will be performed), then you have the values for argv typically
repeating pathname, and finally envp.
