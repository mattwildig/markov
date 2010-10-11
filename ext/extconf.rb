require 'mkmf'

$CFLAGS << " -Wall -std=c99 -Werror"

create_makefile("markov_native")