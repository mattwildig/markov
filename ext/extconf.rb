require 'mkmf'

$CFLAGS << " -Wall -std=c99"

have_header("ruby/io.h")

create_makefile("markov_native")