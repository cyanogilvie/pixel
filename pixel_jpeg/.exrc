set nocompatible
let s:cpo_save=&cpo
set cpo&vim
map! <xHome> <Home>
map! <xEnd> <End>
map! <S-xF4> <S-F4>
map! <S-xF3> <S-F3>
map! <S-xF2> <S-F2>
map! <S-xF1> <S-F1>
map! <xF4> <F4>
map! <xF3> <F3>
map! <xF2> <F2>
map! <xF1> <F1>
map [37~ 
map [29~ 
vnoremap p :let current_reg = @"gvdi=current_reg
map <F2> mj1G\/^classw\/\ €klY\'jibody\ p$a\ {}\ {}\'j\/{€kli::a
map <xHome> <Home>
map <xEnd> <End>
map <S-xF4> <S-F4>
map <S-xF3> <S-F3>
map <S-xF2> <S-F2>
map <S-xF1> <S-F1>
map <xF4> <F4>
map <xF3> <F3>
map <xF2> <F2>
map <xF1> <F1>
let &cpo=s:cpo_save
unlet s:cpo_save
set autoindent
set backspace=2
set helpfile=/usr/share/vim/vim60/doc/help.txt.gz
set history=50
set ignorecase
set incsearch
set ruler
set shiftround
set shiftwidth=4
set smartcase
set suffixes=.bak,~,.swp,.o,.info,.aux,.log,.dvi,.bbl,.blg,.brf,.cb,.ind,.idx,.ilg,.inx,.out,.toc
if &syntax != 'c'
set syntax=c
endif
set tabstop=4
set tags=~/ldev/tags
set viminfo='100,f1,\"100
set virtualedit=block
set visualbell
