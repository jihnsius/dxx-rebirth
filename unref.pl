#!/usr/bin/perl -w

use strict;
use warnings(FATAL => qw(all));

$ENV{'PATH'} = '/usr/bin:/bin';

my $fh;
if (!@ARGV) {
	$fh = \*STDIN;
} else {
	my $fn = $ARGV[0];
	if ($fn eq '-') {
		$fh = \*STDIN;
	} else {
		open $fh, '<', $fn or die "Cannot read $fn: $!";
	}
	shift;
}
my %unref;
while(<$fh>) {
	next if ! /undefined reference to `([[:alnum:]_]+)'/;
	$unref{$1} = 1;
}
undef $fh;
my $args = [];
my $dry = 1;
my $pos;
if (@ARGV) {
	if ($ARGV[0] eq ':') {
		$dry = 2;
		shift;
	}
	if ($ARGV[0] eq '-') {
		shift;
		-- $dry;
		$args = ['/usr/bin/git', 'grep', '-n'];
	}
	for ($pos = 0; $pos < @ARGV; ++ $pos) {
		my $v = $ARGV[$pos];
		last if $v eq '--';
		push @$args, $v;
	}
}
for my $k (sort keys %unref) {
	push @$args, '-e', "\\<$k\\>\\s*\\((.*)\$\\)";
}
if (@ARGV) {
	for ($pos..$#ARGV) {
		push @$args, $ARGV[$_];
	}
}
print STDERR '+';
for (@$args) {
	if (m{^[[:alnum:]_./-]+$}) {
		print STDERR " $_";
	} else {
		print STDERR " '$_'";
	}
}
print STDERR "\n";
exit 0 if ($dry);
exit 1 if !%unref;
exec @$args;
