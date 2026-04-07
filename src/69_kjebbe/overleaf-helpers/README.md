# LaTeX Appendix Generator

A bash script that generates LaTeX appendix files with syntax-highlighted code listings using the `minted` package.

## Files

- `generate_appendix.sh` - Main bash script
- `files_to_include.example.txt` - Example input format

## Usage

1. **Create your file list** (e.g., `my_files.txt`):
   ```
   # Main Section
   ## Subsection Title
   ### Subsubsection Title
   path/to/file1.c
   path/to/file2.h
   
   ## Another Subsection
   path/to/file3.asm
   ```

2. **Run the script**:
   ```bash
   ./generate_appendix.sh my_files.txt > appendix.tex
   ```

3. **Copy `appendix.tex`** into your Overleaf project

## File List Format

The input file uses markdown-style headers to structure your appendix:

- `# Title` → `\section{Title}`
- `## Title` → `\subsection{Title}`
- `### Title` → `\subsubsection{Title}`
- Regular lines → file paths to include as listings
- Empty lines are ignored
- Lines starting with `//` are comments (ignored)

## Language Detection

The script auto-detects the language for syntax highlighting:

| Extension | Language |
|-----------|----------|
| `.c`, `.h` | c |
| `.cpp`, `.hpp`, `.cxx`, `.cc` | cpp |
| `.asm`, `.s` | nasm |
| `.py` | python |
| `.sh` | bash |
| `.js` | javascript |
| `.java` | java |
| `.rs` | rust |
| `.go` | go |
| others | text |

## Features

- Preserves exact `minted` styling (linenos, breaklines, frame, etc.)
- Auto-generates captions: "Implementation of `filename.c`"
- Auto-generates labels: `lst:filename_c`
- Escapes special LaTeX characters automatically
- Warns about missing files (but continues processing)
- Supports UTF-8 encoded files

## Minted Styling

The generated listings use these exact minted options:

```latex
\begin{minted}[
    linenos,
    breaklines,
    frame=single,
    framesep=3mm,
    baselinestretch=1.1,
    numbersep=6pt,
    fontsize=\footnotesize,
    tabsize=4,
    bgcolor=white,
    style=friendly,
]{language}
```

## Prerequisites

- bash 4.0+
- `minted` package in your LaTeX document
- Pygments installed (for minted syntax highlighting)

To compile the generated LaTeX:

```bash
pdflatex -shell-escape appendix.tex
```

## Example Output

The generated `appendix.tex` will look like:

```latex
\documentclass[../main.tex]{subfiles}
\begin{document}

\section{Appendix}
\label{sect:appendix}

\subsection{Boot and GDT Implementation}
\label{apx:boot_and_gdt_implementation}

\subsubsection{GDT Header File}
\label{apx:gdt_header_file}

\begin{listing}[H]
    \begin{minted}[
        linenos,
        breaklines,
        frame=single,
        framesep=3mm,
        baselinestretch=1.1,
        numbersep=6pt,
        fontsize=\footnotesize,
        tabsize=4,
        bgcolor=white,
        style=friendly,
    ]{c}
    // Your code here
    \end{minted}
    \caption{Implementation of \texttt{gdt.h}}
    \label{lst:gdt_h}
\end{listing}

\end{document}
```

## License

MIT - Use it, modify it, enjoy it!
