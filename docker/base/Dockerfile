from ubuntu:18.04
run rm /bin/sh && ln -s /bin/bash /bin/sh && apt-get -y update && apt-get install -y gcc g++ cmake git libssl-dev \
      python3 libpython3-dev python3-pip vim exuberant-ctags gdb gfortran libblas-dev liblapack-dev pkg-config wget \
    && rm -rf /var/lib/apt/lists/* \
    && git clone https://github.com/VundleVim/Vundle.vim.git ~/.vim/bundle/Vundle.vim \ 
    && echo "set nocompatible\nfiletype off\nset encoding=utf-8\nset rtp+=~/.vim/bundle/Vundle.vim\ncall vundle#begin()" >> ~/.vimrc \ 
    && echo "Plugin 'VundleVim/Vundle.vim'\nPlugin 'Valloric/YouCompleteMe'" >> ~/.vimrc \
    && echo "Plugin 'majutsushi/tagbar'\nPlugin 'scrooloose/nerdtree'" >> ~/.vimrc \ 
    && echo "Plugin 'octol/vim-cpp-enhanced-highlight'\ncall vundle#end()" >> ~/.vimrc \ 
    && echo "filetype plugin indent on\nautocmd vimenter * NERDTree\nnmap <F8> :TagbarToggle<CR>" >> ~/.vimrc \
    && echo "let g:cpp_class_scope_highlight = 1\nlet g:cpp_member_variable_highlight = 1\n" >> ~/.vimrc \ 
    && echo "let g:cpp_class_decl_highlight = 1\nlet g:cpp_experimental_simple_template_highlight = 1\n" >> ~/.vimrc && cat ~/.vimrc && vim +PluginInstall +qall \
    && cd ~/.vim/bundle/YouCompleteMe && python3 install.py --clang-completer \
    && vim +PluginInstall +qall
workdir /projects
