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
| `vim/vimrc` | vim 9 config: yegappan/lsp + fzf.vim, clangd LSP, `<space>`-leader keymaps |
| `install.sh` | Bootstrap — symlinks `~/.vimrc`, installs vim-plug, runs `:PlugInstall` |

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
