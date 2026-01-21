zensh 🧘‍♂️

zensh is a small, POSIX-style Unix shell written in C.
It is a learning project to develop a functional interactive shell with history,
pipelines, redirection, quoting, and autocompletion.

---

## FEATURES

### BUILT-IN COMMANDS

- **echo** : print arguments  
- **cd** : change directory (supports `~`, relative & absolute paths)  
- **pwd** : print working directory  
- **type** : identify builtins or executables in `PATH`  
- **history** : view and manage command history  
- **clear** : clear the terminal  
- **exit** : exit the shell  

---

## COMMAND HISTORY (GNU READLINE)

- Persistent history across sessions  
- Arrow-key navigation  
- Reverse search  
- History saved automatically on exit  
- History file configurable via `HISTFILE` environment variable  

**History commands:**  
- `history`  
- `history N`  
- `history -r <file>` read & append history from file  
- `history -w <file>` write history to file  
- `history -a <file>` append current session to file  

---

## PIPELINES

zensh supports Unix-style pipelines using the `|` operator.
The output of one command is passed as input to the next.

**Examples:**  
- `cat file.txt | grep foo | wc -l`  
- `printf "a\nb\n" | grep a | wc -l`  
- `type ls | wc -l`

**Notes:**  
- Each stage runs in its own process  
- Pipes created using `pipe()`  
- File descriptors connected using `dup2()`  
- Invalid pipelines like `"cd | wc"` are rejected  

---

## INPUT / OUTPUT REDIRECTION

**Supported operators:**  
- `>` redirect stdout (overwrite)  
- `>>` redirect stdout (append)  
- `1>` explicit stdout  
- `2>` redirect stderr  
- `2>>` append stderr  

**Examples:**  
- `echo hello > out.txt`  
- `echo world >> out.txt`  
- `ls nonexistent 2> err.txt`  
- `ls | wc -l > count.txt`

**Redirection works for:**  
- Builtins  
- External commands  
- Pipeline endpoints  

---

## QUOTING & ESCAPING

zensh correctly handles:  
- Single quotes: `'...'`  
- Double quotes: `"..."` (with escapes)  
- Backslash escapes outside quotes  
- Concatenated quoted strings  

**Examples:**  
- `echo "hello""world"`  
- `echo 'hello "world"'`  
- `echo world\ \ test`  
- `echo raspberry\\nblueberry`

Behavior closely matches Bash.

---

## AUTOCOMPLETION

Powered by GNU Readline:  
- Builtin command completion  
- Executable completion from `PATH`  
- File and directory completion  
- Context-aware behavior  

**Examples:**  
- `ec<TAB>` → `echo`  
- `ca<TAB>` → `cat`  
- `/usr/bi<TAB>` → `/usr/bin/`  

---

## EXTERNAL COMMAND EXECUTION

- Executes programs found in `PATH`  
- Uses `fork()`, `execvp()`, and `wait()`  
- Clear error messages for invalid commands  

**Example:**  
```
invalid_command: command not found
```

---

## BUILD INSTRUCTIONS

**Dependencies:**  
- GCC or Clang  
- CMake >= 3.13  
- GNU Readline  

**Build steps:**  
```bash
git clone https://github.com/ChethanJS07/zensh.git
cd zensh
mkdir build && cd build
cmake ..
make
```

**Run:**  
```bash
./zensh
```

---

## PROJECT STRUCTURE

```
src/
|-- main.c          REPL loop
|-- tokenize.c      Lexer / tokenizer
|-- exec.c          External command execution
|-- pipeline.c      Pipeline handling
|-- redir.c         I/O redirection
|-- builtins.c      Built-in commands
|-- autocomplete.c  Readline completion
|-- utils.c         Helper functions
`-- zensh.h         Shared declarations
```

---

## LICENSE

MIT License

---

## AUTHOR

Built by Chethan
