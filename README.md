# 🐧 PenguinLang: A Lightweight Scripting Language

PenguinLang is a compact, expressive scripting language designed for learning how languages work. It supports basic control flow, arithmetic, functions, and structured blocks — all parsed into an abstract syntax tree (AST) and compiled into a simple intermediate representation (IR).

---

## ✨ Features

- Variable declarations with `var`
- Arithmetic and comparison operators
- Control flow: `if`, `else`, `while`
- Function definitions and calls
- Return statements
- String and number literals
- Block scoping with `{}`

---

## 📦 Syntax Overview

### Variables

```penguin
var x = 10;
var name = "Penguin";
```

### Arithmetic

```penguin
var result = x + 5 * 2;
```

### Control Flow

```penguin
if (x > 5) {
  print("x is large");
} else {
  print("x is small");
}
```

### Loops

```penguin
while (x > 0) {
  print(x);
  x = x - 1;
}
```

### Functions

```penguin
func greet(name) {
  print("Hello, " + name);
  return 1;
}

greet("Penguin");
```

---

## 🧠 Token Types

| Token Type             | Description               |
| ---------------------- | ------------------------- |
| `TOKEN_NUMBER`         | Integer literal           |
| `TOKEN_STRING`         | String literal            |
| `TOKEN_IDENTIFIER`     | Variable or function name |
| `TOKEN_OPERATOR`       | `+`, `-`, `*`, `/`        |
| `TOKEN_ASSIGN`         | `=` assignment            |
| `TOKEN_KEYWORD_IF`     | `if` keyword              |
| `TOKEN_KEYWORD_WHILE`  | `while` keyword           |
| `TOKEN_KEYWORD_FUNC`   | `func` keyword            |
| `TOKEN_KEYWORD_RETURN` | `return` keyword          |

---

## 🌲 AST Node Types

| Node Type           | Purpose                  |
| ------------------- | ------------------------ |
| `AST_NUMBER`        | Integer literal          |
| `AST_STRING`        | String literal           |
| `AST_VARIABLE`      | Variable reference       |
| `AST_ASSIGNMENT`    | Variable assignment      |
| `AST_BINARY_OP`     | Arithmetic or comparison |
| `AST_IF`            | Conditional branching    |
| `AST_WHILE`         | Looping construct        |
| `AST_FUNCTION_DEF`  | Function definition      |
| `AST_FUNCTION_CALL` | Function invocation      |
| `AST_RETURN`        | Return statement         |
| `AST_BLOCK`         | Statement grouping       |

---

## 🛠️ Compilation Pipeline

1. **Tokenization**: Converts source code into a stream of tokens.
2. **Parsing**: Builds an AST from tokens.
3. **IR Emission**: Translates AST into intermediate instructions.
4. **Execution**: Runs the IR using a simple virtual machine.

---

## 📤 Example Program

```penguin
var count = 5;
while (count > 0) {
  if (count == 3) {
    print("Halfway there!");
  } else {
    print(count);
  }
  count = count - 1;
}
print("Done!");
```