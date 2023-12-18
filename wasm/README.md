# libspot.js

This sub-project ports `libspot` into javascript through webassembly (emscripten).
It uses `bun` to build and test the final package.

## Core library

The core library, namely `libspot.core.js`, is generated from the root project.

```
# from project root directory
# it generates ./wasm/libspot.core.js
make ./wasm/libspot.core.js
```

## Interface

The `index.ts` interface wraps the generated library and provides a OO-like and more dev-friendly API.

## Test

Tests are rather minimal for the moment. See `libspot.test.ts`.

```bash
bun test
```

## Building

See `package.json` for the script definition.

```bash
bun run build
```
