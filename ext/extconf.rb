#!/usr/bin/env ruby

require 'mkmf'

$warnflags.gsub! /-Wdeclaration-after-statement/, ''
$CFLAGS << " -Wall -std=c99 -Werror"

have_header("ruby/io.h")

create_makefile("markov")