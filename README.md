cpp-wasm-interpreter-playground

Run a C++ interpreter compiled to WebAssembly directly in your browser.

This project demonstrates a full C++ interpreter with a tokenizer, parser, and AST evaluation, all compiled to WebAssembly (WASM) and exposed in a React-based playground. You can write, run, and reset code snippets directly in the browser—no server required.
Key Features

    C++ Interpreter: A minimal but real interpreter that supports variables, expressions, conditionals, and basic statements.

    WebAssembly Compilation: The C++ code is compiled to WASM via Emscripten, making the interpreter runnable in the browser.

    Browser Playground: A React frontend with a code editor, run and reset buttons, and built-in snippet examples.

    Real-time Output: Immediate evaluation of your code with console-like output.

    Startup-ready Demo: This project showcases how to bridge low-level systems programming with modern front-end deployment.

Getting Started
Clone the repo

git clone [https://github.com/](https://github.com/)JayZenith/cpp-wasm-interpreter-playground.git
cd cpp-wasm-interpreter-playground

Install dependencies

npm install

Run locally

npm start

Open http://localhost:3000 to access the playground.
How It Works
Interpreter

The interpreter tokenizes the input, builds an Abstract Syntax Tree (AST), and evaluates statements using a simple environment. It handles numbers, strings, booleans, conditionals, variable declarations, and assignments.
WebAssembly

The C++ interpreter is compiled to WASM using Emscripten. The WASM module exposes two key functions to JavaScript:

    runCode(code: string): string – executes code and returns the output.

    resetInterpreter() – clears the interpreter's state.

React Playground

The front end loads the WASM module dynamically in the browser, providing an editor and an output console. It also includes several example snippets to help you get started.
Tech Stack

    C++17: Used for the interpreter logic, AST, and parser.

    WebAssembly (WASM): For executing the C++ code directly in the browser.

    Emscripten: The toolchain used to compile C++ to WASM and generate JavaScript bindings.

    React + TypeScript: Used to build the playground's front end.

    npm / Webpack / Create React App: The development environment and build system.

Future Directions

    Add function definitions and loops.

    Improve the editor experience with syntax highlighting and auto-indentation.

    Optimize WASM performance for larger code snippets.

    Package as a standalone static web app for deployment on platforms like GitHub Pages or Vercel.

Why This Project Matters

    For Startups: It demonstrates a strong understanding of systems-level programming, WASM integration, and modern front-end deployment practices.

    For Developer Tooling: It highlights the ability to bridge low-level language implementation with real-world web applications.

    For Bun / JS Runtime Enthusiasts: It showcases knowledge of interpreters, WASM execution, and runtime environment integration.

License

MIT License – feel free to fork and experiment.