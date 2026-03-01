# ⚓ Sink

[English](#english) | [Українська](#українська)

---

![logo](images/log.png)

---

<a name="english"></a>
## 🇬🇧 English

> **Stop syncing, start sinking.** > A lightweight, git-like CLI tool to "push" your code from a comfortable workspace into awkward, hard-to-reach local directories.

### 💡 The Problem
Many development environments (emulators, local web servers, or embedded systems) require your source files to be located in specific, often "hostile" directories. Keeping your project in those system folders is risky, inconvenient for your IDE, and makes version control a nightmare.

### ✨ The Solution
**Sink** allows you to keep your source code in a safe, version-controlled folder. When you're ready to test, just `push` it to the target environment. It's like a direct pipe for your files.

* **Git-like Workflow:** Familiar `init` and `push` commands.
* **Safety Net:** Your source stays safe; the target is just a "sink" (receiver) for your files.
* **Ignore Support:** Use `.sinkignore` to keep `.git`, `.env`, and temp files out of the destination.

### 🚀 Quick Start
1.  **Initialize:** `sink init /path/to/target/` (creates a `.sink` config).
2.  **Configure:** Add unwanted files to `.sinkignore`.
3.  **Push:** Run `sink push` to instantly mirror your files to the target.

---

<a name="українська"></a>
## 🇺🇦 Українська

> **Досить синхронізувати, почни «зливати» (sink it).** > Легкий CLI-інструмент у стилі Git для «пушу» вашого коду зі зручного робочого простору в незручні або важкодоступні локальні директорії.

### 💡 Проблема
Багато середовищ розробки (емулятори, локальні вебсервери або вбудовані системи) вимагають, щоб ваші вихідні файли знаходилися у специфічних, часто «ворожих» системних папках. Тримати проєкт безпосередньо там ризиковано, незручно для IDE, а контроль версій перетворюється на хаос.

### ✨ Рішення
**Sink** дозволяє зберігати вихідний код у безпечній папці з контролем версій. Коли ви готові до тестування, просто «проштовхніть» (push) його в цільове середовище. Це як пряма труба для ваших файлів.

* **Ворклоу як у Git:** Знайомі команди `init` та `push`.
* **Мережа безпеки:** Ваш вихідний код залишається недоторканим; цільова папка — це лише «приймач» (sink) для файлів.
* **Підтримка ігнорування:** Файли `.git`, `.env` та тимчасові файли не потраплять у ціль завдяки `.sinkignore`.

### 🚀 Швидкий старт
1.  **Ініціалізація:** `sink init /шлях/до/цілі/` (створює конфіг `.sink`).
2.  **Налаштування:** Додайте виключення у `.sinkignore`.
3.  **Пуш:** Запустіть `sink push`, щоб миттєво віддзеркалити файли.

---

## 🛠 Commands / Команди

| Command | Description | Опис |
| :--- | :--- | :--- |
| `sink init <path>` | Link current folder to a target | Прив'язати папку до цілі |
| `sink push` | Copy files to target | Копіювати файли до цілі |
| `sink status` | Show current configuration | Показати поточні налаштування |

## 📄 License
This project is licensed under the **(GNU GPL 3.0)**.

---

## 🛠 Build & Installation

Since **Sink** is written in pure C, it has no dependencies and is extremely easy to build on macOS, Linux, or Windows.

### 1. Requirements
* A C compiler (like `gcc` or `clang`)
* `make` (optional, but recommended)

### 2. Compilation
Open your terminal in the project folder and run:

```bash
gcc main.c -o sink
```

---
*Created with Love for developers who hate digging through AppData folders.*