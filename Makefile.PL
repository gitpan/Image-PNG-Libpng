use strict;
use warnings;
use ExtUtils::MakeMaker;
use ExtUtils::Constant;

use 5.008001;

use lib 'inc';

use Devel::CheckLib;

check_lib_or_exit (lib => 'png', header => 'png.h');

my $verbose ;

WriteMakefile (
    NAME         => 'Image::PNG::Libpng',
    VERSION      => '0.28_04',
    ABSTRACT     => 'Perl interface to libpng',
    OBJECT       => 'Libpng.o',
    LIBS         => '-lpng',
#    INC          => $inc,
    AUTHOR => 'Ben Bullock <bkb@cpan.org>',
    LICENSE => 'perl',
    META_MERGE => {
        resources => {
            "MailingList" => "http://groups.google.com/group/perlimagepng",
            "bugtracker" => "https://github.com/benkasminbullock/image-png-libpng/issues",
            "repository" => "https://github.com/benkasminbullock/image-png-libpng",
        },
        no_index => {
            directory => ['tmpl', 'build'],
        },
    },
    MIN_PERL_VERSION => '5.008001',
    EXE_FILES => ['script/pnginspect',],
#    OPTIMIZE => '-Wall -O',
);

my @names = (qw());

ExtUtils::Constant::WriteConstants(
    NAME         => 'Image::PNG::Libpng',
    NAMES        => \@names,
    DEFAULT_TYPE => 'IV',
    C_FILE       => 'const-c.inc',
    XS_FILE      => 'const-xs.inc',
);

if ($verbose) {
    print "I have successfully completed my job.\n";
}

# The following subroutine adds extra necessary rules to the end of
# "Makefile". See
# http://search.cpan.org/dist/ExtUtils-MakeMaker/lib/ExtUtils/MakeMaker.pm#Overriding_MakeMaker_Methods

sub MY::postamble
{
    my $cfunctions_build = '';

    if ( -d ".git") {
	$cfunctions_build .= <<EOF;

lib/Image/PNG/Libpng.pm:	tmpl/Libpng.pm.tmpl build/make-files.pl
	build/make-files.pl

lib/Image/PNG/Libpng/Libpng.pm:	tmpl/Libpng.pm.tmpl build/make-files.pl tmpl/Libpng.xs.tmpl
	build/make-files.pl

Libpng.xs:   tmpl/Libpng.xs.tmpl tmpl/config build/make-files.pl
	build/make-files.pl

lib/Image/PNG/Libpng/Const.pm:      tmpl/Const.pm.tmpl build/strip-constants.pl
	build/strip-constants.pl
EOF

    }   
    return $cfunctions_build;
}

sub find_program
{
    my ($program, $verbose) = @_;
    if ($verbose) {
        print "I am going to look for $program in the list of directories in your PATH.\n";
    }
    my $found;
    if ($ENV{PATH}) {
        my @path = split /:/, $ENV{PATH};
        for my $dir (@path) {
            if ($verbose) {
                print "Looking in '$dir' for '$program': ";
            }
            my $dprogram = "$dir/$program";
            if (-f $dprogram && -x $dprogram) {
                if ($verbose) {
                    print "Found.\n";
                }
                $found = $dprogram;
                last;
            }
            if ($verbose) {
                print "Not found.\n";
            }
        }
    }
    else {
        if ($verbose) {
            print "There is no PATH environment variable.\n";
        }
    }
    return $found;
}

# Local Variables:
# mode: perl
# End:
