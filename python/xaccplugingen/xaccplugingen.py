import argparse
import os

PLUGIN_TYPES = {"compiler"}


def add_subparser(subparsers):
    """Add the 'generate-plugin' subparser to subparsers."""
    subparser = subparsers.add_parser("generate-plugin", help="A utility for generating plugin skeletons.",
                                      formatter_class=argparse.RawDescriptionHelpFormatter)
    subparser.add_argument("-l", "--libname",
                           help="library name (e.g. xacc-quilcompiler, which would be the default if run with '-n QuilCompiler')")

    required_group = subparser.add_argument_group("required arguments")
    required_group.add_argument("-t", "--type", help="plugin type", required=True, choices=PLUGIN_TYPES)
    required_group.add_argument("-n", "--name", help="plugin name (e.g. QuilCompiler)", required=True)


def run(args):
    """Run the appropriate generator given args."""
    if not args.libname:
        args.libname = "xacc-{}".format(args.name.lower())
    os.mkdir(args.libname)
    templates_dir = os.path.join(os.path.dirname(__file__), "plugin_templates/")
    output_dir = os.path.join(os.getcwd(), args.libname)

    if args.type.lower() == "compiler":
        generate_compiler(templates_dir, output_dir, args)


def generate_compiler(templates_dir, output_dir, args):
    plugin_template_dir = os.path.join(templates_dir, "compiler/")
    for dirpath, _, filenames in os.walk(plugin_template_dir):
        for filename in filenames:
            with open(os.path.join(dirpath, filename), 'r') as template_file:
                contents = template_file.read()
            dir_rel_path = os.path.relpath(dirpath, plugin_template_dir)
            outfile_name = filename.format(class_name=args.name)
            contents = contents.format(lib_name=args.libname, class_name=args.name, class_name_lower=args.name.lower(),
                                       class_name_upper=args.name.upper())
            with open(os.path.join(output_dir, dir_rel_path, outfile_name), 'w') as outfile:
                outfile.write(contents)
