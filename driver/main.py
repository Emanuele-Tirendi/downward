import logging
import os
import sys
import importlib

from . import aliases
from . import arguments
from . import cleanup
from . import limits
from . import run_components
from . import util
from . import __version__


def main():
    # Static import of file using python library requires the
    # library to be fully built. If that's not the case, an error
    # occurs. Since the goal is that depending on the config given
    # to build.py either the python library or the C++ (and not both)
    # is used and thus built, one of the following two options
    # are taken into account.

    # The call of the python library is only provisionally here in main().
    if sys.argv[1] == "pybindings-command-line":
        os.system("python3 -c 'from driver import pydownward; pydownward.search()'")
        return
    if sys.argv[1] == "pybindings-dynamic-import":
        pydownward = importlib.import_module('driver.pydownward')
        # pydownward.search()
        return
    
    args = arguments.parse_args()
    logging.basicConfig(level=getattr(logging, args.log_level.upper()),
                        format="%(levelname)-8s %(message)s",
                        stream=sys.stdout)
    logging.debug("processed args: %s" % args)

    if args.version:
        print(__version__)
        sys.exit()

    if args.show_aliases:
        aliases.show_aliases()
        sys.exit()

    if args.cleanup:
        cleanup.cleanup_temporary_files(args)
        sys.exit()

    limits.print_limits("planner", args.overall_time_limit, args.overall_memory_limit)
    print()

    exitcode = None
    for component in args.components:
        if component == "translate":
            (exitcode, continue_execution) = run_components.run_translate(args)
        elif component == "search":
            (exitcode, continue_execution) = run_components.run_search(args)
            if not args.keep_sas_file:
                print("Remove intermediate file {}".format(args.sas_file))
                os.remove(args.sas_file)
        elif component == "validate":
            (exitcode, continue_execution) = run_components.run_validate(args)
        else:
            assert False, "Error: unhandled component: {}".format(component)
        print("{component} exit code: {exitcode}".format(**locals()))
        print()
        if not continue_execution:
            print("Driver aborting after {}".format(component))
            break

    try:
        logging.info(f"Planner time: {util.get_elapsed_time():.2f}s")
    except NotImplementedError:
        # Measuring the runtime of child processes is not supported on Windows.
        pass

    # Exit with the exit code of the last component that ran successfully.
    # This means for example that if no plan was found, validate is not run,
    # and therefore the return code is that of the search.
    sys.exit(exitcode)


if __name__ == "__main__":
    main()
