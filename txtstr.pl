#!/usr/bin/perl -w

use strict;
use warnings(FATAL => qw(all));

die if (@ARGV < 2);
open my $fstr, '<', $ARGV[0] or die "Cannot read $ARGV[0]: $!\n";
open my $fhdr, '<', $ARGV[1] or die "Cannot read $ARGV[1]: $!\n";
my %s;
while(<$fstr>) {
	die if ! /^(\d+)\t(".*")$/;
	$s{$1} = $2;
}
undef $fstr;
while(<$fhdr>) {
	&munge();
	print;
}

sub munge {
	return if ! /Text_string\[\s*(\d+)\]/;
	die "Unknown string $1" if !defined($s{$1});
	s/Text_string\[\s*(\d+)\]/dxx_gettext($1, $s{$1})/;
}
