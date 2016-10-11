# team8-client
The team8 client create on Camunda-Hackdays 2016



# CUnit Tests

## Install CUnit

Download the CUnit-2.1-2-src.tar.bz2 source from the CUnit files page and save to temporary directory.

### Unpack

```sh
$ cd /tmp
$ bunzip2 CUnit-2.1-2-src.tar.bz2
$ tar -xvf CUnit-2.1-2-src.tar
```

### Configuration

```sh
$ cd CUnit-2.1-2
$ ./configure --prefix=/usr
$ make
$ sudo make install
```

How to install CUnit for Netbeans see [documentation](https://netbeans.org/kb/docs/cnd/c-unit-test.html).

## Run Tests

You can use either the `test` script or use make.

Simply execute the following commands to build and execute the test cases:

```sh
$ make build-test
$ make test
```
