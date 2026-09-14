# dev-env

Personal dev environment dotfiles + bootstrap.

## Bootstrap on a new machine

```sh
git clone git@github.com:base-jacob/dev-env.git ~/dev-env
~/dev-env/install.sh
```

## What's in here

| Path | What |
|---|---|
| `vim/vimrc` | vim 9 config: yegappan/lsp + fzf.vim, clangd + pyright + asm-lsp LSP, ctags, `<space>`-leader keymaps |
| `install.sh` | Bootstrap — symlinks `~/.vimrc` + `~/.ctags.d/`, installs vim-plug + plugins, installs pyright/asm-lsp/universal-ctags via nix |
| `ctags/asm-macros.ctags` | ctags rules so kernel-style asm macros (`ENTRY`/`WEAK`/`SYM_FUNC_START`) tag as functions |
| `asm-lsp/asm-lsp.toml` | Global asm-lsp config — silences the "no config" warning; sets the default instruction set |

## vim cheatsheet

Leader is `<space>`.

| Keys | Action |
|---|---|
| `gd` / `gr` / `gy` / `gi` | LSP: go to definition / references / type def / impl |
| `K` | LSP: hover |
| `<leader>rn` | LSP: rename symbol |
| `<leader>ca` | LSP: code action |
| `[d` / `]d` | Prev/next diagnostic |
| `<leader>e` | Show all diagnostics for current file |
| `<leader>s` / `<leader>S` | Document / workspace symbol search |
| `<leader>f` | Fuzzy find files |
| `<leader>b` | Fuzzy switch buffers |
| `<leader>g` | Ripgrep across project |
| `<leader>l` | Fuzzy lines in current buffer |
| `<leader>h` | Recent files |
| `<C-]>` / `g<C-]>` | Jump to tag / list tag choices (C + asm labels) |
| `<leader>t` | Fuzzy-search tags via fzf |
| `:MkTags` | Build tags in cwd — scoped to `compile_commands.json` when present (drops unreachable defs), else full tree |

Assembly (`.s`/`.S`): `K` and in-file `gd`/`gr` come from **asm-lsp** (clangd
ignores asm). For cross-file jumps, `gd` falls back to the **ctags** index, so it
resolves targets in other files and macro-defined entry points like `ENTRY(_main)`
too. Run `:MkTags` once at a project root to build `tags`; `<C-]>` also jumps via
tags directly.
