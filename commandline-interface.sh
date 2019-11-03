#!/bin/bash




# Flags and variables which will be used for parsing command line options.
program_name=$(basename $0)
executable=./sgpf
params=""
help_flag=false
no_arguments_flag=false

# Default program parameters
lower_bound=0
upper_bound=1000000
memory_usage=100000000 # 100MB

function print_help
{
	cat <<EOF
$program_name - Compute the greatest prime factor of each integer within a
specified interval, and then add it all up.

usage: $program_name -h
usage: $program_name -a [LOWER BOUND]
usage: $program_name -b [UPPER BOUND]
usage: $program_name -m [MEMORY USAGE]

Options:
-h, --help      display help message and exit
-m              specify the memory usage in bytes (default 100MB)
-a              specify the inclusive lower bound (default 0)
-b              specify the inclusive upper bound (default 1M)

NOTE: Integers can be written in the following format:
	1k      = 1000
	23M     = 23000k
	3G      = 3000M
	45T     = 45000G
	123P	= 123000T
But decimals are not allowed.
EOF
}

# Check if variable is empty, and exit if it is.
function empty_check
{
	if [ -z "$1" ]; then
		echo Error! No argument specified. Exiting...
		exit -1
	fi
}

# This function accepts a string, and checks it it's numeric, or of the
# following form:
#	100k
#	12M
#	1294G
#	129T
#	23P
function expand_integer
{
	local str=$1
	local base=${str%?}
	local suffix=${str: -1}

	# If it's completely numeric, then just return the same string
	if [[ $str =~ ^[0-9]+$ ]]; then
		echo $str
	# If not, let's check if it's numeric,
	# but with a numeric suffix at the end.
	elif [[ $base =~ ^[0-9]+$ ]]; then
		case $suffix in
			k)	echo ${base}000;;
			M)	echo ${base}000000;;
			G)	echo ${base}000000000;;
			T)	echo ${base}000000000000;;
			P)	echo ${base}000000000000000;;
			*)
				echo "Unrecognized suffix \'$suffix\'! Exiting..."
				exit -1;;
		esac
	else
		echo Unrecognized argument \'$str\'! Exiting...
		exit -1
	fi
}

# Once the bounds have been passed through expand_integer(), this function
# makes sure that the lower bound is not greater than the upper bound,
# and that the arguments are not empty.
#
# NOTE: The input parameters are assumed to have already been
# passed to expand_integer()
function sanity_check
{
	a=$1
	b=$2
	mem=$3

	if (( $a > $b )); then
		echo Error! Lower bound CANNOT be greater than upper bound. Exiting...
		exit -1
	elif [ -z "$a" ]; then
		echo Error! No argument provided for the lower bound! Exiting...
		exit -1
	elif [ -z "$b" ]; then
		echo Error! No argument provided for the upper bound! Exiting...
		exit -1
	elif [ -z "$mem"]; then
		echo Error! No argument provided for the memory usage! Exiting...
		exit -1
	fi
}

# Take the flags, and decide what to do with the program.
function parse_flags
{
	if $help_flag; then
		print_help
		exit
	else
		a=$(expand_integer $lower_bound)
		b=$(expand_integer $upper_bound)
		mem=$(expand_integer $memory_usage)
		sanity_check $a $b $mem

		$executable $a $b $mem
	fi
}

# Sift through all the command line options, and set the appropriate flags
# and parameters based on these options.
while (( "$#" )); do
	case "$1" in
		-h|--help)
			help_flag=true
			shift
			;;
		-a)
			lower_bound="$2"
			empty_check $lower_bound
			shift 2
			;;
		-b)
			upper_bound="$2"
			empty_check $upper_bound
			shift 2
			;;
		-m)
			memory_usage="$2"
			empty_check $memory_usage
			shift 2
			;;
		--) # end argument parsing
			shift
			break
			;;
		-*|--*=) # unsupported flags
			echo "Error: Unsupported flag $1" >&2
			exit 1
			;;
		*) # preserve positional arguments
			params="$params $1"
			shift
			;;
	esac
done

parse_flags





# If no initial arguments were provided, let the user know that they can access
# a list of command line options
if (( $# == 0 )); then
	echo -e "\n\n"
	echo "No arguments provided; using default values."
	echo "Use -h or --help to see a list of command line options."
fi
