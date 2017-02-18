set nocompatible
filetype off

set rtp+=~/.vim/bundle/Vundle.vim
call vundle#begin()

Plugin 'VundleVim/Vundle.vim'
Bundle 'majutsushi/tagbar'
"Plugin 'Valloric/YouCompleteMe'
Plugin 'scrooloose/nerdtree'
Plugin 'ervandew/supertab'
call vundle#end()            
filetype plugin indent on  


inoremap <expr> <C-Space> pumvisible() \|\| &omnifunc == '' ?
\ "\<lt>C-n>" :
\ "\<lt>C-x>\<lt>C-o><c-r>=pumvisible() ?" .
\ "\"\\<lt>c-n>\\<lt>c-p>\\<lt>c-n>\" :" .
\ "\" \\<lt>bs>\\<lt>C-n>\"\<CR>"
imap <C-@> <C-Space>

aug QFClose
  au!
  au WinEnter * if winnr('$') == 1 && getbufvar(winbufnr(winnr()), "&buftype") == "quickfix"|q|endif
aug END

let g:clang_library_path='/usr/lib/llvm-3.8/lib'
let g:clang_periodic_quickfix=1
let g:clang_complete_copen=1
let g:clang_snippets=1
let g:clang_complete_optional_args_in_snippets=1
let g:clang_complete_patterns=1
let g:clang_complete_macros=1
let g:clang_auto_select=0

colorscheme lucius
let g:ycm_confirm_extra_conf = 0
map <F10> :call g:ClangUpdateQuickFix()<CR>
map <C-n> :NERDTreeToggle<CR>
nmap <F8> :TagbarToggle<CR>
map  <C-p> :tabn<CR>
map  <C-o> :tabp<CR>
map  <C-Right> :tabn<CR>
map  <C-Left> :tabp<CR>
set mouse=a " enable mouse for tabs and pasting etc
set number  " line numbers

syn on se title
set tabstop=4 softtabstop=4 shiftwidth=4

set noexpandtab

set completeopt-=preview " dont show scratchpad
set completeopt=longest,menuone
set listchars=tab:\|\ " show pipes for scopes
set list
highlight SpecialKey ctermfg=249

set tags=./tags;/


augroup auto_comment
    au!
    au FileType * setlocal formatoptions-=c formatoptions-=r formatoptions-=o
augroup END

set pastetoggle=<F2>

set clipboard=unnamedplus " work with non-default vim packages (vim-gnome)
