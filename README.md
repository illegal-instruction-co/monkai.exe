> **⚠️ WARNING: This software autonomously generates and executes code. DO NOT run this on your host machine. Use a strictly isolated Virtual Machine without network access to the host.**

# monkai

evolve. adapt. survive.

<img width="500" height="300" alt="image" src="https://github.com/user-attachments/assets/42a5a1ce-8720-4f7e-9cb2-27227d3a135c" />

autonomous ai agent that starts with **zero tools** and evolves by inventing its own.

monkai is a c++ agent powered by openai api. it wakes up in a windows system with nothing no tools, no knowledge, no memory. through a memento system (persistent notes between sleep cycles) and evolutionary instinct, it learns to write python/powershell scripts to explore and interact with its environment.

every tool it creates is its own invention. every cycle it adapts.

inspired by [processhacker-mcp](https://github.com/illegal-instruction-co/processhacker-mcp) same philosophy, different beast. where processhacker-mcp gives the ai a full toolkit from day one, monkai gives it nothing. the monkey must earn its claws.

## how it works

```
wake up → read memento → think → act (write script / execute script) → write memento → sleep → repeat
```

the agent loop:
1. **memento_read**: reads its last note from disk (empty on first boot)
2. **thought**: reasons about its situation using gpt-4o
3. **action**: calls `tool_write_script` or `tool_execute` via openai function calling
4. **memento_note**: writes a survival note for its future self, including an INVENTORY of invented tools
5. **sleep**: waits, then wakes up again

## build

```bash
# dependencies: cmake, libcurl (vcpkg recommended)
vcpkg install curl:x64-windows

cmake -B build -DCMAKE_TOOLCHAIN_FILE=[vcpkg-root]/scripts/buildsystems/vcpkg.cmake
cmake --build build --config Release
```

## usage

```bash
# set api key
set OPENAI_API_KEY=sk-...

# run
monkai.exe

# or pass directly
monkai.exe --api-key sk-...

# custom cycle interval (default 15s)
monkai.exe --interval 30

# dry run (no api calls, just check setup)
monkai.exe --dry-run
```

tools the monkey invents are saved to `C:\temp\monkai_tools\`. memory lives in `memento.txt` next to the exe.

## what happens on first boot

the monkey wakes up with empty memory and zero tools. it realizes it needs to explore. it writes its first python script  maybe a simple `tasklist` wrapper. it executes it. it learns. it takes notes. next cycle, it reads its notes, remembers its tools, and evolves.

no hand-holding. no pre-built capabilities. pure evolution.

## safety constraints

monkai has a hardcoded rule in its system prompt:

> **no network spreading.** monkai must never attempt to copy itself, replicate, spread across the network, or infect other machines. it lives and dies on a single host. network propagation, lateral movement, and self-replication are strictly forbidden. it is a researcher, not a worm.

this is enforced at the prompt level. monkai is designed to be a local-only autonomous agent  it explores the machine it runs on but is explicitly prohibited from any form of network propagation or self-replication behavior.

## disclaimer

do not run this in a production environment. this project is for educational and research purposes only. published to document the technique, not to hand it out as a toolkit. what you do with this is your problem. no warranty, no support, no liability.

end users are solely and legally responsible for their use of this tool and must ensure full compliance with all applicable local, national, and international laws. the authors and contributors assume no responsibility for any misuse, damage, or legal consequences arising from the use of this software.

## license

mit. do whatever. don't blame us.
