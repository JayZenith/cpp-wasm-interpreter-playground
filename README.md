# cpp-wasm-interpreter-playground

Run a **C++ interpreter compiled to WebAssembly** directly in your browser.

This project demonstrates a minimal but complete C++ interpreter—including tokenizer, parser, and AST evaluation—compiled to WebAssembly (WASM) and exposed in a React-based playground. You can **write, run, and reset C++ code snippets instantly in the browser**, with no server required.
---

## Demo

Try the live demo: [cpp-wasm-interpreter-playground.vercel.app/](https://cpp-wasm-interpreter-playground.vercel.app/)

## Key Features

- **C++ Interpreter**  
  A minimal but real interpreter supporting variables, expressions, and basic statements.

- **WebAssembly Compilation**  
  The interpreter is written in C++17 and compiled to WASM via **Emscripten**, making it runnable in any modern browser.

- **Browser Playground**  
  A React frontend with a built-in editor, run/reset buttons, and example snippets.

- **Real-Time Output**  
  Immediate code evaluation with a console-like output area.

---

## Getting Started
Clone the repo:

```bash
git clone https://github.com/JayZenith/cpp-wasm-interpreter-playground.git
cd cpp-wasm-interpreter-playground

```

## Install Dependencies
npm install


## Run Locally
npm start


📝 How It Works

Lexer/Tokenizer
Breaks C++ code into tokens.

Parser
Converts tokens into an Abstract Syntax Tree (AST) representing expressions and statements.

Interpreter
Walks the AST and evaluates statements and expressions inside an Environment, which tracks variable values.

Output
The interpreter writes results to a console-like area or returns them via WASM bindings for the browser.


Programs You Can Run

The interpreter supports:

Variable declarations:
```bash
let x = 10;
let y = 5;
```

Arithmetic operations:
```bash
let z = 3 + 4 * 2
```


Print statements:
```bash
print("Hello, WebAssembly!");
print(x + z);

```


⚡ Resetting the Interpreter
- In the browser, click Reset to clear all variables and start fresh.


Development Notes
- Written in C++17.
- Compiled to WebAssembly using Emscripten.
- Frontend is React .
- Output normalization handles whitespace differences for reliable testing.

Summary
This project lets you experiment with systems programming, interpreters, and C++ logic, all running directly in your browser. You can extend it to add conditionals, loops, functions, or more complex data types in the future.