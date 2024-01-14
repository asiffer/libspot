---
title: Javascript
icon: material/language-javascript
---

**libspot** has also been ported to the browser (and more generally to the `js` ecosystem) through [webassembly :simple-webassembly:](https://developer.mozilla.org/en-US/docs/WebAssembly/Concepts) thanks to [emscripten](https://emscripten.org/).

## Install

You can get the library through `npm` (or other package manager).

<!-- prettier-ignore -->
=== "npm"

    ```shell
    npm install libspot
    ```

=== "yarn"

    ```shell
    yarn add libspot
    ```

You then get a typescript library that wraps the webassembly code.

## Get started

<!-- prettier-ignore -->
```js
--8<-- "examples/basic.js"
```
