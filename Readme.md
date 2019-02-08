# Shell

Tyler Drover

An interactive shell program writen in C 

To run:

* `make`
* `make run`

List of commands:

	help 
	history 		    prints out a list of the 20 previously used commands
	cd [dir] 		    changes working directory to dir or HOME if dir is omitted
    setenv <var> [value] 	sets environment variable var to value if exists, otherwise create enironment variable var
    unsetenv <var> 		distroys an environment variable
	env 			    prints list of the current environment variables
	exit 			    terminates current session


Based on tutorial from https://brennan.io/2015/01/16/write-a-shell-in-c/# shell
