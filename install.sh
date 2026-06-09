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
    log "Installing pyright via nix profile (no-op if already present)"
    nix profile install nixpkgs#pyright 2>&1 | grep -v "already installed" || true
else
    log "WARNING: nix not found; install pyright-langserver manually for Python LSP"
fi

log "Done."
