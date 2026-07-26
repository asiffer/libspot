---
title: Javascript
summary: libspot in the browser
---

**libspot** has also been ported to the browser (and more generally to the `js` ecosystem) through [webassembly +simple-icons:webassembly+](https://developer.mozilla.org/en-US/docs/WebAssembly/Concepts) thanks to `clang` & `wasm-ld` (see [this example](https://log.schemescape.com/posts/webassembly/trivial-example.html)).

## Install

You can get the library through `npm` (or other package manager).


/// tab | npm

    :::shell
    npm install libspot

///


/// tab | yarn

    :::shell
    yarn add libspot

///

/// tab | bun

    :::shell
    bun add libspot

///

You then get a typescript library that wraps the webassembly code.

## Get started

<!-- prettier-ignore -->
```js
--8<-- "examples/basic.js"
```

/// codexec

    :::typescript
    import spot from "https://esm.sh/libspot@latest"

    console.log(spot);

///