# Memrescue
Memrescue keeps your Linux desktop fast and responsive by proactively killing programs using too much memory and clearing swap/caches as needed.

## Installation
```sh
$ make
$ sudo make install
```

## Set up with systemctl
```sh
$ sudo make start
```

## Update systemctl after recompile
```sh
$ sudo make update
```

## Command usage
```sh
$ sudo memrescue
```

## Configuring Memrescue
You can edit any functionality in config.hpp, however you will need to compile Memrescue again.
