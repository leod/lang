#!/usr/bin/python

from fabricate import *

setup(dirs=['.', 'compiler', '.obj'])

sources = ['parser/parser',
           'common/diagnostics',
		   'main',
		   'semantic/ast_visitor',
		   'semantic/scope',
		   'lexer/token',
		   'lexer/lexer',
		   'ast/declaration',
		   'ast/print_visitor',
		   'semantic/semantic_visitor',
		   'codegen/llvm/codegen']

cflags = '-Icompiler -Wall -g -pedantic -Wextra -Wformat -Wconversion -std=c++0x'.split()
lflags = '-L/usr/lib/llvm -lstdc++ -lLLVM-2.7'.split()

def path_to_object_file(path):
	return '.obj/' + path.replace('/', '_') + '.o'

def build():
    compile()
    link()

def compile():
    for source in sources:
        run('gcc', '-c', 'compiler/'+source+'.cpp', '-o', path_to_object_file(source), cflags)

def link():
    objects = [path_to_object_file(s) for s in sources]
    run('gcc', '-o', 'llc', objects, lflags)

def clean():
    autoclean()

main()
