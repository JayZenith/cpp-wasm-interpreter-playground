import React, { useState, useEffect } from "react";

// Example snippets
const SNIPPETS = [
  `let x = 5;
print(x + 2);`,
  `let counter = 0;
counter = counter + 1;
print(counter);  
`
];

export const PlaygroundWrapper = () => {
  const [ready, setReady] = useState(false);
  const [module, setModule] = useState<any>(null);
  const [code, setCode] = useState("");
  const [output, setOutput] = useState("");

  useEffect(() => {
    const script = document.createElement("script");
    script.src = "/interpreter.js";
    script.onload = async () => {
      if ((window as any).createInterpreterModule) {
        const mod = await (window as any).createInterpreterModule();
        setModule(mod);
        setReady(true);
      }
    };
    document.body.appendChild(script);
  }, []);

  const run = () => {
    if (!ready || !module) return;
    try {
      const result = module.runCode(code);
      setOutput(result);
    } catch (e: any) {
      setOutput("Error: " + e);
    }
  };

  const reset = () => {
    if (!ready || !module) return;
    module.resetInterpreter();
    setOutput("");
  };

  return (
    <div style={{ fontFamily: "sans-serif", padding: 20 }}>
      <h1>WASM Interpreter Playground</h1>
      <div>
        <select
          onChange={(e) => setCode(SNIPPETS[parseInt(e.target.value)])}
          defaultValue=""
        >
          <option value="" disabled>
            Select Example
          </option>
          {SNIPPETS.map((s, i) => (
            <option key={i} value={i}>
              Example {i + 1}
            </option>
          ))}
        </select>
      </div>
      <textarea
        value={code}
        onChange={(e) => setCode(e.target.value)}
        rows={10}
        cols={50}
        style={{ display: "block", marginTop: 10 }}
      />
      <button onClick={run} disabled={!ready} style={{ marginRight: 10 }}>
        Run
      </button>
      <button onClick={reset} disabled={!ready}>
        Reset
      </button>
      <pre
        style={{
          marginTop: 20,
          backgroundColor: "#f0f0f0",
          padding: 10,
          minHeight: 50,
        }}
      >
        {output}
      </pre>
    </div>
  );
};
