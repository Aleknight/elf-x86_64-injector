# Injector

Injects a x86_64 bytecode into an ELF x86_64 binary

## Usage

```sh
./injector <binary-to-infect> <payload-to-inject> <function-to-backdoor>
```

### Compilation

```sh
make
```

## Description

This program add a bytecode in the _.text_ section of the target binary.  
It changes also the got sections in order to call the bytecode instead of the resolver.  
