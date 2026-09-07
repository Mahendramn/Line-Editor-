# 📝 LINE EDITOR

### 💻 A Simple Command-Line Text Editor Built in C

> 🚀 A lightweight terminal-based line editor that allows users to **create, view, insert, delete, replace, and search text** using simple commands.

---

## 🌟 About the Project

**LINE EDITOR** is a command-line text editor developed using the **C programming language**.

Instead of using a graphical interface, the user interacts with the editor directly through the **terminal**.

The program stores the document in memory and allows the user to modify it using simple commands.

### 🎯 Main Goal

The main goal of this project is to understand and apply:

* 🧠 Data structures
* 📌 Pointers
* 💾 Dynamic memory allocation
* 🔤 Strings in C
* ⚙️ Command-based program design
* 🛡️ Error handling
* 🧪 Testing and debugging

---

## ✨ Features

| Command          | Description                               |
| ---------------- | ----------------------------------------- |
| ➕ `append`       | Add a new line at the end of the document |
| 📍 `insert`      | Insert a new line at a specific position  |
| 👀 `print`       | Display all lines in the document         |
| 🗑️ `delete`     | Delete a specific line                    |
| ✏️ `replace`     | Replace an existing line                  |
| 🔎 `findreplace` | Find and replace text in the document     |
| ❓ `help`         | Display all available commands            |
| 🚪 `quit`        | Exit the editor                           |

---

## 🧩 How It Works

The editor follows a simple command-based flow:

```text
        👤 User
          │
          ▼
   ⌨️ Enter Command
          │
          ▼
   🔍 Command Processing
          │
          ▼
   📍 Identify Line / Text
          │
          ▼
   ⚙️ Perform Operation
          │
          ▼
   📄 Updated Document
```

For example:

```text
> append
Enter line: Hello World

> append
Enter line: Welcome to C

> print

1: Hello World
2: Welcome to C
```

---

## 🗂️ Data Structure

### 📦 Dynamic Array of Strings

The project uses a **dynamic array of strings** to store the document.

Conceptually, the document looks like:

```text
lines
 │
 ├── [0] ──> "Hello World"
 ├── [1] ──> "Welcome to C"
 └── [2] ──> "Line Editor"
```

The program maintains three important values:

| Variable   | Purpose                                      |
| ---------- | -------------------------------------------- |
| `lines`    | Stores the text lines                        |
| `count`    | Number of lines currently stored             |
| `capacity` | Number of lines the allocated array can hold |

### 📈 Why Dynamic Array?

A fixed-size array would limit the number of lines.

A dynamic array allows the program to increase its size when more lines are added.

```text
Initial Capacity
      │
      ▼
   [ ][ ][ ][ ][ ]
      │
      │ More lines added
      ▼
   realloc()
      │
      ▼
[ ][ ][ ][ ][ ][ ][ ][ ][ ][ ]
```

This makes the editor flexible while keeping the implementation simple.

---

## 🧠 Why We Chose This Data Structure

We chose a **dynamic array of strings** because:

* ⚡ Fast access to a line using its index
* 📈 Can grow when more lines are added
* 🧩 Simple to implement in C
* 💾 Works well with `malloc()` and `realloc()`
* 📄 Suitable for a small text document
* 🔧 Easy to modify and manage

### ⚖️ Trade-off

When inserting or deleting a line, some existing lines may need to be shifted.

For a small line editor, this is acceptable and keeps the overall design simple.

---

## 💾 Memory Management

Dynamic memory is an important part of this project.

The program uses:

### `malloc()`

Used to allocate memory for new strings and data structures.

### `realloc()`

Used to increase the size of the dynamic array when more space is required.

### `free()`

Used to release memory that is no longer required.

```text
malloc()
   │
   ▼
Allocate Memory
   │
   ▼
Use Memory
   │
   ▼
realloc() ──> Grow when required
   │
   ▼
free()
   │
   ▼
Release Memory
```

The program also frees the allocated memory when the editor is closed.

---

## 🛡️ Error Handling

The editor checks for common invalid situations.

### It handles:

* ❌ Invalid commands
* 🔢 Invalid line numbers
* 📄 Empty documents
* ⌨️ Invalid input
* 💥 Memory allocation failure
* 🚫 Attempting to modify a non-existing line

The goal is to make sure that invalid input **does not cause the program to crash**.

---

## 🧪 Testing

The program is tested using different types of inputs.

### ✅ Normal Cases

* Adding multiple lines
* Printing the document
* Replacing a line
* Finding and replacing text

### 🔍 Edge Cases

* Empty document
* Deleting the only line
* Inserting at the beginning
* Inserting at the end
* Invalid line number
* Non-existing line
* Invalid command
* Large input lines

### 🧠 Memory Tests

* Adding and deleting lines
* Growing the dynamic array
* Exiting after creating multiple lines
* Checking that allocated memory is released

---

## 🖥️ Technologies Used

<p align="center">

### 🔧 C Programming Language

💻 Command Line / Terminal
🧠 Dynamic Arrays
📌 Pointers
💾 Dynamic Memory Allocation
🔤 String Manipulation

</p>

---

## 🚀 Getting Started

### 📋 Requirements

Before running the project, make sure you have:

* 💻 A computer
* ⚙️ GCC compiler
* 📝 Git (optional, for cloning the repository)

---

## 📥 Clone the Repository

```bash
git clone <YOUR-GITHUB-REPOSITORY-URL>
```

Move into the project folder:

```bash
cd LINE-EDITOR
```

---

## 🔨 Compile the Program

Using GCC:

```bash
gcc -Wall -Wextra -Wpedantic -std=c11 main.c -o line_editor
```

### 🪟 Windows

```bash
gcc -Wall -Wextra -Wpedantic -std=c11 main.c -o line_editor.exe
```

### 🐧 Linux / macOS

```bash
gcc -Wall -Wextra -Wpedantic -std=c11 main.c -o line_editor
```

---

## ▶️ Run the Program

### 🪟 Windows

```bash
.\line_editor.exe
```

### 🐧 Linux / macOS

```bash
./line_editor
```

---

## 🎮 Example Usage

```text
========================================
          LINE EDITOR
========================================

> append
Enter line: Hello World

> append
Enter line: This is my document.

> print

1: Hello World
2: This is my document.

> insert 2
Enter line: Welcome to Line Editor.

> print

1: Hello World
2: Welcome to Line Editor.
3: This is my document.

> replace 2
Enter new line: Learning C is interesting.

> print

1: Hello World
2: Learning C is interesting.
3: This is my document.

> delete 1

> print

1: Learning C is interesting.
2: This is my document.

> quit
```

---

## 📁 Project Structure

```text
LINE-EDITOR/
│
├── 📄 main.c
│
├── 📖 README.md
│
└── 📄Design.docs
```

> 📌 The project structure may change as new files or documentation are added.

---

## 📚 Documentation

The project includes documentation explaining:

* 📌 Project design
* 🧠 Data structure selection
* ⚙️ Available commands
* 💾 Memory management
* 🛡️ Error handling
* 🧪 Testing approach

For command details, use:

```text
> help
```

inside the editor.

---

## 🎓 Learning Outcomes

Through this project, we learned how to work with:

* 🔹 Pointers
* 🔹 Pointer-to-pointer concepts
* 🔹 Strings
* 🔹 Dynamic arrays
* 🔹 Structures
* 🔹 `malloc()`
* 🔹 `realloc()`
* 🔹 `free()`
* 🔹 Command-line input
* 🔹 String processing
* 🔹 Error handling
* 🔹 Debugging

---

## 👥 Team

| Role                | Member               |
| ------------------- | -------------------- |
| 👨‍💻 Team Member 1 | Mahendra M N           |
| 👨‍💻 Team Member 2 | Manoj L                |
| 👨‍💻 Team Member 3 | Ramappa Kumar Chougala |

---

## 📌 Project Status

🟢 **Project Development**

Core line-editing features have been implemented and tested.

More improvements and features may be added in future versions.

---

## ⭐ Acknowledgement

This project was created as part of our academic work to gain practical experience with **C programming, data structures, dynamic memory management, and software development**.

---

<div align="center">

### 📝 LINE EDITOR

**Simple • Lightweight • Command-Line Based • Built with C**

⭐ If you found this project useful, consider giving the repository a star!

</div>
