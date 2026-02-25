> **⚠️ WARNING: This software autonomously generates and executes code. DO NOT run this on your host machine. Use a strictly isolated Virtual Machine without network access to the host.**

# monkai

evolve. adapt. survive.

<img width="500" height="300" alt="image" src="https://github.com/user-attachments/assets/42a5a1ce-8720-4f7e-9cb2-27227d3a135c" />

autonomous ai agent that starts with zero tools and writes its own.

monkai is a c++ agent powered by openai's function calling api. it wakes up inside a windows system with nothing, no tools, no knowledge, no memory of previous runs. using a persistent note system (memento), it remembers what it learned between sleep cycles and builds on it. over time, it writes python and powershell scripts to explore its environment.

inspired by [processhacker-mcp](https://github.com/illegal-instruction-co/processhacker-mcp). where that project gives the ai a full toolkit from the start, monkai gives it nothing and lets it figure things out.

## how it works

```
wake up → read memento → think → act → write memento → sleep → repeat
```

each cycle:
1. **memento_read**, reads its last note from disk (empty on first boot)
2. **thought**, reasons about its situation using gpt-4o
3. **action**, calls `tool_write_script` or `tool_execute` via function calling
4. **memento_note**, writes a note for the next cycle, including a list of tools it has created so far
5. **sleep**, waits, then starts over

<img width="800" height="400" alt="image" src="https://github.com/user-attachments/assets/e759329c-7c9b-4f66-acda-79fa1a07064b" />

## build

dependencies are fetched automatically via cmake (libcurl, nlohmann/json).

```bash
cmake -B build
cmake --build build --config Release
```

## usage

```bash
set OPENAI_API_KEY=sk-...
monkai.exe

# or pass directly
monkai.exe --api-key sk-...

# custom cycle interval (default 15s)
monkai.exe --interval 30

# dry run, no api calls, just check setup
monkai.exe --dry-run
```

invented scripts are saved to `C:\temp\monkai_tools\`. memory lives in `memento.txt` next to the executable.

## first boot

the agent wakes up with empty memory and no tools. it typically writes a simple script (e.g. a `tasklist` wrapper), executes it, observes the output, and takes notes. next cycle it reads those notes, remembers what it built, and continues from there.

## safety

monkai has a hardcoded rule in its system prompt: **no network spreading.** it must not copy itself, replicate, or move laterally. it runs on one machine only. this is enforced at the prompt level, it is not a technical sandbox.

## disclaimer

this project is for educational and research purposes. it is published to document the technique. end users are solely responsible for compliance with all applicable laws. no warranty, no support, no liability.

## license

mit
