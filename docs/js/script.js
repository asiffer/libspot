if (document.URL.includes("#")) {
  let s = document.URL.split("#");
  let id = s[s.length - 1];
  let anchor = document.getElementById(id);
  if (anchor) {
    let found = false;
    let siblings = new Array(...anchor.parentElement.children);
    console.log("siblings:", siblings);
    let i = siblings.findIndex((e) => anchor.isSameNode(e));
    console.log(i, siblings[i + 1].tagName);
    if (i >= 0 && siblings[i + 1].tagName === "DETAILS") {
      new Array(...document.getElementsByTagName("details")).map((e) =>
        e.removeAttribute("open")
      );
      siblings[i + 1].setAttribute("open", "");
    }
  }
}
