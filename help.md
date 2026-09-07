╔══════════════════════════════════════════════════════════╗
║                 📝 LINE EDITOR                          ║
║              💻 COMMAND-LINE TEXT EDITOR               ║
╚══════════════════════════════════════════════════════════╝

🌟 ABOUT
══════════════════════════════════════════════════════════

LINE EDITOR is a simple command-line text editor written
in C.

It allows users to create, view and modify a small
document using simple commands.

🟢 AVAILABLE COMMANDS
══════════════════════════════════════════════════════════

➕ APPEND
──────────────────────────────────────────────────────────

📌 Command:
append

📝 Purpose:
Adds a new line at the end of the document.

💡 Example:

> append
> Enter line: Hello World

📍 INSERT
──────────────────────────────────────────────────────────

📌 Command:
insert <line_number>

📝 Purpose:
Inserts a new line at the specified position.

💡 Example:

> insert 2
> Enter line: Welcome to Line Editor.

👀 PRINT
──────────────────────────────────────────────────────────

📌 Command:
print

📝 Purpose:
Displays all lines currently stored in the document.

💡 Example:

> print

1: Hello World
2: Welcome to Line Editor.

🗑️ DELETE
──────────────────────────────────────────────────────────

📌 Command:
delete <line_number>

📝 Purpose:
Deletes the specified line from the document.

💡 Example:

> delete 2

✏️ REPLACE
──────────────────────────────────────────────────────────

📌 Command:
replace <line_number>

📝 Purpose:
Replaces an existing line with new text.

💡 Example:

> replace 1
> Enter new line: Learning C Programming

🔎 FIND AND REPLACE
──────────────────────────────────────────────────────────

📌 Command:
findreplace

📝 Purpose:
Finds text in the document and replaces it with
another text.

💡 Example:

> findreplace

Find: C
Replace with: C Programming

❓ HELP
──────────────────────────────────────────────────────────

📌 Command:
help

📝 Purpose:
Displays the available commands and their usage.

💡 Example:

> help

🚪 QUIT
──────────────────────────────────────────────────────────

📌 Command:
quit

📝 Purpose:
Exits the Line Editor safely.

💡 Example:

> quit

⚡ QUICK COMMAND REFERENCE
══════════════════════════════════════════════════════════

➕ append              Add a line at the end
📍 insert <number>     Insert a line
👀 print               Display all lines
🗑️ delete <number>    Delete a line
✏️ replace <number>    Replace a line
🔎 findreplace         Find and replace text
❓ help                 Display help
🚪 quit                 Exit the editor

⚠️ IMPORTANT NOTES
══════════════════════════════════════════════════════════

🔹 Line numbers start from 1.

🔹 Use valid line numbers for INSERT, DELETE and REPLACE.

🔹 The document is stored in memory while the program
is running.

🔹 Invalid commands and inputs are handled safely.

🔹 Memory allocated by the program is released when the
editor exits.

🧪 BASIC WORKFLOW
══════════════════════════════════════════════════════════

> append
> Enter line: Hello World

> append
> Enter line: This is my document.

> print

1: Hello World
2: This is my document.

> replace 2
> Enter new line: Learning C is fun!

> print

1: Hello World
2: Learning C is fun!

> quit


══════════════════════════════════════════════════════════
