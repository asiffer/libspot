// see https://squidfunk.github.io/mkdocs-material/reference/math/#katex
document$.subscribe(({ body }) => { 
    renderMathInElement(body, {
      macros: {
        "\\RR": "\\mathbb{R}",
        "\\KK" : "\\operatorname*{\\LARGE K}",
    },
      delimiters: [
        { left: "$$",  right: "$$",  display: true },
        { left: "$",   right: "$",   display: false },
        { left: "\\(", right: "\\)", display: false },
        { left: "\\[", right: "\\]", display: true }
      ],
    })
  })
  