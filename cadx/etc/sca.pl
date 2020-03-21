#!/bin/perl
#
#
# File:	sca.pl
#
# Copyright 1998 Eastman Kodak Company
#
# Description:
# This script does a rough source code analysis of C++ source code supplied
# on stdin.
#
# Author: Andy Blose
#
# Limitations: Doesn't handle old-style C comments /* */
#

$lineCount = 0;
$commentOnlyLines = 0;
$comments = 0;

while (<>) {
	chomp;

	# Only count and process lines that contain characters.
	#
	if (/\S/) {
		$lineCount++;

		# Count lines beginning with //
		#
		if (/^\/\/.*/) {
			$commentOnlyLines++;
			$comments++;
		}
		# Also count lines containing a // anywhere
		#
		elsif (/^.*\/\/.*/) {
			$comments++;
		}
	}
} # while

print "Total Lines: $lineCount\n";
print "Comment Only Lines: $commentOnlyLines\n";
print "Commented Lines: $comments\n";
printf "Comment Percentage: %d%%\n", ($comments / $lineCount * 100);
