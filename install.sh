#!/usr/bin/env bash
# Bootstrap personal dev environment on a fresh machine.
#
# Idempotent — safe to re-run after edits.
#
# Requires: bash, curl, git, vim (>= 9.0).
# Pulled at runtime: vim-plug + plugins declared in vim/vimrc.
# Expected on PATH at use time (not install time): clangd, fzf, rg.

set -euo pipefail

REPO_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"

log() { printf '[install] %s\n' "$*"; }

# -----------------------------------------------------------------------------
# vim
# -----------------------------------------------------------------------------

log "Linking ~/.vimrc -> $REPO_DIR/vim/vimrc"
ln -sfn "$REPO_DIR/vim/vimrc" "$HOME/.vimrc"

# ctags config: teach universal-ctags the kernel-style asm entry macros
# (ENTRY/WEAK/SYM_FUNC_START) so ENTRY(_main) & co. tag as functions.
# universal-ctags auto-loads every ~/.ctags.d/*.ctags.
log "Linking ~/.ctags.d/asm-macros.ctags -> $REPO_DIR/ctags/asm-macros.ctags"
mkdir -p "$HOME/.ctags.d"
ln -sfn "$REPO_DIR/ctags/asm-macros.ctags" "$HOME/.ctags.d/asm-macros.ctags"

# asm-lsp global config: its presence silences the per-workspace
# "No .asm-lsp.toml config file found" warning.
log "Linking ~/.config/asm-lsp/.asm-lsp.toml -> $REPO_DIR/asm-lsp/asm-lsp.toml"
mkdir -p "$HOME/.config/asm-lsp"
ln -sfn "$REPO_DIR/asm-lsp/asm-lsp.toml" "$HOME/.config/asm-lsp/.asm-lsp.toml"

PLUG_VIM="$HOME/.vim/autoload/plug.vim"
if [[ ! -f "$PLUG_VIM" ]]; then
    log "Installing vim-plug -> $PLUG_VIM"
    curl -fLo "$PLUG_VIM" --create-dirs \
        https://raw.githubusercontent.com/junegunn/vim-plug/master/plug.vim
else
    log "vim-plug already present, skipping download"
fi

log "Installing/updating vim plugins (:PlugInstall)"
vim +'PlugInstall --sync' +qall || true

if command -v nix >/dev/null; then
    log "Installing LSP/tooling via nix profile (no-op if already present)"
    nix profile install nixpkgs#pyright 2>&1 | grep -v "already installed" || true
    # asm-lsp: LSP for .s/.S files (clangd ignores asm).
    # universal-ctags: whole-tree C+asm tag index for :Tags / <C-]>.
    nix profile install nixpkgs#asm-lsp 2>&1 | grep -v "already installed" || true
    nix profile install nixpkgs#universal-ctags 2>&1 | grep -v "already installed" || true
    # jq: used by :MkTags to scope tags to compile_commands.json.
    nix profile install nixpkgs#jq 2>&1 | grep -v "already installed" || true
else
    log "WARNING: nix not found; install pyright-langserver, asm-lsp, universal-ctags manually"
fi

log "Done."
