# templatious

This library was created to avoid many boilerplates in C++11 code. Examples and documentation is on  http://www.templatious.org/ .
[![Build Status](https://travis-ci.org/davidkazlauskas/tests-templatious.svg?branch=master)](https://travis-ci.org/davidkazlauskas/tests-templatious)
[![Coverage Status](https://coveralls.io/repos/davidkazlauskas/tests-templatious/badge.svg?branch=master)](https://coveralls.io/r/davidkazlauskas/tests-templatious?branch=master)

To use this library you can take these example steps:

Clone this library to some folder (this case called Templatious-Clone

~~~~~~~
mkdir /home/theuser/Templatious-Clone
cd /home/theuser/Templatious-Clone
git clone https://github.com/davidkazlauskas/templatious.git
~~~~~~~

Then, on your favorite C++ buildsystem add the include directory (for instance, cmake)
~~~~~~
include_directories( /home/theuser/Templatious-Clone )
~~~~~~

However, on most of the other repositories for the templatious library (like, test suite, is it fast, decoupled GUI examples) TEMPLATIOUS_DIR enviroment variable is used.

In bash shell:
~~~~~~~
export TEMPLATIOUS_DIR=/home/theuser/Templatious-Clone
~~~~~~~

In cmake:
~~~~~~~
include_directories( $ENV{TEMPLATIOUS_DIR} )
~~~~~~~
