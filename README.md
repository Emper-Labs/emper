# Emper

Emper is an open-source simulation engine for scientific computing.

The project aims to provide a foundation for building simulations across different hardware platforms, with a focus on data-oriented storage, memory management, and extensible backends.

## Goals

- CPU and GPU computing.
- Data-oriented memory and storage.
- Modular simulation architecture.
- Extensible hardware backends.

## Project Structure

- `core/` — Core engine infrastructure.
- `test/` — Core tests.

## Build

**Requirements:** CMake and a C++ compiler.

```bash
cmake -S . -B build -G Ninja
cmake --build build
```

## Tests

Run the test executable :)) :

```bash
./build/test/executable_file
```

## Status

Emper is under active development. APIs and architecture are subject to change.
