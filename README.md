# CLIENT

## PURPOSE
HTTP/1.0 client, only support GET method, fetch the data from a url, can optionally local saved.(Doesn't support get query, nor doing url validation)

## USAGE

```Makefile
# build
make client

# run (after build)
# second argument could be 'y' | '1' | 'n' | '0'
# 'y' or '1' for locally saved, 'n' or '0' print to terminal
./bin/httpclient http://x.x.x.x/Foo/bar y

# clean
make clean
```
