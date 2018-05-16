set nocompatible
filetype off
set encoding=utf-8
call vundle#begin()
Plugin 'VundleVim/Vundle.vim'
Plugin 'majutsushi/tagbar'
Plugin 'scrooloose/nerdtree'
Plugin 'Valloric/YouCompleteMe'
call vundle#end()
filetype plugin indent on
autocmd vimenter * NERDTree
nmap <F8> :TagbarToggle<CR>
