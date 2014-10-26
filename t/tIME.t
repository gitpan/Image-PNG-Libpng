# Tests related to set_tIME and get_tIME functions, which set the
# optional "tIME" chunk of a PNG image.

use warnings;
use strict;
use FindBin;
use Image::PNG::Libpng ':all';
use Image::PNG::Const ':all';
use Test::More tests => 1;

my $png = create_write_struct ();
eval {
    set_tIME ($png);
};
ok (! $@, "set_tIME without a time value");

