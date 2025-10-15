---
title: Javascript
icon: material/language-javascript
---

**libspot** has also been ported to the browser (and more generally to the `js` ecosystem) through [webassembly +simple-icons:webassembly+](https://developer.mozilla.org/en-US/docs/WebAssembly/Concepts) thanks to [emscripten](https://emscripten.org/).

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
    import spot from "https://esm.sh/libspot@2.0.0-beta.4"

    console.log(spot);

///