import os
import sys
import time
import subprocess
import string

# configuration parameters:
Translator_tests = os.environ["HOME"]+"/cs245/A-folder-of-examples/TranslatorTests/"
optional_newline = ""  # set to "" or to "\n    " to omit or include line breaks within tests
file_name_display_format = "%-30s"
input_display_format = "%-24s"


def run_command(command):
    subprocess.check_call(command.split(" "))

def run_command_with_input_from(command, input_file):
    stream = open(input_file, 'r')
    subprocess.check_call(command.split(" "), stdin=stream)
    stream.close()



        

def get_output(command):
    return subprocess.check_output(command.split(" "))

def get_tests_of_type(path, suffix):
    all = os.listdir(Translator_tests + path)
    def match_suffix(name):
        return len(name) > len(suffix) and name[-len(suffix):] == suffix
    def attach_path(name):
        return path+name
    return map(attach_path, filter(match_suffix, all))

def do_tests():
    good1 = get_tests_of_type("davew/", ".scm")
    good2 = get_tests_of_type("others/", ".scm")
    good1.sort()
    good2.sort()
    good = good1 + good2
    

    print "=== Running", len(good) ,"tests that should give the right answer ==="
            
    for test in good:
        print "Running test: ", file_name_display_format % (test), "which is",
        try:
            scheme = open(Translator_tests + test, 'r').read()
            print (optional_newline +
                   "'" + input_display_format % (string.replace(scheme, '\n', ' ')) +
                   "', " + optional_newline + "got: "),
            sys.stdout.flush()
        except:
            print "OOPS ... can't read file"

        try:
            run_command_with_input_from("Debug/Translator",Translator_tests + test)
        except:
            print " ******************* THE TRANSLATOR RETURNED AN ERROR CODE RATHER THAN return 0 FROM MAIN OR exit(0) BUT SHOULD NOT HAVE **************** "

    errs1 = get_tests_of_type("davew/", ".err")
    errs2 = get_tests_of_type("others/", ".err")
    errs1.sort()
    errs2.sort()
    errs = errs1 + errs2

    print " "
    print "=== Running", len(errs) ,"tests that should give the wrong answer ==="

    for test in errs:
        print "Running   error test: ", file_name_display_format % (test),"which is",
        try:
            scheme = open(Translator_tests + test, 'r').read()
            print (optional_newline +
                   "'" + input_display_format % (string.replace(scheme, '\n', ' ')) +
                   "', " + optional_newline + "got: "),
            sys.stdout.flush()
        except:
            print "OOPS ... can't read file"

        try:
            run_command_with_input_from("Debug/Translator",Translator_tests + test)
            print " ****************** THE TRANSLATOR SHOULD HAVE RETURNED A NON-ZERO RESULT VIA return FROM MAIN OR EXIT ***************"
        except:
            "do nothing"


if __name__ == "__main__": do_tests()

