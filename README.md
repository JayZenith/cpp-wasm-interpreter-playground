# cpp-wasm-interpreter-playground

Run a **C++ interpreter compiled to WebAssembly** directly in your browser.

This project demonstrates a minimal but complete C++ interpreter—including tokenizer, parser, and AST evaluation—compiled to WebAssembly (WASM) and exposed in a React-based playground. You can **write, run, and reset C++ code snippets instantly in the browser**, with no server required.

---

## 🚀 Key Features

- **C++ Interpreter**  
  A minimal but real interpreter supporting variables, expressions, conditionals, and basic statements.

- **WebAssembly Compilation**  
  The interpreter is written in C++17 and compiled to WASM via **Emscripten**, making it runnable in any modern browser.

- **Browser Playground**  
  A React frontend with a built-in editor, run/reset buttons, and example snippets.

- **Real-Time Output**  
  Immediate code evaluation with a console-like output area.

- **Startup-Ready Demo**  
  Demonstrates how to bridge low-level systems programming with modern front-end deployment.

---

## 📦 Getting Started

Clone the repo:
```bash
git clone https://github.com/JayZenith/cpp-wasm-interpreter-playground.git
cd cpp-wasm-interpreter-playground
